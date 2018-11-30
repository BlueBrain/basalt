#include <ostream>

#include <rocksdb/db.h>
#include <rocksdb/slice_transform.h>

#include <basalt/network.hpp>

#include "network_impl.hpp"

namespace basalt {

network_t::network_t(const std::string& path)
    : pimpl_(new network_impl_t(path)) {}

network_t::~network_t() = default;

const connections_t& network_t::connections() const {
    return pimpl_->connections_get();
}

connections_t& network_t::connections() { return pimpl_->connections_get(); }

nodes_t& network_t::nodes() { return pimpl_->nodes_get(); }

status_t network_t::commit() { return pimpl_->commit(); }

/////

node_uid_t make_id(node_t type, node_id_t id) { return {type, id}; }

} // namespace basalt

std::ostream& operator<<(std::ostream& ostr, const basalt::node_uid_t& id) {
    return ostr << '(' << id.first << ':' << id.second << ')';
}

std::ostream& operator<<(std::ostream& ostr, const basalt::node_uids_t& ids) {
    ostr << '[';
    for (const auto& id : ids) {
        ostr << id << ' ';
    }
    return ostr << ']';
}
