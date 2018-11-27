#include <basalt/connections.hpp>

#include "network_impl.hpp"

namespace basalt {

connections_t::connections_t(network_impl_t& pimpl) : pimpl_(pimpl) {}

status_t connections_t::connect(const node_uid_t& node1,
                                const node_uid_t& node2,
                                const payload_t& payload, bool commit) {
    return pimpl_.connections_connect(node1, node2, payload, commit);
}

status_t connections_t::connect(const node_uid_t& node,
                                const node_uids_t& nodes,
                                const payload_t& payload, bool commit) {
    return pimpl_.connections_connect(node, nodes, payload, commit);
}

std::pair<bool, status_t>
connections_t::connected(const node_uid_t& node1,
                         const node_uid_t& node2) const {
    return pimpl_.connections_connected(node1, node2);
}

status_t connections_t::get(const node_uid_t& node,
                            node_uids_t& connections) const {
    return pimpl_.connections_get(node, connections);
}

status_t connections_t::get(const node_uid_t& node, node_t filter,
                            node_uids_t& connections) const {
    return pimpl_.connections_get(node, filter, connections);
}

status_t connections_t::erase(const node_uid_t& node1, const node_uid_t& node2,
                              bool commit) {
    return pimpl_.connections_erase(node1, node2, commit);
}

status_t connections_t::erase(const node_uid_t& node, node_t filter,
                              size_t& removed, bool commit) {
    return pimpl_.connections_erase(node, filter, removed, commit);
}

status_t connections_t::erase(const node_uid_t& node, std::size_t& removed,
                              bool commit) {
    return pimpl_.connections_erase(node, removed, commit);
}

} // namespace basalt
