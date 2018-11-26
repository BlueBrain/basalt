#include <basalt/node_iterator.hpp>
#include <basalt/nodes.hpp>

#include "network_pimpl.hpp"

namespace basalt {

nodes_t::nodes_t(network_pimpl_t& pimpl) : pimpl_(pimpl) {}

nodes_t::~nodes_t() = default;

status_t nodes_t::erase(const network_t::node_uid_t& id, bool commit) {
    pimpl_.logger_get()->debug("erase(id={}, commit={})", id, commit);
    return status_t::not_implemented();
}

status_t nodes_t::erase(const network_t::node_uids_t& ids, bool commit) {
    pimpl_.logger_get()->debug("erase(ids={}, commit={})", ids, commit);
    return status_t::not_implemented();
}

status_t nodes_t::count(std::size_t& count) const {
    count = 0;
    return status_t::not_implemented();
}

node_iterator nodes_t::begin(size_t pos) const {
    return node_iterator(pimpl_, pos);
}

node_iterator nodes_t::end() const {
    auto count = 0lu;
    pimpl_.nodes_get().count(count).raise_on_error();
    return node_iterator(pimpl_, count);
}

} // namespace basalt
