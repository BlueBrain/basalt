/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include <limits>

#include <basalt/vertex_iterator.hpp>
#include <basalt/vertices.hpp>

#include "graph_impl.hpp"

namespace basalt {

template <bool Ordered>
Vertices<Ordered>::Vertices(GraphImpl<Ordered>& pimpl)
    : pimpl_(pimpl) {}

template <bool Ordered>
Status Vertices<Ordered>::insert(const basalt::vertex_uid_t& vertex,
                                 const char* data,
                                 std::size_t size,
                                 bool commit) {
    return pimpl_.vertices_insert(vertex, {data, size}, commit);
}

template <bool Ordered>
Status Vertices<Ordered>::insert(const basalt::vertex_uid_t& vertex, bool commit) {
    return pimpl_.vertices_insert(vertex, commit);
}

template <bool Ordered>
Status Vertices<Ordered>::insert(const vertex_t* types,
                                 const vertex_id_t* ids,
                                 const char* const* payloads,
                                 const std::size_t* payloads_sizes,
                                 size_t num_vertices,
                                 bool commit) {
    if (payloads == nullptr) {
        return pimpl_.vertices_insert({types, num_vertices}, {ids, num_vertices}, {}, {}, commit);
    }
    return pimpl_.vertices_insert({types, num_vertices},
                                  {ids, num_vertices},
                                  {payloads, num_vertices},
                                  {payloads_sizes, num_vertices},
                                  commit);
}

template <bool Ordered>
Status Vertices<Ordered>::has(const vertex_uid_t& vertex, bool& result) const {
    return pimpl_.vertices_has(vertex, result);
}

template <bool Ordered>
Status Vertices<Ordered>::get(const basalt::vertex_uid_t& vertex, std::string* value) const {
    return pimpl_.vertices_get(vertex, value);
}

template <bool Ordered>
Status Vertices<Ordered>::erase(const vertex_uid_t& vertex, bool commit) {
    return pimpl_.vertices_erase(vertex, commit);
}

template <bool Ordered>
Status Vertices<Ordered>::count(std::size_t& count) const {
    return pimpl_.vertices_count(count);
}

template <bool Ordered>
Status Vertices<Ordered>::count(vertex_t type, std::size_t& count) const {
    return pimpl_.vertices_count(type, count);
}

template <bool Ordered>
VertexIterator Vertices<Ordered>::begin(size_t position) const {
    return {pimpl_, position};
}

template <bool Ordered>
VertexIterator Vertices<Ordered>::end() const {
    return {pimpl_, std::numeric_limits<std::size_t>::max()};
}

template <bool Ordered>
Status Vertices<Ordered>::clear(bool commit) {
    return pimpl_.vertices_clear(commit);
}

template class Vertices<true>;
template class Vertices<false>;

}  // namespace basalt
