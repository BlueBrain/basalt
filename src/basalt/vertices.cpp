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

template <EdgeOrientation Orientation>
Vertices<Orientation>::Vertices(GraphImpl<Orientation>& pimpl)
    : pimpl_(pimpl) {}

template <EdgeOrientation Orientation>
Status Vertices<Orientation>::insert(const basalt::vertex_uid_t& vertex,
                                     const char* data,
                                     std::size_t size,
                                     bool commit) {
    return pimpl_.vertices_insert(vertex, {data, size}, commit);
}

template <EdgeOrientation Orientation>
Status Vertices<Orientation>::insert(const basalt::vertex_uid_t& vertex, bool commit) {
    return pimpl_.vertices_insert(vertex, commit);
}

template <EdgeOrientation Orientation>
Status Vertices<Orientation>::insert(const vertex_t* types,
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

template <EdgeOrientation Orientation>
Status Vertices<Orientation>::has(const vertex_uid_t& vertex, bool& result) const {
    return pimpl_.vertices_has(vertex, result);
}

template <EdgeOrientation Orientation>
Status Vertices<Orientation>::get(const basalt::vertex_uid_t& vertex, std::string* value) const {
    return pimpl_.vertices_get(vertex, value);
}

template <EdgeOrientation Orientation>
Status Vertices<Orientation>::erase(const vertex_uid_t& vertex, bool commit) {
    return pimpl_.vertices_erase(vertex, commit);
}

template <EdgeOrientation Orientation>
Status Vertices<Orientation>::count(std::size_t& count) const {
    return pimpl_.vertices_count(count);
}

template <EdgeOrientation Orientation>
Status Vertices<Orientation>::count(vertex_t type, std::size_t& count) const {
    return pimpl_.vertices_count(type, count);
}

template <EdgeOrientation Orientation>
VertexIterator Vertices<Orientation>::begin(size_t position) const {
    return {pimpl_, position};
}

template <EdgeOrientation Orientation>
VertexIterator Vertices<Orientation>::end() const {
    return {pimpl_, std::numeric_limits<std::size_t>::max()};
}

template <EdgeOrientation Orientation>
Status Vertices<Orientation>::clear(bool commit) {
    return pimpl_.vertices_clear(commit);
}

template class Vertices<EdgeOrientation::directed>;
template class Vertices<EdgeOrientation::undirected>;

}  // namespace basalt
