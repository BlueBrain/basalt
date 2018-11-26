#ifndef BASALT_NODES_HPP
#define BASALT_NODES_HPP

#include <basalt/fwd.hpp>
#include <basalt/network.hpp>
#include <basalt/status.hpp>

namespace basalt {

class nodes_t {
  public:
    explicit nodes_t(network_impl_t& pimpl);
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
    std::pair<node_uid_t, status_t>
    insert(node_t type, node_id_t id, const T& payload, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Retrieve a node from the graph
     * \tparam T node payload type
     * \param uid node unique identifier
     * \param payload object updated if node is present
     * \return information whether operation succeeded or not
     */
    template <typename T>
    status_t get(node_uid_t uid, T& payload) const
        __attribute__((warn_unused_result));

    status_t has(const node_uid_t& node, bool& result) const
        __attribute__((warn_unused_result));

    /**
     * \brief Remove a node from the graph
     * \param node pair of node type and identifier
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const node_uid_t& node, bool commit = false)
        __attribute__((warn_unused_result));

  private:
    network_impl_t& pimpl_;
};
} // namespace basalt

#include <basalt/nodes.hxx>

#endif // BASALT_NODES_HPP
