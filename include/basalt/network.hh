#ifndef BASALT_NETWORK_HH
#define BASALT_NETWORK_HH

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace spdlog {
/// Forward declaration
class logger;
} // namespace spdlog

namespace basalt {

struct status_t {
    status_t(int code_, std::string message_);
    status_t() = delete;

    inline bool operator!() const noexcept { return code != 0; }
    inline explicit operator bool() const noexcept { return code == 0; }

    const int code;
    const std::string& message;
};

/// \brief Undirected Connectivity Graph
class network_t {
  public:
    using node_t = int;
    using node_id_t = std::size_t;
    using node_uid_t = std::pair<node_t, std::size_t>;
    using node_uids_t = std::vector<node_uid_t>;

    /**
     * \brief load graph if present on disk, initialize it otherwise
     * \param path graph location on filesystem
     */
    explicit network_t(const std::string& path);

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

    /**
     * \brief Remove a node from the graph
     * \param id pair of node type and identifier
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const node_uid_t& id, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Remove several nodes from the graph
     * \param ids vector if pair of node types and identifiers
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const node_uids_t& ids, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Create an edge between 2 nodes.
     * Connection can be inserted even if nodes aren't yet in the graph.
     * It is visible only if both nodes are inserted.
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t connect(const node_uid_t& node1, const node_uid_t& node2,
                     bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief Connect a node to several nodes
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t connect(const node_uid_t& node, const node_uids_t& nodes,
                     bool commit = false);

    /**
     * \brief check connectivity between 2 nodes
     * \param node1
     * \param node2
     * \return a pair where \a first member is true if both nodes exists and are
     * connected, false otherwise. The \a second member provides information
     * whether operation succeeded or not
     */
    std::pair<bool, status_t> connected(const node_uid_t& node1,
                                        const node_uid_t& node2) const
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
    status_t get(const node_uid_t& node, node_t filter,
                 node_uids_t& connections) const
        __attribute__((warn_unused_result));

    /**
     * \brief remove connection between 2 nodes
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const node_uid_t& node1, const node_uid_t& node2,
                   bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief remove connections of a given type
     * \param node node from which to remove connections
     * \param type_filter node type filter
     * \param removed number of nodes removed during the operation
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase(const node_uid_t& node, node_t filter, size_t& removed,
                   bool commit = false) __attribute__((warn_unused_result));

    /**
     * \brief remove all connections of a node
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t erase_connections(const node_uid_t& node, bool commit = false)
        __attribute__((warn_unused_result));

    /**
     * \brief Process uncommitted operations
     * \param commit whether uncommitted operations should be flushed or not
     * \return information whether operation succeeded or not
     */
    status_t commit() __attribute__((warn_unused_result));

private:
    const std::string path_;
    std::shared_ptr<spdlog::logger> logger_;

};

/// \brief node_id_t constructor helper function
network_t::node_uid_t make_id(network_t::node_t type, network_t::node_id_t id);

} // namespace basalt

/// \brief specialization to gently push a \a node_uid_t to an output stream
std::ostream& operator<<(std::ostream& ostr,
                         const basalt::network_t::node_uid_t& id);

/// \brief specialization to gently push a \a node_uids_t to an output stream
std::ostream& operator<<(std::ostream& ostr,
                                    const basalt::network_t::node_uids_t& ids);

/// \brief specialization to gently push a \a status_t to an output stream
std::ostream& operator<<(std::ostream& ostr, const basalt::status_t& status);

#include <basalt/network.hxx>

#endif // !BASALT_NETWORK_HH