#include <cassert>
#include <cstring>

#include "graph.hpp"

namespace basalt {

void graph::encode(const node_uid_t& node, node_key_t& key) {

    key[0] = 'N';
    std::strncpy(key.data() + 1, reinterpret_cast<const char*>(&node.first),
                 sizeof(node_t));
    std::strncpy(key.data() + 1 + sizeof(node_t),
                 reinterpret_cast<const char*>(&node.second),
                 sizeof(node_id_t));
}

void graph::encode_connection_prefix(const node_uid_t& node,
                                     connection_key_prefix_t& key) {
    key[0] = 'E';
    std::strncpy(key.data() + 1, reinterpret_cast<const char*>(&node.first),
                 sizeof(node_t));
    std::strncpy(key.data() + 1 + sizeof(node_t),
                 reinterpret_cast<const char*>(&node.second),
                 sizeof(node_id_t));
}

void graph::encode_connection_prefix(const node_uid_t& node, node_t type,
                                     connection_key_type_prefix_t& key) {

    key[0] = 'E';
    std::strncpy(key.data() + 1, reinterpret_cast<const char*>(&node.first),
                 sizeof(node_t));
    std::strncpy(key.data() + 1 + sizeof(node_t),
                 reinterpret_cast<const char*>(&node.second),
                 sizeof(node_id_t));
    std::strncpy(key.data() + 1 + sizeof(node_t) + sizeof(node_uid_t),
                 reinterpret_cast<const char*>(&type), sizeof(node_t));
}

void graph::encode(const basalt::node_uid_t& node1,
                   const basalt::node_uid_t& node2,
                   basalt::graph::connection_key_t& key) {
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

void graph::encode(const node_uid_t& node1, const node_uid_t& node2,
                   connection_keys_t& keys) {
    encode(node1, node2, keys[0]);
    encode(node2, node1, keys[1]);
}

void graph::decode_connection_dest(const char* data, size_t size,
                                   node_uid_t& node) {
    assert(size == std::tuple_size<connection_key_t>::value);
    assert(data[0] == 'E');
    std::strncpy(reinterpret_cast<char*>(&node.first),
                 data + 1 + sizeof(node_t) + sizeof(node_id_t), sizeof(node_t));
    std::strncpy(reinterpret_cast<char*>(&node.second),
                 data + 1 + 2 * sizeof(node_t) + sizeof(node_id_t),
                 sizeof(node_id_t));
}

void graph::encode_reversed_connection(const char* data, size_t size,
                                       connection_key_t& key) {
    assert(size == std::tuple_size<connection_key_t>::value);
    key[0] = 'E';
    std::strncpy(key.data() + 1, data + 1 + sizeof(node_t) + sizeof(node_id_t),
                 sizeof(node_t) + sizeof(node_id_t));
    std::strncpy(key.data() + 1 + sizeof(node_t) + sizeof(node_id_t), data + 1,
                 sizeof(node_t) + sizeof(node_id_t));
}

} // namespace basalt