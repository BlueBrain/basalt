/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace basalt {

enum class EdgeOrientation {
    undirected,
    directed,
};

/// Forward declarations
template <EdgeOrientation Orientation>
class Edges;
class EdgeIterator;
class EdgeIteratorImpl;
template <EdgeOrientation Orientation>
class GraphImpl;
class VertexIteratorImpl;
class VertexIterator;
template <EdgeOrientation Orientation>
class Vertices;

using vertex_t = int;
using vertex_id_t = std::size_t;
/** \brief a vertex is made of a type and an identifier */
using vertex_uid_t = std::pair<vertex_t, std::size_t>;
using edge_uid_t = std::pair<vertex_uid_t, vertex_uid_t>;
using vertex_uids_t = std::vector<vertex_uid_t>;

}  // namespace basalt
