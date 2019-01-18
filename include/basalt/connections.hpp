#ifndef BASALT_CONNECTIONS_HPP
#define BASALT_CONNECTIONS_HPP

#include <basalt/fwd.hpp>
#include <basalt/status.hpp>

namespace basalt {

class connections_t {
  public:
    explicit connections_t(network_impl_t& pimpl);
    /**
     * \brief Create an edge between 2 nodes.
     * Connection can be inserted even if nodes aren't yet in the graph.
     * It is visible only if both nodes are inserted.
     * \param node1 first node to connect
     * \param node2 second node to connect
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t insert(const node_uid_t& node1, const node_uid_t& node2, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Create an edge between 2 nodes.
     * Connection can be inserted even if nodes aren't yet in the graph.
     * It is visible only if both nodes are inserted.
     * \param node1 first node to connect
     * \param node2 second node to connect
     * \param data payload
     * \param size payload length
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t insert(const node_uid_t& node1, const node_uid_t& node2, const char* data,
                    std::size_t size, bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief Connect a node to several nodes
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t insert(const node_uid_t& node, const node_uids_t& nodes,
                    const std::vector<const char*>& data = {},
                    const std::vector<std::size_t>& sizes = {}, bool commit = false);

    status_t insert(node_t type, node_id_t id, const char* data, std::size_t size, node_uid_t& node,
                    bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief check connectivity between 2 nodes
     * \param node1
     * \param node2
     * \param res a boolean indicating whether node1 and node2 are connected
     * \return provides information whether operation succeeded or not
     */
    status_t has(const node_uid_t& node1, const node_uid_t& node2, bool& res) const
        __attribute__((warn_unused_result));

    /**
     * \brief get nodes connected to one node
     * \param node
     * \param connections accumulator where connected nodes are added.
     * \return information whether operation succeeded or not
     */
    status_t get(const node_uid_t& node, node_uids_t& connections) const
        __attribute__((warn_unused_result));

    /**
     * \brief get nodes of a specific type connected to one node
     * \param node
     * \param type filter connections by their node type
     * \param connections accumulator where connected nodes are added
     * \return information whether operation succeeded or not
     */
    status_t get(const node_uid_t& node, node_t filter, node_uids_t& connections) const
        __attribute__((warn_unused_result));

    /**
     * \brief remove connection between 2 nodes
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const node_uid_t& node1, const node_uid_t& node2, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief remove connections of a given type
     * \param node node from which to remove connections
     * \param type_filter node type filter
     * \param removed number of nodes removed during the operation
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const node_uid_t& node, node_t filter, size_t& removed, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief remove all connections of a node
     * \param commit whether uncommitted operations should be flushed or not
     * \param removed number of nodes removed during the operation
     * \return information whether operation succeeded or not
     */
    status_t erase(const node_uid_t& node, std::size_t& removed, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief returns the number of connections in the network
     * \param res the number of connections
     * \return information whether operation succeeded or not
     */
    status_t count(std::size_t& res) const __attribute__((warn_unused_result));

  private:
    network_impl_t& pimpl_;
};

}  // namespace basalt

#endif  // BASALT_CONNECTIONS_HPP
