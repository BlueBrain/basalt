#include <basalt/node_iterator.hpp>
#include <basalt/nodes.hpp>

#include "network_impl.hpp"

namespace basalt {

static const node_iterator::value_type fixme{0, 0};

node_iterator::node_iterator(const basalt::network_impl_t& pimpl,
                             size_t /*from*/)
    : pimpl_(pimpl){};
node_iterator::node_iterator(const basalt::node_iterator& rhs)
    : pimpl_(rhs.pimpl_) {}

bool node_iterator::operator!=(const basalt::node_iterator& /*rhs*/) const {
    return true;
}

bool node_iterator::operator==(const basalt::node_iterator& /*rhs*/) const {
    return false;
}

const node_iterator::value_type& node_iterator::operator*() { return fixme; }

} // namespace basalt
