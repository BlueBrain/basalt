#include <basalt/connections.hpp>

#include "network_impl.hpp"

namespace basalt {

connections_t::connections_t(network_impl_t& pimpl)
    : pimpl_(pimpl) {}

status_t connections_t::insert(const node_uid_t& node1, const node_uid_t& node2, bool commit) {
    return pimpl_.connections_insert(node1, node2, {nullptr, 0}, commit);
}

status_t connections_t::insert(const node_uid_t& node1,
                               const node_uid_t& node2,
                               const char* data,
                               std::size_t size,
                               bool commit) {
    return pimpl_.connections_insert(node1, node2, {data, size}, commit);
}

status_t connections_t::insert(const node_uid_t& node,
                               const node_uids_t& nodes,
                               const std::vector<const char*>& data,
                               const std::vector<std::size_t>& sizes,
                               bool commit) {
    return pimpl_.connections_insert(node, nodes, data, sizes, commit);
}

status_t connections_t::has(const node_uid_t& node1, const node_uid_t& node2, bool& res) const {
    return pimpl_.connections_has(node1, node2, res);
}

status_t connections_t::get(const node_uid_t& node, node_uids_t& connections) const {
    return pimpl_.connections_get(node, connections);
}

status_t connections_t::get(const node_uid_t& node, node_t filter, node_uids_t& connections) const {
    return pimpl_.connections_get(node, filter, connections);
}

status_t connections_t::erase(const node_uid_t& node1, const node_uid_t& node2, bool commit) {
    return pimpl_.connections_erase(node1, node2, commit);
}

status_t connections_t::erase(const node_uid_t& node, node_t filter, size_t& removed, bool commit) {
    return pimpl_.connections_erase(node, filter, removed, commit);
}

status_t connections_t::erase(const node_uid_t& node, std::size_t& removed, bool commit) {
    return pimpl_.connections_erase(node, removed, commit);
}

}  // namespace basalt
