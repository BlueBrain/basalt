#include <basalt/node_iterator.hpp>
#include <basalt/nodes.hpp>

#include "network_pimpl.hpp"

namespace basalt {

static const node_iterator::value_type fixme{0, 0};

node_iterator::node_iterator(const basalt::network_pimpl_t& pimpl, size_t from)
    : pimpl_(pimpl) {
    this->pimpl_.logger_get()->debug("node_iterator: creating from {}", from);
};
node_iterator::node_iterator(const basalt::node_iterator& rhs)
    : pimpl_(rhs.pimpl_) {
    this->pimpl_.logger_get()->debug("node_iterator: creating copy");
}

bool node_iterator::operator!=(const basalt::node_iterator& /*rhs*/) const {
    return true;
}

bool node_iterator::operator==(const basalt::node_iterator& /*rhs*/) const {
    return false;
}

const node_iterator::value_type& node_iterator::operator*() { return fixme; }

} // namespace basalt
