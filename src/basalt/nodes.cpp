#include <basalt/node_iterator.hpp>
#include <basalt/nodes.hpp>

#include "network_impl.hpp"

namespace basalt {

nodes_t::nodes_t(network_impl_t& pimpl) : pimpl_(pimpl) {}

nodes_t::~nodes_t() = default;

status_t nodes_t::has(const node_uid_t& node, bool& result) const {
    return pimpl_.nodes_has(node, result);
}

status_t nodes_t::erase(const node_uid_t& node, bool commit) {
    return pimpl_.nodes_erase(node, commit);
}

status_t nodes_t::count(std::size_t& count) const {
    count = 0;
    return status_t::error_not_implemented();
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
