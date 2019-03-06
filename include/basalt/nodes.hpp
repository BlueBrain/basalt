#pragma once

#include <basalt/fwd.hpp>
#include <basalt/network.hpp>
#include <basalt/status.hpp>

namespace basalt {

/**
 * \brief Manipulate graph vertices
 */
class nodes_t {
  public:
    /**
     * Build a \a nodes_t
     * \param pimpl Pointer to implementation
     */
    explicit nodes_t(network_impl_t& pimpl);
    /**
     * \brief Iterate over nodes
     * \param pos starting position, default at the beginning
     * @return node iterator
     */
    node_iterator begin(std::size_t pos = 0) const;

    /**
     * @return an iterator referring to the past-the-end
     */
    node_iterator end() const;

    /**
     * @return number of nodes in the graph, and operation status.
     */
    status_t count(std::size_t& count) const __attribute__((warn_unused_result));

    /**
     * \brief Insert a node in the graph
     * \param type node type
     * \param id node identifier
     * \param node unique node identifier returned to caller
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t insert(node_t type, node_id_t id, node_uid_t& node, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Insert a node in the graph.
     * \tparam T node payload type
     * \param type node type
     * \param id node identifier
     * \param data node payload
     * \param node unique node identifier returned to caller
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    template <typename Payload>
    status_t insert(node_t type,
                    node_id_t id,
                    const Payload& data,
                    node_uid_t& node,
                    bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief Insert a node in the graph.
     * \tparam T node payload type
     * \param type node type
     * \param id node identifier
     * \param data node payload
     * \param size payload length
     * \param node unique node identifier returned to caller
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t insert(node_t type,
                    node_id_t id,
                    const char* data,
                    std::size_t size,
                    node_uid_t& node,
                    bool commit = false) __attribute__((warn_unused_result));
    /**
     * \brief Insert a list of nodes all at once
     * \param types array of node types
     * \param ids array of node identifiers
     * \param payloads array of serialized data, \a nullptr if
     * none of the nodes have a payload
     * \param payloads_sizes size of every payloads, \a nullptr
     * if none of the node have a payload
     * \param num_nodes number of nodes to insert
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t insert(const node_t* types,
                    const node_id_t* ids,
                    const char* const* payloads,
                    const std::size_t* payloads_sizes,
                    size_t num_nodes,
                    bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief Retrieve a node from the graph
     * \tparam T node payload type
     * \param node node unique identifier
     * \param payload object updated if node is present
     * \return information whether operation succeeded or not
     */
    template <typename T>
    status_t get(const node_uid_t& node, T& payload) const __attribute__((warn_unused_result));

    /**
     * \brief Retrive a node from the graph
     * \param node node unique identifier
     * \param value payload object updated if not is present
     * \return information whether operation succeeded or not
     */
    status_t get(const node_uid_t& node, std::string* value) const
        __attribute__((warn_unused_result));

    /**
     * \brief Check presence of a node in the graph
     * \param node node unique identifier
     * \param result reference set to true if node exists, false otherwise
     * \return information whether operation managed to update \a result
     */
    status_t has(const node_uid_t& node, bool& result) const __attribute__((warn_unused_result));

    /**
     * \brief Remove a node from the graph
     * \param node pair of node type and identifier
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const node_uid_t& node, bool commit = false) __attribute__((warn_unused_result));

  private:
    network_impl_t& pimpl_;
};
}  // namespace basalt

#include <basalt/nodes.hxx>
