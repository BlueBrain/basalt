#include <dirent.h>

#include "edge_iterator_impl.hpp"
#include "graph_impl.hpp"
#include "vertex_iterator_impl.hpp"


#include <gsl-lite/gsl-lite.hpp>
#include <rocksdb/db.h>
#include <rocksdb/filter_policy.h>
#include <rocksdb/slice_transform.h>
#include <rocksdb/statistics.h>
#include <rocksdb/table.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace basalt {

/**
 * \name RocksDB write policies helper
 * \{
 */

inline static const rocksdb::ReadOptions& default_read_options() {
    static const rocksdb::ReadOptions default_read_options;
    return default_read_options;
}

inline static const rocksdb::WriteOptions& write_options(bool commit) {
    if (commit) {
        static const rocksdb::WriteOptions sync_write = []() {
            rocksdb::WriteOptions eax;
            eax.sync = true;
            return eax;
        }();
        return sync_write;
    }
    static const rocksdb::WriteOptions async_write;
    return async_write;
}

/**
 * \}
 */

inline static const rocksdb::Options& db_options() {
    static const rocksdb::Options& db_options = []() {
        rocksdb::Options options;
        // FIXME set it on column family level
        options.prefix_extractor.reset(
            rocksdb::NewFixedPrefixTransform(1 + sizeof(vertex_t) + sizeof(vertex_id_t)));
        options.create_if_missing = true;
        options.max_open_files = -1;
        options.statistics = rocksdb::CreateDBStatistics();
        return options;
    }();
    return db_options;
}

inline static const rocksdb::ColumnFamilyOptions& vertices_cfo() {
    static const rocksdb::ColumnFamilyOptions vertices_cfo = []() {
        rocksdb::ColumnFamilyOptions options;
        // optimize vertex prefix enumeration to look for all vertices of a particular
        // type
        options.prefix_extractor.reset(rocksdb::NewFixedPrefixTransform(1 + sizeof(vertex_t)));
        options.write_buffer_size = 128u << 20u;      // 128MB
        options.target_file_size_base = 128u << 20u;  // 128MB
        options.max_bytes_for_level_base = options.target_file_size_base * 10;
        return options;
    }();
    return vertices_cfo;
}

inline static const rocksdb::ColumnFamilyOptions& edges_cfo() {
    static const rocksdb::ColumnFamilyOptions edges_cfo = []() {
        rocksdb::ColumnFamilyOptions options;
        // optimize vertex prefix enumeration to look for all edges of a particular
        // vertex
        options.prefix_extractor.reset(
            rocksdb::NewFixedPrefixTransform(1 + sizeof(vertex_t) + sizeof(vertex_id_t)));
        // Enable prefix bloom for SST files
        rocksdb::BlockBasedTableOptions table_options{};
        table_options.filter_policy.reset(rocksdb::NewBloomFilterPolicy(10, true));
        options.table_factory.reset(NewBlockBasedTableFactory(table_options));
        options.write_buffer_size = 128u << 20u;      // 128MB
        options.target_file_size_base = 128u << 20u;  // 128MB
        options.max_bytes_for_level_base = options.target_file_size_base * 10;
        return options;
    }();
    return edges_cfo;
}

inline static const std::string& vertices_cfn() {
    static const std::string vertices_cfn = rocksdb::kDefaultColumnFamilyName;
    return vertices_cfn;
}

inline static const std::string& edges_cfn() {
    static const std::string edges_cfn = "edges";
    return edges_cfn;
}

