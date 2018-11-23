#ifndef BASALT_NETWORK_HXX
#define BASALT_NETWORK_HXX

#include <basalt/network.hh>

namespace basalt {

template <typename T>
std::pair<network_t::node_uid_t, status_t>
network_t::insert(node_t type, node_id_t id, const T& /*payload*/,
                  bool /*commit*/) {
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
status_t network_t::get(node_uid_t /*uid*/, T& /*payload*/) const {
    return {1, "operation-no-implemented"};
}

} // namespace basalt

#endif // !BASALT_NETWORK_HXX
