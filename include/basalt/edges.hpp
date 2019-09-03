/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

#include <basalt/fwd.hpp>
#include <basalt/status.hpp>

namespace basalt {

/**
 * Manipulate the edges of a graph
 */
template <EdgeOrientation Orientation>
class Edges {
  public:
    /**
     * \brief Construct a \a Edges
     * \param pimpl Pointer to implementation
     */
    explicit Edges(GraphImpl<Orientation>& pimpl);

    /**
     * \brief Iterator over the edges of the graph
     * \param position starting position, default at the beginning
     * \return edge iterator
     */
    EdgeIterator begin(std::size_t position = 0) const;

    /**
     * \return an iterator referring to the past-the-end
     */
    EdgeIterator end() const;

    /**
     * \brief Create an edge between 2 vertices.
     * Both vertices must already be in the graph.
     * \param vertex1 one end of the edge
     * \param vertex2 second end of the edge
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status insert(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Create an edge between 2 vertices.
     * Both vertices must already be in the graph.
     * \param vertex1 one end of the edge
     * \param vertex2 second end of the edge
     * \param data payload of the edge
     * \param size payload length
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status insert(const vertex_uid_t& vertex1,
                  const vertex_uid_t& vertex2,
                  const char* data,
                  std::size_t size,
                  bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief Create edges between a vertex and several vertices
     * \param vertex the vertex to connect to others
     * \param vertices the vertices to connect to \a vertex
     * \param data payloads of every edges to create
     * \param sizes the sizes of the payloads
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status insert(const vertex_uid_t& vertex,
                  const vertex_uids_t& vertices,
                  const std::vector<const char*>& data = {},
                  const std::vector<std::size_t>& sizes = {},
                  bool commit = false);

    /**
     * \brief Create edges between a vertex and several vertices of the same type
     * \param vertex the vertex to connect to others
     * \param type target vertices type
     * \param vertices the vertices to connect to \a vertex
     * \param num_vertices number of target vertices
     * \param create_vertices whether vertices should be created as well
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status insert(const vertex_uid_t& vertex,
                  vertex_t type,
                  const vertex_id_t* vertices,
                  size_t num_vertices,
                  bool create_vertices = false,
                  bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief Creates edges between a vertex and several vertices of the same type
     * \param vertex the vertex to connect to others
     * \param type target vertices type
     * \param vertices the vertices to connect to \a vertex
     * \param vertex_payloads payload of every target vertex
     * Use \a nullptr is there is no payload.
     * \param vertex_payloads_sizes payload size of every target vertex
     * \param num_vertices number of target vertices
     * \param create_vertices whether vertices should be created as well
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status insert(const vertex_uid_t& vertex,
                  vertex_t type,
                  const std::size_t* vertices,
                  const char* const* vertex_payloads,
                  const std::size_t* vertex_payloads_sizes,
                  size_t num_vertices,
                  bool create_vertices = false,
                  bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief Retrieve an edge payload
     * \param edge unique identifier to retrieve
     * \param value payload object updated if the edge exists and has an associated payload
     * \return information whether operation succeeded or not
     */
    Status get(const edge_uid_t& edge, std::string* value) const
        __attribute__((warn_unused_result));

    /**
     * \brief check connectivity between 2 vertices
     * \param vertex1 first end of the edge to look for
     * \param vertex2 second end of the edge to look for
     * \param result a boolean indicating whether vertex1 and vertex2 are connected
     * \return provides information whether operation succeeded or not
     */
    Status has(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool& result) const
        __attribute__((warn_unused_result));

    /**
     * \brief get vertices connected to a vertex
     * \param vertex for directed graph, the head of the edges to look for,
     * any end of the edges otherwise
     * \param edges accumulator where connected vertices are added.
     * \return information
     * whether operation succeeded or not
     */
    Status get(const vertex_uid_t& vertex, vertex_uids_t& edges) const
        __attribute__((warn_unused_result));

    /**
     * \brief get vertices of a specific type connected to one vertex
     * \param vertex one end of the edges to look
     * \param filter type of target vertices
     * \param edges accumulator where connected vertices are added
     * \return information whether operation succeeded or not
     */
    Status get(const vertex_uid_t& vertex, vertex_t filter, vertex_uids_t& edges) const
        __attribute__((warn_unused_result));

    /**
     * \brief remove edge between 2 vertices
     * \param vertex1 one end of the edge to remove
     * \param vertex2 other end of the edge to remove
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status erase(const vertex_uid_t& vertex1, const vertex_uid_t& vertex2, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief remove edges of a given type
     * \param vertex vertex from which to remove edges
     * \param filter type of target vertices
     * \param removed number of vertices removed during the operation
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status erase(const vertex_uid_t& vertex, vertex_t filter, size_t& removed, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief remove all edges of a vertex
     * \param vertex one end of the edges to remove
     * \param commit whether uncommitted operations should be flushed or not
     * \param removed number of vertices removed during the operation
     * \return information whether operation succeeded or not
     */
    Status erase(const vertex_uid_t& vertex, std::size_t& removed, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \param count non-const reference updated by this member function
     * with the number of edges in the graph
     * \return information whether operation succeeded or not
     */
    Status count(std::size_t& count) const __attribute__((warn_unused_result));

    /**
     * \brief Remove all edges of the graph along. Vertices are kept intact.
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status clear(bool commit) __attribute__((warn_unused_result));

  private:
    GraphImpl<Orientation>& pimpl_;
};

extern template class Edges<EdgeOrientation::directed>;
extern template class Edges<EdgeOrientation::undirected>;

}  // namespace basalt
