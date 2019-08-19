/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

#include <basalt/fwd.hpp>
#include <basalt/graph.hpp>
#include <basalt/status.hpp>

namespace basalt {

/**
 * \brief Manipulate graph vertices
 */
template <EdgeOrientation Orientation>
class Vertices {
  public:
    /**
     * Build a \a Vertices
     * \param pimpl Pointer to implementation
     */
    explicit Vertices(GraphImpl<Orientation>& pimpl);
    /**
     * \brief Iterate over vertices
     * \param position starting position, default at the beginning
     * \return vertex iterator
     */
    VertexIterator begin(std::size_t position = 0) const;

    /**
     * \return an iterator referring to the past-the-end
     */
    VertexIterator end() const;

    /**
     * \brief get number of vertices in the graph
     * \param count non-const reference updated by this member function
     * with the number of vertices in the graph
     * \return information whether operation succeeded or not
     */
    Status count(std::size_t& count) const __attribute__((warn_unused_result));

    /**
     * \brief get number of vertices of a certain type in the graph
     * \param type type of vertex
     * \param count non-const reference updated by this member function
     * \return information whether operation succeeded or not
     */
    Status count(vertex_t type, std::size_t& count) const __attribute__((warn_unused_result));

    /**
     * \brief Remove all vertices of the graph along with their edges
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status clear(bool commit) __attribute__((warn_unused_result));

    /**
     * \brief Insert a vertex in the graph
     * \param vertex the vertex unique identifier to insert
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status insert(const vertex_uid_t& vertex, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Insert a vertex in the graph.
     * \tparam Payload vertex payload type
     * \param vertex the vertex unique identifier to insert
     * \param data vertex payload
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    template <typename Payload>
    Status insert(const vertex_uid_t& vertex, const Payload& data, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Insert a vertex in the graph.
     * \param vertex vertex unique identifier to insert
     * \param data vertex payload
     * \param size payload length
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status insert(const vertex_uid_t& vertex,
                  const char* data,
                  std::size_t size,
                  bool commit = false) __attribute__((warn_unused_result));
    /**
     * \brief Insert a list of vertices all at once
     * \param types array of vertex types
     * \param ids array of vertex identifiers
     * \param payloads array of serialized data, \a nullptr if
     * none of the vertices have a payload
     * \param payloads_sizes size of every payloads, \a nullptr
     * if none of the vertex have a payload
     * \param num_vertices number of vertexs to insert
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status insert(const vertex_t* types,
                  const vertex_id_t* ids,
                  const char* const* payloads,
                  const std::size_t* payloads_sizes,
                  size_t num_vertices,
                  bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief Retrieve a vertex from the graph
     * \tparam T vertex payload type
     * \param vertex the vertex to retrieve
     * \param payload object updated if vertex is present
     * \return information whether operation succeeded or not
     */
    template <typename T>
    Status get(const vertex_uid_t& vertex, T& payload) const __attribute__((warn_unused_result));

    /**
     * \brief Retrieve a vertex payload
     * \param vertex the vertex to retrieve
     * \param value payload object updated if vertex exists and has an associated payload
     * \return information whether operation succeeded or not
     */
    Status get(const vertex_uid_t& vertex, std::string* value) const
        __attribute__((warn_unused_result));

    /**
     * \brief Check presence of a vertex in the graph
     * \param vertex the vertex to look for
     * \param result reference set to true if vertex exists, false otherwise
     * \return information whether operation managed to update \a result
     */
    Status has(const vertex_uid_t& vertex, bool& result) const __attribute__((warn_unused_result));

    /**
     * \brief Remove a vertex from the graph
     * \param vertex the vertex to remove
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status erase(const vertex_uid_t& vertex, bool commit = false)
        __attribute__((warn_unused_result));

  private:
    GraphImpl<Orientation>& pimpl_;
};

extern template class Vertices<EdgeOrientation::directed>;
extern template class Vertices<EdgeOrientation::undirected>;

}  // namespace basalt

#include <basalt/vertices.ipp>
