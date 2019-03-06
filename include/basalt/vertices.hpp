#pragma once

#include <basalt/fwd.hpp>
#include <basalt/graph.hpp>
#include <basalt/status.hpp>

namespace basalt {

/**
 * \brief Manipulate graph vertices
 */
class Vertices {
  public:
    /**
     * Build a \a Vertices
     * \param pimpl Pointer to implementation
     */
    explicit Vertices(GraphImpl& pimpl);
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
     * \return number of vertices in the graph, and operation status.
     */
    Status count(std::size_t& count) const __attribute__((warn_unused_result));

    /**
     * \brief Insert a vertex in the graph
     * \param type the vertex type
     * \param id the vertex identifier
     * \param vertex unique vertex identifier returned to caller
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status insert(vertex_t type, vertex_id_t id, vertex_uid_t& vertex, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Insert a vertex in the graph.
     * \tparam T vertex payload type
     * \param type vertex type
     * \param id vertex identifier
     * \param data vertex payload
     * \param vertex unique vertex identifier returned to caller
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    template <typename Payload>
    Status insert(vertex_t type,
                  vertex_id_t id,
                  const Payload& data,
                  vertex_uid_t& vertex,
                  bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief Insert a vertex in the graph.
     * \tparam T vertex payload type
     * \param type vertex type
     * \param id vertex identifier
     * \param data vertex payload
     * \param size payload length
     * \param vertex unique vertex identifier returned to caller
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    Status insert(vertex_t type,
                  vertex_id_t id,
                  const char* data,
                  std::size_t size,
                  vertex_uid_t& vertex,
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
     * \brief Retrieve a vertex from the graph
     * \param vertex the vertex to retrieve
     * \param value payload object updated if not is present
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
    GraphImpl& pimpl_;
};
}  // namespace basalt

#include <basalt/vertices.ipp>