void GraphImpl::setup_db(const rocksdb::Options& options, const std::string& path) {
    std::unique_ptr<rocksdb::DB> db_ptr;
    {  // open db
        rocksdb::DB* db;
        rocksdb::Status status = rocksdb::DB::Open(options, path, &db);
        db_ptr.reset(db);
        if (status.code() == rocksdb::Status::kInvalidArgument) {
            return;
        }
        to_status(status).raise_on_error();
    }

    std::vector<std::string> column_family_names;
    rocksdb::DB::ListColumnFamilies(options, db_ptr->GetName(), &column_family_names);
    if (std::find(column_family_names.begin(), column_family_names.end(), vertices_cfn()) ==
        column_family_names.end()) {
        gsl::owner<rocksdb::ColumnFamilyHandle*> cf = nullptr;
        to_status(db_ptr->CreateColumnFamily(vertices_cfo(), vertices_cfn(), &cf)).raise_on_error();
        delete cf;
    }
    if (std::find(column_family_names.begin(), column_family_names.end(), edges_cfn()) ==
        column_family_names.end()) {
        gsl::owner<rocksdb::ColumnFamilyHandle*> cf = nullptr;
        to_status(db_ptr->CreateColumnFamily(edges_cfo(), edges_cfn(), &cf)).raise_on_error();
        delete cf;
    }
}

GraphImpl::GraphImpl(const std::string& path)
    : path_(path)
    , vertices_(*this)
    , edges_(*this)
    , statistics_(rocksdb::CreateDBStatistics())
    , options_(new rocksdb::Options(db_options(), {})) {
    rocksdb::DB* db;

    setup_db(*options_, path);

    std::vector<rocksdb::ColumnFamilyDescriptor> column_families;
    column_families.emplace_back(vertices_cfn(), vertices_cfo());
    column_families.emplace_back(edges_cfn(), edges_cfo());
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    handles.reserve(2);
    to_status(rocksdb::DB::Open(*options_, path, column_families, &handles, &db)).raise_on_error();
    vertices_column_.reset(handles[0]);
    edges_column_.reset(handles[1]);

    db_.reset(db);
    mkdir((path + "/logs").c_str(), 0777);
    const std::string logger_name = "basalt[" + path + "]";
    logger_ = spdlog::get(logger_name);
    if (!logger_) {
        logger_ = spdlog::rotating_logger_mt(logger_name, path + "/logs/graph.log", 1048576 * 5, 3);
        logger_->info("creating or loading database at location: {}", path);
        logger_->set_level(spdlog::level::level_enum::trace);
    }
}

Status GraphImpl::to_status(const rocksdb::Status& status) {
    return {static_cast<Status::Code>(status.code()), status.ToString()};
}

///// vertices methods

Status GraphImpl::vertices_insert(const vertex_uid_t& vertex, bool commit) {
    logger_get()->debug("vertices_insert(vertex={}, commit={})", vertex, commit);
    GraphKV::vertex_key_t key;
    GraphKV::encode(vertex, key);
    return to_status(db_get()->Put(write_options(commit), vertices_column_.get(),
                                   rocksdb::Slice(key.data(), key.size()), rocksdb::Slice()));
}

Status GraphImpl::vertices_insert(const vertex_uid_t& vertex,
                                  const gsl::span<const char>& payload,
                                  bool commit) {
    logger_get()->debug("vertices_insert(vertex={}, data_size={}, commit={})", vertex,
                        payload.size(), commit);
    GraphKV::vertex_key_t key;
    GraphKV::encode(vertex, key);
    return to_status(db_get()->Put(write_options(commit), vertices_column_.get(),
                                   rocksdb::Slice(key.data(), key.size()),
                                   rocksdb::Slice(payload.data(), payload.size())));
}

Status GraphImpl::vertices_insert(const gsl::span<const vertex_t> types,
                                  const gsl::span<const vertex_id_t> ids,
                                  const gsl::span<const char* const> payloads,
                                  const gsl::span<const std::size_t> payloads_sizes,
                                  bool commit) {
    logger_get()->debug("vertices_insert(vertices={}, payloads={}, commit={}", types.length(),
                        payloads.length() != 0, commit);
    GraphKV::vertex_key_t key;
    rocksdb::WriteBatch batch;

    if (payloads.empty()) {
        const rocksdb::Slice empty_payload;
        for (auto i = 0u; i < types.length(); ++i) {
            GraphKV::encode(types[i], ids[i], key);
            batch.Put(vertices_column_.get(), rocksdb::Slice(key.data(), key.size()),
                      empty_payload);
        }
    } else {
        for (auto i = 0u; i < types.length(); ++i) {
            GraphKV::encode(types[i], ids[i], key);
            batch.Put(vertices_column_.get(), rocksdb::Slice(key.data(), key.size()),
                      rocksdb::Slice(payloads[i], payloads_sizes[i]));
        }
    }
    return to_status(db_get()->Write(write_options(commit), &batch));
}


