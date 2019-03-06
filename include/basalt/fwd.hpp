#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace basalt {

/// Forward declarations
class Edges;
class GraphImpl;
class VertexIteratorImpl;
class VertexIterator;
class Vertices;

using vertex_t = int;
using vertex_id_t = std::size_t;
/** \brief a vertex is made of a type and an identifier */
using vertex_uid_t = std::pair<vertex_t, std::size_t>;
using vertex_uids_t = std::vector<vertex_uid_t>;

}  // namespace basalt
