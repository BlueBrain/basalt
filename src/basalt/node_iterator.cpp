#include <basalt/node_iterator.hpp>
#include <basalt/nodes.hpp>

#include "network_impl.hpp"
#include "node_iterator_impl.hpp"

namespace basalt {

node_iterator::node_iterator(const basalt::network_impl_t& pimpl, size_t from) {
    // \fixme TCL this limit value is node_iterator_impl internal stuff
    // and so should not be used here
    if (from == std::numeric_limits<std::size_t>::max()) {
        pimpl_ = node_iterator_impl_ptr(nullptr);
    } else {
        pimpl_ = pimpl.node_iterator(from);
        std::advance(*this, from);
    }
}

node_iterator::node_iterator(const basalt::node_iterator& rhs)
    : pimpl_(rhs.pimpl_) {}

node_iterator& node_iterator::operator++() {
    ++*pimpl_;
    return *this;
}

node_iterator& node_iterator::operator++(int value) {
    while (value-- > 0) {
        this->operator++();
    }
    return *this;
}

bool node_iterator::operator==(const basalt::node_iterator& rhs) const {
    if (pimpl_) {
        if (rhs.pimpl_) {
            return *this->pimpl_ == *rhs.pimpl_;
        } else {
            if (pimpl_->end_reached()) {
                return true;
            } else {
                return false;
            }
        }
    } else {
        if (rhs.pimpl_) {
            return false;
        } else {
            return true;
        }
    }
}

bool node_iterator::operator!=(const basalt::node_iterator& rhs) const {
    return !(*this == rhs);
}

const node_iterator::value_type& node_iterator::operator*() { return **pimpl_; }

} // namespace basalt
