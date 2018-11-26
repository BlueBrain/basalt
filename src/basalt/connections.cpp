#include <basalt/connections.hpp>

#include "network_pimpl.hpp"

namespace basalt {

connections_t::connections_t(network_pimpl_t& pimpl) : pimpl_(pimpl) {}

status_t connections_t::connect(const network_t::node_uid_t& node1,
                                const network_t::node_uid_t& node2,
                                bool commit) {
    pimpl_.logger_get()->debug("connect(node1={}, node2={}, commit={})", node1,
                               node2, commit);
    return status_t::not_implemented();
}

status_t connections_t::connect(const network_t::node_uid_t& node,
                                const network_t::node_uids_t& nodes,
                                bool commit) {
    pimpl_.logger_get()->debug("connect({}, {}, {})", node, nodes, commit);
    return status_t::not_implemented();
}

std::pair<bool, status_t>
connections_t::connected(const network_t::node_uid_t& node1,
                         const network_t::node_uid_t& node2) const {
    pimpl_.logger_get()->debug("connected({}, {})", node1, node2);
    return {false, status_t::not_implemented()};
}

status_t connections_t::get(const network_t::node_uid_t& node,
                            network_t::node_uids_t& /*connections*/) const {
    pimpl_.logger_get()->debug("get(node={})", node);
    return status_t::not_implemented();
}

status_t connections_t::get(const network_t::node_uid_t& node,
                            network_t::node_t filter,
                            network_t::node_uids_t& /*connections*/) const {
    pimpl_.logger_get()->debug("get({}, {})", node, filter);
    return status_t::not_implemented();
}

status_t connections_t::erase(const network_t::node_uid_t& node1,
                              const network_t::node_uid_t& node2, bool commit) {
    pimpl_.logger_get()->debug("erase({}, {}, {})", node1, node2, commit);
    return status_t::not_implemented();
}

status_t connections_t::erase(const network_t::node_uid_t& node,
                              network_t::node_t filter, size_t& /*removed*/,
                              bool commit) {
    pimpl_.logger_get()->debug("erase({}, {}, {})", node, filter, commit);
    return status_t::not_implemented();
}

status_t connections_t::erase_connections(const network_t::node_uid_t& node,
                                          bool commit) {
    pimpl_.logger_get()->debug("erase_connections({}, {})", node, commit);
    return status_t::not_implemented();
}

} // namespace basalt