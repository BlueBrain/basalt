#include <basalt/vertex_iterator.hpp>
#include <basalt/vertices.hpp>

#include "graph_impl.hpp"
#include "vertex_iterator_impl.hpp"

namespace basalt {

VertexIterator::VertexIterator(const basalt::GraphImpl& pimpl, size_t from) {
    // \fixme TCL this limit value is VertexIteratorImpl internal stuff
    // and so should not be used here
    if (from == std::numeric_limits<std::size_t>::max()) {
        pimpl_ = VertexIteratorImpl_ptr(nullptr);
    } else {
        pimpl_ = pimpl.VertexIterator(from);
        std::advance(*this, from);
    }
}

VertexIterator::VertexIterator(const basalt::VertexIterator& rhs)
    : pimpl_(rhs.pimpl_) {}

VertexIterator& VertexIterator::operator++() {
    ++*pimpl_;
    return *this;
}

VertexIterator& VertexIterator::operator++(int value) {  // NOLINT(cert-dcl21-cpp)
    while (value-- > 0) {
        this->operator++();
    }
    return *this;
}

bool VertexIterator::operator==(const basalt::VertexIterator& rhs) const {
    if (pimpl_) {
        if (rhs.pimpl_) {
            return *this->pimpl_ == *rhs.pimpl_;
        }
        return pimpl_->end_reached();
    }
    return !rhs.pimpl_;
}

bool VertexIterator::operator!=(const basalt::VertexIterator& rhs) const {
    return !(*this == rhs);
}

const VertexIterator::value_type& VertexIterator::operator*() {
    return **pimpl_;
}

}  // namespace basalt