Status GraphImpl::vertices_has(const vertex_uid_t& vertex, bool& result) const {
    logger_get()->debug("vertices_has(vertex={})", vertex);
    GraphKV::vertex_key_t key;
    GraphKV::encode(vertex, key);
    const rocksdb::Slice slice(key.data(), key.size());

    std::string value;
    const auto& status = db_get()->Get(default_read_options(), vertices_column_.get(), slice,
                                       &value);
    if (status.IsNotFound()) {
        result = false;
        return Status::ok();
    }
    result = status.ok();
    return to_status(status);
}

Status GraphImpl::vertices_get(const vertex_uid_t& vertex, std::string* value) {
    logger_get()->debug("vertices_get(vertex={})", vertex);
    GraphKV::vertex_key_t key;
    GraphKV::encode(vertex, key);
    const auto& status = db_get()->Get(default_read_options(), vertices_column_.get(),
                                       rocksdb::Slice(key.data(), key.size()), value);
    if (status.IsNotFound()) {
        return Status::error_missing_vertex(vertex);
    }
    return to_status(status);
}

Status GraphImpl::vertices_erase(const vertex_uid_t& vertex, bool commit) {
    logger_get()->debug("vertices_erase(vertex={}, commit={})", vertex, commit);
    GraphKV::vertex_key_t key;
    GraphKV::encode(vertex, key);
    rocksdb::WriteBatch batch;
    const rocksdb::Slice slice(key.data(), key.size());
    batch.Delete(vertices_column_.get(), slice);
    auto removed = 0ul;
    const auto status = edges_erase(batch, vertex, removed);
    if (!status) {
        return status;
    }
    return to_status(db_get()->Write(write_options(commit), &batch));
}

Status GraphImpl::vertices_count(std::size_t& count) const {
    std::size_t num_vertices{};

    auto iter = db_get()->NewIterator(default_read_options(), this->vertices_column_.get());
    iter->SeekToFirst();
    while (iter->Valid()) {
        ++num_vertices;
        iter->Next();
    }
    count = num_vertices;
    return to_status(iter->status());
}

Status GraphImpl::vertices_count(vertex_t type, std::size_t& count) const {
    std::size_t num_vertices{};

    auto iter = db_get()->NewIterator(default_read_options(), this->vertices_column_.get());
    iter->SeekToFirst();
    vertex_uid_t vertex;
    while (iter->Valid()) {
        const auto& key = iter->key();
        GraphKV::decode_vertex(key.data(), key.size(), vertex);
        if (vertex.first == type) {
            ++num_vertices;
        }
        iter->Next();
    }
    count = num_vertices;
    return to_status(iter->status());
}

std::shared_ptr<VertexIteratorImpl> GraphImpl::VertexIterator(std::size_t from) const {
    logger_get()->debug("VertexIterator(from={})", from);
    return std::make_shared<VertexIteratorImpl>(db_get(), vertices_column_.get(), "N", from);
}

std::shared_ptr<EdgeIteratorImpl> GraphImpl::edge_iterator(std::size_t from) const {
    logger_get()->debug("edge_iterator(from={})", from);
    return std::make_shared<EdgeIteratorImpl>(db_get(), edges_column_.get(), "E", from);
}

