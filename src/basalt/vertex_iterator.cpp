/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include <basalt/vertex_iterator.hpp>
#include <basalt/vertices.hpp>

#include "graph_impl.hpp"
#include "vertex_iterator_impl.hpp"

namespace basalt {

template <EdgeOrientation Orientation>
VertexIterator::VertexIterator(const basalt::GraphImpl<Orientation>& pimpl, size_t from) {
    // \fixme TCL this limit value is VertexIteratorImpl internal stuff
    // and so should not be used here
    if (from == std::numeric_limits<std::size_t>::max()) {
        pimpl_ = VertexIteratorImpl_ptr(nullptr);
    } else {
        pimpl_ = pimpl.vertex_iterator(from);
        std::advance(*this, static_cast<VertexIterator::difference_type>(from));
    }
}

VertexIterator::VertexIterator(const basalt::VertexIterator& other)
    : pimpl_(other.pimpl_) {}

VertexIterator& VertexIterator::operator++() {
    ++*pimpl_;
    return *this;
}

const VertexIterator VertexIterator::operator++(int value) {
    const VertexIterator result(*this);
    while (value-- > 0) {
        this->operator++();
    }
    return result;
}

bool VertexIterator::operator==(const basalt::VertexIterator& other) const {
    if (pimpl_) {
        if (other.pimpl_) {
            return *this->pimpl_ == *other.pimpl_;
        }
        return pimpl_->end_reached();
    }
    return !other.pimpl_;
}

bool VertexIterator::operator!=(const basalt::VertexIterator& other) const {
    return !(*this == other);
}

const VertexIterator::value_type& VertexIterator::operator*() {
    return **pimpl_;
}

template VertexIterator::VertexIterator(const basalt::GraphImpl<EdgeOrientation::directed>& pimpl,
                                        size_t from);
template VertexIterator::VertexIterator(const basalt::GraphImpl<EdgeOrientation::undirected>& pimpl,
                                        size_t from);

}  // namespace basalt
