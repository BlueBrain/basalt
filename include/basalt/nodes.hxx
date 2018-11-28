#ifndef BASALT_NETWORK_HXX
#define BASALT_NETWORK_HXX

#include <basalt/network.hpp>
#include <basalt/status.hpp>

namespace basalt {

template <typename T>
status_t nodes_t::insert(node_t type, node_id_t id, const T& /*payload*/,
                         node_uid_t& res, bool /*commit*/) {
    res = {type, id};
    return status_t::error_not_implemented();
}

/**
 * \brief Retrieve a node from the graph
 * \tparam T node payload type
 * \param uid node unique identifier
 * \param payload object updated if node is present
 * \return information whether operation succeeded or not
 */
template <typename T>
status_t nodes_t::get(node_uid_t /*uid*/, T& /*payload*/) const {
    return status_t::error_not_implemented();
}

} // namespace basalt

#endif // !BASALT_NETWORK_HXX