Status GraphImpl::vertices_clear(bool commit) {
    rocksdb::WriteBatch batch;
    clear(batch, vertices_column_);
    clear(batch, edges_column_);
    return to_status(db_get()->Write(write_options(commit), &batch));
}


///// edges methods

Status GraphImpl::edges_count(std::size_t& count) const {
    std::size_t num_vertices{};

    auto iter = db_get()->NewIterator(default_read_options(), this->edges_column_.get());
    iter->SeekToFirst();
    while (iter->Valid()) {
        ++num_vertices;
        iter->Next();
    }
    count = num_vertices / 2;
    return to_status(iter->status());
}

Status GraphImpl::edges_clear(bool commit) {
    rocksdb::WriteBatch batch;
    clear(batch, edges_column_);
    return to_status(db_get()->Write(write_options(commit), &batch));
}

Status GraphImpl::edges_insert(const vertex_uid_t& vertex1,
                               const vertex_uid_t& vertex2,
                               const gsl::span<const char>& payload,
                               bool commit) {
    logger_get()->debug("edges_insert(vertex1={}, vertex2={}, payload={}, commit={})", vertex1,
                        vertex2, !payload.empty(), commit);
    if (vertex1 == vertex2) {
        return Status::error_invalid_edge(vertex1, vertex2);
    }
    {  // check presence of both vertices
        bool vertex_present = false;
        vertices_has(vertex1, vertex_present).raise_on_error();
        if (!vertex_present) {
            return Status::error_missing_vertex(vertex1);
        }
        vertices_has(vertex2, vertex_present).raise_on_error();
        if (!vertex_present) {
            return Status::error_missing_vertex(vertex2);
        }
    }

    GraphKV::edge_keys_t keys;
    GraphKV::encode(vertex1, vertex2, keys);
    const rocksdb::Slice data_slice(payload.data(), payload.size());
    rocksdb::WriteBatch batch;

    for (const auto& key: keys) {
        batch.Put(edges_column_.get(), rocksdb::Slice(key.data(), key.size()), data_slice);
    }
    return to_status(db_get()->Write(write_options(commit), &batch));
}

Status GraphImpl::edges_insert(const vertex_uid_t& vertex,
                               const vertex_t type,
                               const gsl::span<const vertex_id_t>& vertices,
                               const gsl::span<const char* const> vertex_payloads,
                               const gsl::span<const std::size_t>& vertex_payloads_sizes,
                               bool create_vertices,
                               bool commit) {
    logger_get()->debug("edges_insert(vertex={}, type={}, count={}, create_vertices={}, commit={})",
                        vertex, type, vertices.size(), create_vertices, commit);
    if (vertices.empty()) {
        return Status::ok();
    }
    rocksdb::WriteBatch batch;
    if (!create_vertices) {
        bool vertex_present;
        vertices_has(vertex, vertex_present).raise_on_error();
        if (!vertex_present) {
            return Status::error_missing_vertex(vertex);
        }
        for (auto to_vertex_id: vertices) {
            const auto to_vertex = make_id(type, to_vertex_id);
            vertices_has(to_vertex, vertex_present).raise_on_error();
            if (!vertex_present) {
                return Status::error_missing_vertex(to_vertex);
            }
        }
    } else {
        GraphKV::vertex_key_t key;
        GraphKV::encode(vertex, key);
        batch.Put(this->vertices_column_.get(), rocksdb::Slice(key.data(), key.size()),
                  rocksdb::Slice());
    }
    std::vector<GraphKV::edge_keys_t> keys(vertices.size());
    GraphKV::vertex_key_t vertex_key;
    for (auto i = 0u; i < keys.size(); ++i) {
        const auto target = make_id(type, vertices[i]);
        if (create_vertices) {
            GraphKV::encode(target, vertex_key);
            const rocksdb::Slice payload{vertex_payloads[i], vertex_payloads_sizes[i]};
            batch.Put(this->vertices_column_.get(),
                      rocksdb::Slice(vertex_key.data(), vertex_key.size()), payload);
        }
        GraphKV::encode(vertex, target, keys[i]);
        for (const auto& key: keys[i]) {
            batch.Put(edges_column_.get(), rocksdb::Slice(key.data(), key.size()),
                      rocksdb::Slice());
        }
    }
    return to_status(db_get()->Write(write_options(commit), &batch));
}

