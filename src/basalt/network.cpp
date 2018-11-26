#include <basalt/network.hpp>

#include "network_pimpl.hpp"

namespace basalt {

network_t::network_t(const std::string& path)
    : pimpl_(new network_pimpl_t(path)) {}

network_t::~network_t() = default;

const connections_t& network_t::connections() const {
    return pimpl_->connections_get();
}

connections_t& network_t::connections() { return pimpl_->connections_get(); }

const nodes_t& network_t::nodes() const { return pimpl_->nodes_get(); }

nodes_t& network_t::nodes() { return pimpl_->nodes_get(); }

status_t network_t::commit() {
    pimpl_->logger_get()->debug("commit()");
    return status_t::not_implemented();
}

/////

network_t::node_uid_t make_id(network_t::node_t type, network_t::node_id_t id) {
    return {type, id};
}

} // namespace basalt

std::ostream& operator<<(std::ostream& ostr,
                         const basalt::network_t::node_uid_t& id) {
    return ostr << '(' << id.first << ':' << id.second << ')';
}

std::ostream& operator<<(std::ostream& ostr,
                         const basalt::network_t::node_uids_t& ids) {
    ostr << '[';
    for (const auto& id : ids) {
        ostr << id << ' ';
    }
    return ostr << ']';
}
