#ifndef BASALT_CONNECTIONS_HPP
#define BASALT_CONNECTIONS_HPP

#include <basalt/fwd.hpp>
#include <basalt/network.hpp>
#include <basalt/status.hpp>

namespace basalt {

class connections_t {
  public:
    explicit connections_t(network_pimpl_t& pimpl);
    /**
     * \brief Create an edge between 2 nodes.
     * Connection can be inserted even if nodes aren't yet in the graph.
     * It is visible only if both nodes are inserted.
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t connect(const network_t::node_uid_t& node1,
                     const network_t::node_uid_t& node2, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Connect a node to several nodes
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t connect(const network_t::node_uid_t& node,
                     const network_t::node_uids_t& nodes, bool commit = false);

    /**
     * \brief check connectivity between 2 nodes
     * \param node1
     * \param node2
     * \return a pair where \a first member is true if both nodes exists and
     * are connected, false otherwise. The \a second member provides
     * information whether operation succeeded or not
     */
    std::pair<bool, status_t>
    connected(const network_t::node_uid_t& node1,
              const network_t::node_uid_t& node2) const
        __attribute__((warn_unused_result));

    /**
     * \brief get nodes connected to one node
     * \param node
     * \param connections accumulator where connected nodes are added.
     * \return information whether operation succeeded or not
     */
    status_t get(const network_t::node_uid_t& node,
                 network_t::node_uids_t& connections) const
        __attribute__((warn_unused_result));

    /**
     * \brief get nodes of a specific type connected to one node
     * \param node
     * \param type filter connections by their node type
     * \param connections accumulator where connected nodes are added
     * \return information whether operation succeeded or not
     */
    status_t get(const network_t::node_uid_t& node, network_t::node_t filter,
                 network_t::node_uids_t& connections) const
        __attribute__((warn_unused_result));

    /**
     * \brief remove connection between 2 nodes
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const network_t::node_uid_t& node1,
                   const network_t::node_uid_t& node2, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief remove connections of a given type
     * \param node node from which to remove connections
     * \param type_filter node type filter
     * \param removed number of nodes removed during the operation
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const network_t::node_uid_t& node, network_t::node_t filter,
                   size_t& removed, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief remove all connections of a node
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase_connections(const network_t::node_uid_t& node,
                               bool commit = false)
        __attribute__((warn_unused_result));

    std::pair<std::size_t, status_t> count() const
        __attribute__((warn_unused_result));

  private:
    network_pimpl_t& pimpl_;
};

} // namespace basalt

#endif // BASALT_CONNECTIONS_HPP
