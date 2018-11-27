#include "network_impl.hpp"

#include <rocksdb/db.h>
#include <rocksdb/slice_transform.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace basalt {

static const rocksdb::ReadOptions default_read_options;
static const rocksdb::WriteOptions async_write;

network_impl_t::network_impl_t(const std::string& path)
    : path_(path), nodes_(*this), connections_(*this) {
    if (path.empty()) {
        logger_ = spdlog::stdout_color_mt("basalt");
    } else {
        logger_ = spdlog::rotating_logger_mt(
            "basalt", path + "/logs/network.log", 1048576 * 5, 3);
    }
    logger_->info("creating or loading database at location: {}", path);
    rocksdb::Options options;
    options.prefix_extractor.reset(rocksdb::NewFixedPrefixTransform(1));
    options.create_if_missing = true;
    rocksdb::DB* db;
    rocksdb::Status status = rocksdb::DB::Open(options, path, &db);
    db_.reset(db);
    if (!status.ok()) {
        const auto message = status.ToString();
        logger_->error("Could not open database: {}", message);
        throw std::runtime_error(message);
    }
}

status_t network_impl_t::to_status(const rocksdb::Status& status) {
    return {status.code(), status.ToString()};
}

///// nodes methods

status_t network_impl_t::nodes_has(const basalt::node_uid_t& node,
                                   bool& result) const {
    logger_get()->debug("nodes_has(node={})", node);
    graph::node_key_t key;
    graph::encode(node, key);
    const rocksdb::Slice slice(key.data(), key.size());
    /// \todo TCL we may need to use Get() method
    result = db_get()->KeyMayExist(default_read_options, slice, nullptr);
    return status_t::ok();
}

/// \todo TCL use a single batch
status_t network_impl_t::nodes_erase(const node_uid_t& node, bool commit) {
    logger_get()->debug("nodes_erase(node={}, commit={})", node, commit);
    graph::node_key_t key;
    graph::encode(node, key);
    const rocksdb::Slice slice(key.data(), key.size());
    const auto result = db_get()->SingleDelete(async_write, slice);
    auto removed = 0ul;
    connections_erase(node, removed, commit);
    return to_status(result);
}

std::shared_ptr<node_iterator_impl> network_impl_t::node_iterator(std::size_t from) const {
    return std::shared_ptr<node_iterator_impl>(new node_iterator_impl(db_get(), "N", from));
}

///// connections methods

status_t network_impl_t::connections_connect(const node_uid_t& node1,
                                             const node_uid_t& node2,
                                             const payload_t& data,
                                             bool commit) {
    logger_get()->debug("connections_connect(node1={}, node2={}, commit={})",
                        node1, node2, commit);
    { // check presence of both nodes
        bool node_present;
        nodes_has(node1, node_present).raise_on_error();
        if (!node_present) {
            return status_t::error_missing_node(node1);
        }
        nodes_has(node2, node_present).raise_on_error();
        if (!node_present) {
            return status_t::error_missing_node(node2);
        }
    }
    graph::connection_keys_t keys;
    graph::encode(node1, node2, keys);
    const rocksdb::Slice data_slice(data.data(), data.size());
    rocksdb::WriteBatch batch;

    for (const auto& key : keys) {
        const rocksdb::Slice key_slice(key.data(), key.size());
        batch.Put(key_slice, data_slice);
    }
    return to_status(db_get()->Write(async_write, &batch));
}

status_t network_impl_t::connections_connect(const node_uid_t& node,
                                             const node_uids_t& nodes,
                                             const payload_t& payload,
                                             bool commit) {
    logger_get()->debug("connections_connect(node={}, nodes={}, commit={})",
                        node, nodes, commit);
    { // check presence of both nodes
        bool node_present;
        nodes_has(node, node_present).raise_on_error();
        if (!node_present) {
            return status_t::error_missing_node(node);
        }
        for (auto const& dest_node : nodes) {
            nodes_has(dest_node, node_present).raise_on_error();
            if (!node_present) {
                return status_t::error_missing_node(dest_node);
            }
        }
    }

    std::vector<graph::connection_keys_t> keys(nodes.size());
    rocksdb::WriteBatch batch;
    const rocksdb::Slice data_slice(payload.data(), payload.size());
    for (auto i = 0u; i < nodes.size(); ++i) {
        graph::encode(node, nodes[i], keys[i]);
        for (const auto& key : keys[i]) {
            const rocksdb::Slice key_slice(key.data(), key.size());
            batch.Put(key_slice, data_slice);
        }
    }
    return to_status(db_get()->Write(async_write, &batch));
}

std::pair<bool, status_t>
network_impl_t::connections_connected(const basalt::node_uid_t& node1,
                                      const basalt::node_uid_t& node2) const {
    logger_get()->debug("connections_connected(node1={}, node2={})", node1,
                        node2);
    graph::connection_key_t key;
    graph::encode(node1, node2, key);
    const rocksdb::Slice slice(key.data(), key.size());
    return {db_get()->KeyMayExist(default_read_options, slice, nullptr),
            status_t::ok()};
}

