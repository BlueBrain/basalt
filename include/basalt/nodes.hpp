#ifndef BASALT_NODES_HPP
#define BASALT_NODES_HPP

#include <basalt/fwd.hpp>
#include <basalt/network.hpp>
#include <basalt/status.hpp>

namespace basalt {

class nodes_t {
  public:
    explicit nodes_t(network_pimpl_t& pimpl);
    ~nodes_t();
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
    status_t count(std::size_t& count) const
        __attribute__((warn_unused_result));

    /**
     * \brief Insert a node in the graph.
     * \tparam T node payload type
     * \param type node type
     * \param id node identifier
     * \param payload node payload
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    template <typename T>
    std::pair<network_t::node_uid_t, status_t>
    insert(network_t::node_t type, network_t::node_id_t id, const T& payload,
           bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief Retrieve a node from the graph
     * \tparam T node payload type
     * \param uid node unique identifier
     * \param payload object updated if node is present
     * \return information whether operation succeeded or not
     */
    template <typename T>
    status_t get(network_t::node_uid_t uid, T& payload) const
        __attribute__((warn_unused_result));

    status_t has(const network_t::node_id_t& node, bool& result) const
        __attribute__((warn_unused_result));

    /**
     * \brief Remove a node from the graph
     * \param id pair of node type and identifier
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const network_t::node_uid_t& id, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Remove several nodes from the graph
     * \param ids vector if pair of node types and identifiers
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const network_t::node_uids_t& ids, bool commit = false)
        __attribute__((warn_unused_result));

  private:
    network_pimpl_t& pimpl_;
};
} // namespace basalt

#include <basalt/nodes.hxx>

#endif // BASALT_NODES_HPP