Status GraphImpl::edges_insert(const vertex_uid_t& vertex,
                               const vertex_t type,
                               const gsl::span<const vertex_id_t>& vertices,
                               bool create_vertices,
                               bool commit) {
    logger_get()->debug("edges_insert(vertex={}, type={}, count={}, create_vertices={}, commit={})",
                        vertex, type, vertices.size(), create_vertices, commit);
    if (vertices.empty()) {
        return Status::ok();
    }
    rocksdb::WriteBatch batch;
    if (!create_vertices) {
        bool vertex_present;
        vertices_has(vertex, vertex_present).raise_on_error();
        if (!vertex_present) {
            return Status::error_missing_vertex(vertex);
        }
        for (auto to_vertex_id: vertices) {
            const auto to_vertex = make_id(type, to_vertex_id);
            vertices_has(to_vertex, vertex_present).raise_on_error();
            if (!vertex_present) {
                return Status::error_missing_vertex(to_vertex);
            }
        }
    } else {
        GraphKV::vertex_key_t key;
        GraphKV::encode(vertex, key);
        batch.Put(this->vertices_column_.get(), rocksdb::Slice(key.data(), key.size()),
                  rocksdb::Slice());
    }
    std::vector<GraphKV::edge_keys_t> keys(vertices.size());
    GraphKV::vertex_key_t vertex_key;
    for (auto i = 0u; i < keys.size(); ++i) {
        const auto target = make_id(type, vertices[i]);
        if (create_vertices) {
            GraphKV::encode(target, vertex_key);
            batch.Put(this->vertices_column_.get(),
                      rocksdb::Slice(vertex_key.data(), vertex_key.size()), rocksdb::Slice());
        }
        GraphKV::encode(vertex, target, keys[i]);
        for (const auto& key: keys[i]) {
            batch.Put(edges_column_.get(), rocksdb::Slice(key.data(), key.size()),
                      rocksdb::Slice());
        }
    }
    return to_status(db_get()->Write(write_options(commit), &batch));
}

Status GraphImpl::edges_insert(const vertex_uid_t& vertex,
                               const vertex_uids_t& vertices,
                               const std::vector<const char*>& data,
                               const std::vector<std::size_t>& sizes,
                               bool commit) {
    logger_get()->debug("edges_insert(vertex={}, vertices={}, commit={})", vertex, vertices,
                        commit);
    {  // check presence of both vertices
        bool vertex_present = false;
        vertices_has(vertex, vertex_present).raise_on_error();
        if (!vertex_present) {
            return Status::error_missing_vertex(vertex);
        }
        for (auto const& dest_vertex: vertices) {
            if (vertex == dest_vertex) {
                return Status::error_invalid_edge(vertex, dest_vertex);
            }
            vertices_has(dest_vertex, vertex_present).raise_on_error();
            if (!vertex_present) {
                return Status::error_missing_vertex(dest_vertex);
            }
        }
    }

    std::vector<GraphKV::edge_keys_t> keys(vertices.size());
    rocksdb::WriteBatch batch;
    if (data.empty()) {
        for (auto i = 0u; i < vertices.size(); ++i) {
            GraphKV::encode(vertex, vertices[i], keys[i]);
            for (const auto& key: keys[i]) {
                batch.Put(edges_column_.get(), rocksdb::Slice(key.data(), key.size()),
                          rocksdb::Slice());
            }
        }
    } else {
        for (auto i = 0u; i < vertices.size(); ++i) {
            GraphKV::encode(vertex, vertices[i], keys[i]);
            for (const auto& key: keys[i]) {
                batch.Put(edges_column_.get(), rocksdb::Slice(key.data(), key.size()),
                          rocksdb::Slice(data[i], sizes[i]));
            }
        }
    }
    return to_status(db_get()->Write(write_options(commit), &batch));
}

