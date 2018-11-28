#ifndef BASALT_TOPOLOGY_HPP
#define BASALT_TOPOLOGY_HPP

#include <array>

#include <basalt/fwd.hpp>

namespace basalt {

class graph {
  public:
    using node_key_t = std::array<char, 1 + sizeof(node_id_t) + sizeof(node_t)>;
    using connection_key_prefix_t =
        std::array<char, 1 + sizeof(node_id_t) + sizeof(node_t)>;
    using connection_key_type_prefix_t =
        std::array<char, 1 + sizeof(node_id_t) + 2 * sizeof(node_t)>;
    using connection_key_t =
        std::array<char, 1 + 2 * (sizeof(node_id_t) + sizeof(node_t))>;
    using connection_keys_t = std::array<connection_key_t, 2>;

    constexpr static auto connection_key_size =
        std::tuple_size<connection_key_t>::value;

    /** \name Key encoding functions
     *  \{
     */
    static void encode(const node_uid_t& node, node_key_t& key);
    static void encode_connection_prefix(const node_uid_t& node,
                                         connection_key_prefix_t& key);
    static void encode_connection_prefix(const node_uid_t& node, node_t type,
                                         connection_key_type_prefix_t& key);
    static void encode(const node_uid_t& node1, const node_uid_t& node2,
                       connection_key_t& key);
    static void encode(const node_uid_t& node1, const node_uid_t& node2,
                       connection_keys_t& key);
    static void encode_reversed_connection(const char* data, size_t size,
                                           connection_key_t& key);
    /**
     *  \}
     */

    /** \name Key decoding functions
     * \{
     */
    static void decode_connection_dest(const char* data, size_t size,
                                       node_uid_t& uid);
    static void decode_node(const char* data, size_t size, node_uid_t& node);
    /**
     * \}
     */
};

} // namespace basalt

#endif // BASALT_TOPOLOGY_HPP
