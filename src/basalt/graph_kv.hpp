/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

#include <array>
#include <cassert>
#include <cstring>

#include <basalt/fwd.hpp>

namespace basalt {

class GraphKV {
  public:
    using vertex_key_t = std::array<char, 1 + sizeof(vertex_id_t) + sizeof(vertex_t)>;
    using edge_key_prefix_t = std::array<char, 1 + sizeof(vertex_id_t) + sizeof(vertex_t)>;
    using edge_key_type_prefix_t = std::array<char, 1 + sizeof(vertex_id_t) + 2 * sizeof(vertex_t)>;
    using edge_key_t = std::array<char, 1 + 2 * (sizeof(vertex_id_t) + sizeof(vertex_t))>;

    constexpr static auto edge_key_size = std::tuple_size<edge_key_t>::value;

    /** \name Key encoding functions
     *  \{
     */
    static inline void encode(const vertex_t type, const vertex_id_t id, vertex_key_t& key) {
        key[0] = 'N';
        std::memcpy(key.data() + 1, reinterpret_cast<const char*>(&type), sizeof(decltype(type)));
        std::memcpy(key.data() + 1 + sizeof(decltype(type)),
                    reinterpret_cast<const char*>(&id),
                    sizeof(decltype(id)));
    }

    static inline void encode(const vertex_uid_t& vertex, vertex_key_t& key) {
        encode(vertex.first, vertex.second, key);
    }

    static inline void encode_edge_prefix(const vertex_uid_t& vertex, edge_key_prefix_t& key) {
        key[0] = 'E';
        std::memcpy(key.data() + 1,
                    reinterpret_cast<const char*>(&vertex.first),
                    sizeof(vertex_uid_t::first_type));
        std::memcpy(key.data() + 1 + sizeof(vertex_uid_t::first_type),
                    reinterpret_cast<const char*>(&vertex.second),
                    sizeof(vertex_uid_t::second_type));
    }

    static inline void encode_edge_prefix(const vertex_uid_t& vertex,
                                          vertex_t type,
                                          edge_key_type_prefix_t& key) {
        key[0] = 'E';
        std::memcpy(key.data() + 1,
                    reinterpret_cast<const char*>(&vertex.first),
                    sizeof(vertex_uid_t::first_type));
        std::memcpy(key.data() + 1 + sizeof(vertex_uid_t::first_type),
                    reinterpret_cast<const char*>(&vertex.second),
                    sizeof(vertex_uid_t::second_type));
        std::memcpy(key.data() + 1 + sizeof(vertex_uid_t::first_type) +
                        sizeof(vertex_uid_t::second_type),
                    reinterpret_cast<const char*>(&type),
                    sizeof(vertex_uid_t::first_type));
    }

    static inline void encode(const vertex_uid_t& vertex1,
                              const vertex_uid_t& vertex2,
                              edge_key_t& key) {
        key[0] = 'E';
        std::memcpy(key.data() + 1,
                    reinterpret_cast<const char*>(&vertex1.first),
                    sizeof(vertex_uid_t::first_type));
        std::memcpy(key.data() + 1 + sizeof(vertex_uid_t::first_type),
                    reinterpret_cast<const char*>(&vertex1.second),
                    sizeof(vertex_uid_t::second_type));
        std::memcpy(key.data() + 1 + sizeof(vertex_uid_t::first_type) +
                        sizeof(vertex_uid_t::second_type),
                    reinterpret_cast<const char*>(&vertex2.first),
                    sizeof(vertex_uid_t::first_type));
        std::memcpy(key.data() + 1 + 2 * sizeof(vertex_uid_t::first_type) +
                        sizeof(vertex_uid_t::second_type),
                    reinterpret_cast<const char*>(&vertex2.second),
                    sizeof(vertex_uid_t::second_type));
    }

    static inline void encode(const vertex_uid_t& vertex1,
                              const vertex_uid_t& vertex2,
                              std::array<edge_key_t, 1>& keys) {
        encode(vertex1, vertex2, keys[0]);
    }

    static inline void encode(const vertex_uid_t& vertex1,
                              const vertex_uid_t& vertex2,
                              std::array<edge_key_t, 2>& keys) {
        encode(vertex1, vertex2, keys[0]);
        encode(vertex2, vertex1, keys[1]);
    }

    static inline void encode_reversed_edge(const char* data, size_t size, edge_key_t& key) {
        static_cast<void>(size);
        assert(size == std::tuple_size<edge_key_t>::value);
        key[0] = 'E';
        std::memcpy(key.data() + 1,
                    data + 1 + sizeof(vertex_uid_t::first_type) + sizeof(vertex_uid_t::second_type),
                    sizeof(vertex_uid_t::first_type) + sizeof(vertex_uid_t::second_type));
        std::memcpy(key.data() + 1 + sizeof(vertex_uid_t::first_type) +
                        sizeof(vertex_uid_t::second_type),
                    data + 1,
                    sizeof(vertex_uid_t::first_type) + sizeof(vertex_uid_t::second_type));
    }
    /**
     *  \}
     */

    /** \name Key decoding functions
     * \{
     */
    static inline void decode_edge_dest(const char* data, size_t size, vertex_uid_t& vertex) {
        static_cast<void>(size);
        assert(size == std::tuple_size<edge_key_t>::value);
        assert(data[0] == 'E');
        std::memcpy(reinterpret_cast<char*>(&vertex.first),
                    data + 1 + sizeof(vertex_uid_t::first_type) + sizeof(vertex_uid_t::second_type),
                    sizeof(vertex_uid_t::first_type));
        std::memcpy(reinterpret_cast<char*>(&vertex.second),
                    data + 1 + 2 * sizeof(vertex_uid_t::first_type) +
                        sizeof(vertex_uid_t::second_type),
                    sizeof(vertex_uid_t::second_type));
    }

    static inline void decode_vertex(const char* data, size_t size, vertex_uid_t& vertex) {
        static_cast<void>(size);
        assert(size == 1 + sizeof(vertex_uid_t::first_type) + sizeof(vertex_uid_t::second_type));
        assert(data[0] == 'N');
        std::memcpy(reinterpret_cast<char*>(&vertex.first),
                    data + 1,
                    sizeof(vertex_uid_t::first_type));
        std::memcpy(reinterpret_cast<char*>(&vertex.second),
                    data + 1 + sizeof(vertex_uid_t::first_type),
                    sizeof(vertex_uid_t::second_type));
    }

    static inline void decode_edge(const char* data, size_t size, edge_uid_t& edge) {
        decode_edge_dest(data, size, edge.second);
        std::memcpy(reinterpret_cast<char*>(&edge.first.first),
                    data + 1,
                    sizeof(vertex_uid_t::first_type));
        std::memcpy(reinterpret_cast<char*>(&edge.first.second),
                    data + 1 + sizeof(vertex_uid_t::first_type),
                    sizeof(vertex_uid_t::second_type));
    }

    /**
     * \}
     */
};

}  // namespace basalt