Status GraphImpl::edges_has(const vertex_uid_t& vertex1,
                            const vertex_uid_t& vertex2,
                            bool& result) const {
    logger_get()->debug("edges_has(vertex1={}, vertex2={})", vertex1, vertex2);
    GraphKV::edge_key_t key;
    GraphKV::encode(vertex1, vertex2, key);
    std::string value;
    const auto& status = db_get()->Get(default_read_options(), edges_column_.get(),
                                       rocksdb::Slice(key.data(), key.size()), &value);
    if (status.IsNotFound()) {
        result = false;
        return Status::ok();
    }
    result = status.ok();
    return to_status(status);
}

Status GraphImpl::edges_get(const edge_uid_t& edge, std::string* value) const {
    logger_get()->debug("edges_get(edge={})", edge);
    GraphKV::edge_key_t key;
    GraphKV::encode(edge.first, edge.second, key);
    const auto& status = db_get()->Get(default_read_options(), edges_column_.get(),
                                       rocksdb::Slice(key.data(), key.size()), value);
    if (status.IsNotFound()) {
        return Status::error_missing_edge(edge);
    }
    return to_status(status);
}

Status GraphImpl::edges_get(const vertex_uid_t& vertex, vertex_uids_t& edges) const {
    logger_get()->debug("edges_get(vertex={})", vertex);
    GraphKV::edge_key_prefix_t key;
    GraphKV::encode_edge_prefix(vertex, key);
    const rocksdb::Slice slice(key.data(), key.size());
    auto iter = db_get()->NewIterator(default_read_options(), this->edges_column_.get());
    iter->Seek(slice);
    while (iter->Valid()) {
        auto const& conn_key = iter->key();
        if (std::memcmp(key.data(), conn_key.data(), key.size()) != 0) {
            break;
        }
        vertex_uid_t dest;
        GraphKV::decode_edge_dest(conn_key.data(), conn_key.size(), dest);
        edges.push_back(dest);
        iter->Next();
    }
    return to_status(iter->status());
}

Status GraphImpl::edges_get(const vertex_uid_t& vertex,
                            vertex_t filter,
                            vertex_uids_t& edges) const {
    logger_get()->debug("edges_get(vertex={}, filter={}, edges_column_={})", vertex, filter, edges);
    GraphKV::edge_key_type_prefix_t key;
    GraphKV::encode_edge_prefix(vertex, filter, key);
    const rocksdb::Slice slice(key.data(), key.size());
    auto iter = db_get()->NewIterator(default_read_options(), this->edges_column_.get());
    iter->Seek(slice);
    if (!iter->status().ok()) {
        return to_status(iter->status());
    }
    while (iter->Valid()) {
        auto const& conn_key = iter->key();
        if (std::memcmp(key.data(), conn_key.data(), key.size()) != 0) {
            break;
        }
        vertex_uid_t dest;
        GraphKV::decode_edge_dest(conn_key.data(), conn_key.size(), dest);
        edges.push_back(dest);
        iter->Next();
    }
    return Status::ok();
}

Status GraphImpl::edges_erase(const vertex_uid_t& vertex1,
                              const vertex_uid_t& vertex2,
                              bool commit) {
    logger_get()->debug("edges_erase(vertex1={}, vertex2={}, commit={})", vertex1, vertex2, commit);

    GraphKV::edge_keys_t keys;
    GraphKV::encode(vertex1, vertex2, keys);
    rocksdb::WriteBatch batch;

    for (const auto& key: keys) {
        batch.Delete(edges_column_.get(), rocksdb::Slice(key.data(), key.size()));
    }
    return to_status(db_get()->Write(write_options(commit), &batch));
}

