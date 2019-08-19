/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include "vertex_iterator_impl.hpp"

#include <rocksdb/db.h>
#include <rocksdb/slice_transform.h>

#include "graph_impl.hpp"
#include "graph_kv.hpp"

namespace basalt {

VertexIteratorImpl::VertexIteratorImpl(const basalt::db_t& db,
                                       rocksdb::ColumnFamilyHandle* vertices,
                                       const std::string& /*prefix*/,
                                       std::size_t position)
    : position_(position) {
    iter_ = db->NewIterator(rocksdb::ReadOptions(), vertices);
    iter_->SeekToFirst();
    if (!iter_->Valid()) {
        position_ = std::numeric_limits<std::size_t>::max();
    }
    GraphImpl<EdgeOrientation::directed>::to_status(iter_->status()).raise_on_error();
}

bool VertexIteratorImpl::operator==(const basalt::VertexIteratorImpl& rhs) const {
    return this->iter_ == rhs.iter_ and this->position_ == rhs.position_;
}

VertexIteratorImpl& VertexIteratorImpl::operator++() {
    iter_->Next();
    if (!iter_->Valid()) {
        position_ = std::numeric_limits<std::size_t>::max();
    } else {
        ++position_;
    }
    return *this;
}

bool VertexIteratorImpl::operator!=(const VertexIteratorImpl& rhs) const {
    return !(*this == rhs);
}

const VertexIteratorImpl::value_type& VertexIteratorImpl::operator*() {
    auto const& slice = iter_->key();
    GraphKV::decode_vertex(slice.data(), slice.size(), value);
    return value;
}

bool VertexIteratorImpl::end_reached() const {
    return position_ == std::numeric_limits<std::size_t>::max();
}

}  // namespace basalt
