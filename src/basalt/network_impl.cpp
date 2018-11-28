#include <cstring>

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

//// Helper methods

void network_impl_t::encode(const node_uid_t& node, node_key_t& key) {

    key[0] = 'N';
    std::strncpy(key.data() + 1, reinterpret_cast<const char*>(&node.first),
                 sizeof(node_t));
    std::strncpy(key.data() + 1 + sizeof(node_t),
                 reinterpret_cast<const char*>(&node.second),
                 sizeof(node_id_t));
}

void network_impl_t::encode_connection_prefix(const node_uid_t& node,
                                              connection_key_prefix_t& key) {
    key[0] = 'E';
    std::strncpy(key.data() + 1, reinterpret_cast<const char*>(&node.first),
                 sizeof(node_t));
    std::strncpy(key.data() + 1 + sizeof(node_t),
                 reinterpret_cast<const char*>(&node.second),
                 sizeof(node_id_t));
}

void network_impl_t::encode_connection_prefix(
    const node_uid_t& node, node_t type, connection_key_type_prefix_t& key) {

    key[0] = 'E';
    std::strncpy(key.data() + 1, reinterpret_cast<const char*>(&node.first),
                 sizeof(node_t));
    std::strncpy(key.data() + 1 + sizeof(node_t),
                 reinterpret_cast<const char*>(&node.second),
                 sizeof(node_id_t));
    std::strncpy(key.data() + 1 + sizeof(node_t) + sizeof(node_uid_t),
                 reinterpret_cast<const char*>(&type), sizeof(node_t));
}

void network_impl_t::encode(const basalt::node_uid_t& node1,
                            const basalt::node_uid_t& node2,
                            basalt::network_impl_t::connection_key_t& key) {
    key[0] = 'E';
    std::strncpy(key.data() + 1, reinterpret_cast<const char*>(&node1.first),
                 sizeof(node_t));
    std::strncpy(key.data() + 1 + sizeof(node_t),
                 reinterpret_cast<const char*>(&node1.second),
                 sizeof(node_id_t));
    std::strncpy(key.data() + 1 + sizeof(node_t) + sizeof(node_id_t),
                 reinterpret_cast<const char*>(&node2.first), sizeof(node_t));
    std::strncpy(key.data() + 1 + 2 * sizeof(node_t) + sizeof(node_id_t),
                 reinterpret_cast<const char*>(&node2.second),
                 sizeof(node_id_t));
}

void network_impl_t::encode(const node_uid_t& node1, const node_uid_t& node2,
                            connection_keys_t& keys) {
    encode(node1, node2, keys[0]);
    encode(node2, node1, keys[1]);
}

void network_impl_t::decode_connection_dest(const char* data, size_t size,
                                            node_uid_t& node) {
    assert(size == std::tuple_size<connection_key_t>::value);
    assert(data[0] == 'E');
    std::strncpy(reinterpret_cast<char*>(&node.first),
                 data + 1 + sizeof(node_t) + sizeof(node_id_t), sizeof(node_t));
    std::strncpy(reinterpret_cast<char*>(&node.second),
                 data + 1 + 2 * sizeof(node_t) + sizeof(node_id_t),
                 sizeof(node_id_t));
}

void network_impl_t::encode_reversed_connection(const char* data, size_t size,
                                                connection_key_t& key) {
    assert(size == std::tuple_size<connection_key_t>::value);
    key[0] = 'E';
    std::strncpy(key.data() + 1, data + 1 + sizeof(node_t) + sizeof(node_id_t),
                 sizeof(node_t) + sizeof(node_id_t));
    std::strncpy(key.data() + 1 + sizeof(node_t) + sizeof(node_id_t), data + 1,
                 sizeof(node_t) + sizeof(node_id_t));
}

status_t network_impl_t::to_status(const rocksdb::Status& status) {
    return {status.code(), status.ToString()};
}

///// nodes methods

status_t network_impl_t::nodes_has(const basalt::node_uid_t& node,
                                   bool& result) const {
    logger_get()->debug("nodes_has(node={})", node);
    node_key_t key;
    encode(node, key);
    const rocksdb::Slice slice(key.data(), key.size());
    /// \todo TCL we may need to use Get() method
    result = db_get()->KeyMayExist(default_read_options, slice, nullptr);
    return status_t::ok();
}

/// \todo TCL use a single batch
status_t network_impl_t::nodes_erase(const node_uid_t& node, bool commit) {
    logger_get()->debug("nodes_erase(node={}, commit={})", node, commit);
    node_key_t key;
    encode(node, key);
    const rocksdb::Slice slice(key.data(), key.size());
    const auto result = db_get()->SingleDelete(async_write, slice);
    auto removed = 0ul;
    connections_erase(node, removed, commit);
    return to_status(result);
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
    connection_keys_t keys;
    encode(node1, node2, keys);
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

    std::vector<connection_keys_t> keys(nodes.size());
    rocksdb::WriteBatch batch;
    const rocksdb::Slice data_slice(payload.data(), payload.size());
    for (auto i = 0u; i < nodes.size(); ++i) {
        encode(node, nodes[i], keys[i]);
        for (const auto& key : keys[i]) {
            const rocksdb::Slice key_slice(key.data(), key.size());
            batch.Put(key_slice, data_slice);
        }
    }
    return to_status(db_get()->Write(async_write, &batch));
}

status_t network_impl_t::connections_connected(const basalt::node_uid_t& node1,
                                               const basalt::node_uid_t& node2,
                                               bool& res) const {
    logger_get()->debug("connections_connected(node1={}, node2={})", node1,
                        node2);
    connection_key_t key;
    encode(node1, node2, key);
    const rocksdb::Slice slice(key.data(), key.size());
    res = db_get()->KeyMayExist(default_read_options, slice, nullptr);
    return status_t::ok();
}

status_t network_impl_t::connections_get(const node_uid_t& node,
                                         node_uids_t& connections) const {
    logger_get()->debug("connections_get(node={}, connections={})", node,
                        connections);
    connection_key_prefix_t key;
    encode_connection_prefix(node, key);
    const rocksdb::Slice slice(key.data(), key.size());
    auto iter = db_get()->NewIterator(default_read_options);
    iter->Seek(slice);
    if (!iter->status().ok()) {
        return to_status(iter->status());
    }
    while (iter->Valid()) {
        node_uid_t dest;
        auto const& conn_key = iter->key();
        decode_connection_dest(conn_key.data(), conn_key.size(), dest);
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
    connection_key_type_prefix_t key;
    encode_connection_prefix(node, filter, key);
    const rocksdb::Slice slice(key.data(), key.size());
    auto iter = db_get()->NewIterator(default_read_options);
    iter->Seek(slice);
    if (!iter->status().ok()) {
        return to_status(iter->status());
    }
    while (iter->Valid()) {
        node_uid_t dest;
        auto const& conn_key = iter->key();
        decode_connection_dest(conn_key.data(), conn_key.size(), dest);
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

    connection_keys_t keys;
    encode(node1, node2, keys);
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
    connection_key_prefix_t key;
    encode_connection_prefix(node, key);
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
        connection_key_t reversed_key;
        encode_reversed_connection(conn_slice.data(), conn_slice.size(),
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
    connection_key_type_prefix_t key;
    encode_connection_prefix(node, filter, key);
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
        connection_key_t reversed_key;
        encode_reversed_connection(conn_slice.data(), conn_slice.size(),
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