status_t network_impl_t::connections_get(const node_uid_t& node,
                                         node_uids_t& connections) const {
    logger_get()->debug("connections_get(node={}, connections={})", node,
                        connections);
    graph::connection_key_prefix_t key;
    graph::encode_connection_prefix(node, key);
    const rocksdb::Slice slice(key.data(), key.size());
    auto iter = db_get()->NewIterator(default_read_options);
    iter->Seek(slice);
    if (!iter->status().ok()) {
        return to_status(iter->status());
    }
    while (iter->Valid()) {
        node_uid_t dest;
        auto const& conn_key = iter->key();
        graph::decode_connection_dest(conn_key.data(), conn_key.size(), dest);
        connections.push_back(dest);
        iter->Next();
        if (!iter->status().ok()) {
            return to_status(iter->status());
        }
    }
    return status_t::ok();
}

status_t network_impl_t::connections_get(const node_uid_t& node, node_t filter,
                                         node_uids_t& connections) const {
    logger_get()->debug("connections_get(node={}, filter={}, connections={})",
                        node, filter, connections);
    graph::connection_key_type_prefix_t key;
    graph::encode_connection_prefix(node, filter, key);
    const rocksdb::Slice slice(key.data(), key.size());
    auto iter = db_get()->NewIterator(default_read_options);
    iter->Seek(slice);
    if (!iter->status().ok()) {
        return to_status(iter->status());
    }
    while (iter->Valid()) {
        node_uid_t dest;
        auto const& conn_key = iter->key();
        graph::decode_connection_dest(conn_key.data(), conn_key.size(), dest);
        connections.push_back(dest);
        iter->Next();
        if (!iter->status().ok()) {
            return to_status(iter->status());
        }
    }
    return status_t::ok();
}

status_t network_impl_t::connections_erase(const node_uid_t& node1,
                                           const node_uid_t& node2,
                                           bool commit) {
    logger_get()->debug("connections_erase(node1={}, node2={}, commit={})",
                        node1, node2, commit);

    graph::connection_keys_t keys;
    graph::encode(node1, node2, keys);
    rocksdb::WriteBatch batch;

    for (const auto& key : keys) {
        const rocksdb::Slice key_slice(key.data(), key.size());
        batch.Delete(key_slice);
    }
    return to_status(db_get()->Write(async_write, &batch));
}

status_t network_impl_t::connections_erase(const node_uid_t& node,
                                           size_t& removed, bool commit) {
    logger_get()->debug("connections_erase(node={}, commit={})", node, commit);
    graph::connection_key_prefix_t key;
    graph::encode_connection_prefix(node, key);
    const rocksdb::Slice slice(key.data(), key.size());
    auto iter = db_get()->NewIterator(default_read_options);
    iter->Seek(slice);
    if (!iter->status().ok()) {
        removed = 0;
        return to_status(iter->status());
    }
    rocksdb::WriteBatch batch;
    // iterate over all connections
    auto connections = 0ul;
    while (iter->Valid()) {
        // remove the key
        auto const& conn_slice = iter->key();
        batch.Delete(conn_slice);

        // remove the reverse connection
        graph::connection_key_t reversed_key;
        graph::encode_reversed_connection(conn_slice.data(), conn_slice.size(),
                                          reversed_key);
        ;
        batch.Delete(rocksdb::Slice(reversed_key.data(), reversed_key.size()));
        ++connections;
        iter->Next();
        {
            const auto& next_status = iter->status();
            if (!next_status.ok()) {
                removed = 0;
                return to_status(next_status);
            }
        }
    }
    auto const& status = to_status(db_get()->Write(async_write, &batch));
    if (status) {
        removed = connections;
    } else {
        removed = 0;
    }
    return status;
}

status_t network_impl_t::connections_erase(const node_uid_t& node,
                                           node_t filter, size_t& removed,
                                           bool commit) {
    logger_get()->debug("connections_erase(node={}, filter={}, commit={})",
                        node, filter, commit);
    graph::connection_key_type_prefix_t key;
    graph::encode_connection_prefix(node, filter, key);
    auto iter = db_get()->NewIterator(default_read_options);
    iter->Seek(rocksdb::Slice(key.data(), key.size()));
    if (!iter->status().ok()) {
        removed = 0;
        return to_status(iter->status());
    }

    // iterate over all connections
    rocksdb::WriteBatch batch;
    auto connections = 0ul;
    while (iter->Valid()) {
        auto const& conn_slice = iter->key();
        batch.Delete(conn_slice);

        // remove the reverse connection
        graph::connection_key_t reversed_key;
        graph::encode_reversed_connection(conn_slice.data(), conn_slice.size(),
                                          reversed_key);
        batch.Delete(rocksdb::Slice(reversed_key.data(), reversed_key.size()));
        ++connections;
        iter->Next();
        {
            const auto& next_status = iter->status();
            if (!next_status.ok()) {
                removed = 0;
                return to_status(next_status);
            }
        }
    }
    { // commit changes
        const auto& status = to_status(db_get()->Write(async_write, &batch));
        if (!status) {
            removed = 0;
        } else {
            removed = connections;
        }
        return status;
    }
}

status_t network_impl_t::commit() {
    logger_get()->debug("commit()");
    return to_status(db_get()->Flush(rocksdb::FlushOptions()));
}

} // namespace basalt