Status GraphImpl::edges_erase(rocksdb::WriteBatch& batch,
                              const vertex_uid_t& vertex,
                              size_t& removed) {
    GraphKV::edge_key_prefix_t key;
    GraphKV::encode_edge_prefix(vertex, key);
    const rocksdb::Slice slice(key.data(), key.size());

    auto iter = db_get()->NewIterator(default_read_options(), edges_column_.get());
    iter->Seek(slice);

    // iterate over all edges
    auto edges = 0ul;
    while (iter->Valid()) {
        // remove the key
        auto const& conn_slice = iter->key();

        if (std::memcmp(key.data(), conn_slice.data(), key.size()) != 0) {
            break;
        }
        batch.Delete(this->edges_column_.get(), conn_slice);

        // remove the reverse edge
        GraphKV::edge_key_t reversed_key;
        GraphKV::encode_reversed_edge(conn_slice.data(), conn_slice.size(), reversed_key);
        const rocksdb::Slice reversed_slice(reversed_key.data(), reversed_key.size());
        batch.Delete(this->edges_column_.get(), reversed_slice);
        ++edges;
        iter->Next();
    }
    const auto status = iter->status();
    if (status.ok()) {
        removed = edges;
    }
    return to_status(status);
}

Status GraphImpl::edges_erase(const vertex_uid_t& vertex, size_t& removed, bool commit) {
    logger_get()->debug("edges_erase(vertex={}, commit={})", vertex, commit);
    rocksdb::WriteBatch batch;
    auto edges = 0ul;
    edges_erase(batch, vertex, edges).raise_on_error();
    auto const& status = to_status(db_get()->Write(write_options(commit), &batch));
    if (status) {
        removed = edges;
    } else {
        removed = 0;
    }
    return status;
}

Status GraphImpl::edges_erase(const vertex_uid_t& vertex,
                              vertex_t filter,
                              size_t& removed,
                              bool commit) {
    logger_get()->debug("edges_erase(vertex={}, filter={}, commit={})", vertex, filter, commit);
    GraphKV::edge_key_type_prefix_t key;
    GraphKV::encode_edge_prefix(vertex, filter, key);
    auto iter = db_get()->NewIterator(default_read_options());
    iter->Seek(rocksdb::Slice(key.data(), key.size()));

    // iterate over all edges
    rocksdb::WriteBatch batch;
    auto edges = 0ul;
    while (iter->Valid()) {
        auto const& conn_key = iter->key();
        if (std::memcmp(key.data(), conn_key.data(), key.size()) != 0) {
            break;
        }
        batch.Delete(conn_key);

        // remove the reverse edge
        GraphKV::edge_key_t reversed_key;
        GraphKV::encode_reversed_edge(conn_key.data(), conn_key.size(), reversed_key);
        batch.Delete(rocksdb::Slice(reversed_key.data(), reversed_key.size()));
        ++edges;
        iter->Next();
    }
    auto status = iter->status();
    if (status.ok()) {
        status = db_get()->Write(write_options(commit), &batch);
    }
    if (status.ok()) {
        removed = edges;
    } else {
        removed = 0;
    }
    return to_status(status);
}

Status GraphImpl::commit() {
    logger_get()->debug("commit()");
    to_status(db_get()->Flush(rocksdb::FlushOptions(), vertices_column_.get())).raise_on_error();
    return to_status(db_get()->Flush(rocksdb::FlushOptions(), edges_column_.get()))
        .raise_on_error();
}

std::string GraphImpl::statistics() const {
    return statistics_->ToString();
}

void GraphImpl::clear(rocksdb::WriteBatch& batch,
                      const std::unique_ptr<rocksdb::ColumnFamilyHandle>& handle) {
    auto iter = db_get()->NewIterator(default_read_options(), handle.get());
    iter->SeekToFirst();
    if (iter->Valid()) {
        while (iter->Valid()) {
            batch.Delete(handle.get(), iter->key());
            iter->Next();
        }
    }
}

}  // namespace basalt
