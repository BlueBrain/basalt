#ifndef BASALT_NETWORK_HXX
#define BASALT_NETWORK_HXX

#include <basalt/network.hpp>

namespace basalt {

template <typename T>
std::pair<network_t::node_uid_t, status_t>
nodes_t::insert(network_t::node_t type, network_t::node_id_t id,
                const T& /*payload*/, bool /*commit*/) {
    return {{type, id}, {1, "operation-no-implemented"}};
}

/**
 * \brief Retrieve a node from the graph
 * \tparam T node payload type
 * \param uid node unique identifier
 * \param payload object updated if node is present
 * \return information whether operation succeeded or not
 */
template <typename T>
status_t nodes_t::get(network_t::node_uid_t /*uid*/, T& /*payload*/) const {
    return {1, "operation-no-implemented"};
}

} // namespace basalt

#endif // !BASALT_NETWORK_HXX
