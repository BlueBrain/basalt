#ifndef BASALT_TOPOLOGY_HPP
#define BASALT_TOPOLOGY_HPP

#include <array>
#include <cassert>
#include <cstring>

#include <basalt/fwd.hpp>

namespace basalt {

class graph {
  public:
    using node_key_t = std::array<char, 1 + sizeof(node_id_t) + sizeof(node_t)>;
    using connection_key_prefix_t = std::array<char, 1 + sizeof(node_id_t) + sizeof(node_t)>;
    using connection_key_type_prefix_t =
        std::array<char, 1 + sizeof(node_id_t) + 2 * sizeof(node_t)>;
    using connection_key_t = std::array<char, 1 + 2 * (sizeof(node_id_t) + sizeof(node_t))>;
    using connection_keys_t = std::array<connection_key_t, 2>;

    constexpr static auto connection_key_size = std::tuple_size<connection_key_t>::value;

    /** \name Key encoding functions
     *  \{
     */
    static inline void encode(const node_uid_t& node, node_key_t& key) {
        key[0] = 'N';
        std::memcpy(key.data() + 1, reinterpret_cast<const char*>(&node.first),
                    sizeof(node_uid_t::first_type));
        std::memcpy(key.data() + 1 + sizeof(node_uid_t::first_type),
                    reinterpret_cast<const char*>(&node.second), sizeof(node_uid_t::second_type));
    }

    static inline void encode_connection_prefix(const node_uid_t& node,
                                                connection_key_prefix_t& key) {
        key[0] = 'E';
        std::memcpy(key.data() + 1, reinterpret_cast<const char*>(&node.first),
                    sizeof(node_uid_t::first_type));
        std::memcpy(key.data() + 1 + sizeof(node_uid_t::first_type),
                    reinterpret_cast<const char*>(&node.second), sizeof(node_uid_t::second_type));
    }

    static inline void encode_connection_prefix(const node_uid_t& node,
                                                node_t type,
                                                connection_key_type_prefix_t& key) {
        key[0] = 'E';
        std::memcpy(key.data() + 1, reinterpret_cast<const char*>(&node.first),
                    sizeof(node_uid_t::first_type));
        std::memcpy(key.data() + 1 + sizeof(node_uid_t::first_type),
                    reinterpret_cast<const char*>(&node.second), sizeof(node_uid_t::second_type));
        std::memcpy(key.data() + 1 + sizeof(node_uid_t::first_type) +
                        sizeof(node_uid_t::second_type),
                    reinterpret_cast<const char*>(&type), sizeof(node_uid_t::first_type));
    }

    static inline void encode(const node_uid_t& node1,
                              const node_uid_t& node2,
                              connection_key_t& key) {
        key[0] = 'E';
        std::memcpy(key.data() + 1, reinterpret_cast<const char*>(&node1.first),
                    sizeof(node_uid_t::first_type));
        std::memcpy(key.data() + 1 + sizeof(node_uid_t::first_type),
                    reinterpret_cast<const char*>(&node1.second), sizeof(node_uid_t::second_type));
        std::memcpy(key.data() + 1 + sizeof(node_uid_t::first_type) +
                        sizeof(node_uid_t::second_type),
                    reinterpret_cast<const char*>(&node2.first), sizeof(node_uid_t::first_type));
        std::memcpy(key.data() + 1 + 2 * sizeof(node_uid_t::first_type) +
                        sizeof(node_uid_t::second_type),
                    reinterpret_cast<const char*>(&node2.second), sizeof(node_uid_t::second_type));
    }
    static inline void encode(const node_uid_t& node1,
                              const node_uid_t& node2,
                              connection_keys_t& keys) {
        encode(node1, node2, keys[0]);
        encode(node2, node1, keys[1]);
    }
    static inline void encode_reversed_connection(const char* data,
                                                  size_t size,
                                                  connection_key_t& key) {
        assert(size == std::tuple_size<connection_key_t>::value);
        key[0] = 'E';
        std::memcpy(key.data() + 1,
                    data + 1 + sizeof(node_uid_t::first_type) + sizeof(node_uid_t::second_type),
                    sizeof(node_uid_t::first_type) + sizeof(node_uid_t::second_type));
        std::memcpy(key.data() + 1 + sizeof(node_uid_t::first_type) +
                        sizeof(node_uid_t::second_type),
                    data + 1, sizeof(node_uid_t::first_type) + sizeof(node_uid_t::second_type));
    }
    /**
     *  \}
     */

    /** \name Key decoding functions
     * \{
     */
    static inline void decode_connection_dest(const char* data, size_t size, node_uid_t& node) {
        assert(size == std::tuple_size<connection_key_t>::value);
        assert(data[0] == 'E');
        std::memcpy(reinterpret_cast<char*>(&node.first),
                    data + 1 + sizeof(node_uid_t::first_type) + sizeof(node_uid_t::second_type),
                    sizeof(node_uid_t::first_type));
        std::memcpy(reinterpret_cast<char*>(&node.second),
                    data + 1 + 2 * sizeof(node_uid_t::first_type) + sizeof(node_uid_t::second_type),
                    sizeof(node_uid_t::second_type));
    }
    static inline void decode_node(const char* data, size_t size, node_uid_t& node) {
        assert(size == 1 + sizeof(node_uid_t::first_type) + sizeof(node_uid_t::second_type));
        assert(data[0] == 'N');
        std::memcpy(reinterpret_cast<char*>(&node.first), data + 1, sizeof(node_uid_t::first_type));
        std::memcpy(reinterpret_cast<char*>(&node.second),
                    data + 1 + sizeof(node_uid_t::first_type), sizeof(node_uid_t::second_type));
    }
    /**
     * \}
     */
};

}  // namespace basalt

#endif  // BASALT_TOPOLOGY_HPP
