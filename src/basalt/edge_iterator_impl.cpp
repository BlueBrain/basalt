/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include "edge_iterator_impl.hpp"

#include <rocksdb/db.h>
#include <rocksdb/slice_transform.h>

#include "graph_impl.hpp"
#include "graph_kv.hpp"

namespace basalt {

EdgeIteratorImpl::EdgeIteratorImpl(const basalt::db_t& db,
                                   rocksdb::ColumnFamilyHandle* edges,
                                   const std::string& /*prefix*/,
                                   std::size_t position)
    : position_(position) {
    iter_ = db->NewIterator(rocksdb::ReadOptions(), edges);
    iter_->SeekToFirst();
    if (!iter_->Valid()) {
        position_ = std::numeric_limits<std::size_t>::max();
    }
    GraphImpl<EdgeOrientation::directed>::to_status(iter_->status()).raise_on_error();
}

bool EdgeIteratorImpl::operator==(const basalt::EdgeIteratorImpl& other) const {
    return this->iter_ == other.iter_ and this->position_ == other.position_;
}


EdgeIteratorImpl& EdgeIteratorImpl::operator++() {
    iter_->Next();
    if (!iter_->Valid()) {
        position_ = std::numeric_limits<std::size_t>::max();
    } else {
        ++position_;
    }
    return *this;
}

bool EdgeIteratorImpl::operator!=(const EdgeIteratorImpl& other) const {
    return !(*this == other);
}

const EdgeIteratorImpl::value_type& EdgeIteratorImpl::operator*() {
    auto const& slice = iter_->key();
    GraphKV::decode_edge(slice.data(), slice.size(), value);
    return value;
}

bool EdgeIteratorImpl::end_reached() const {
    return position_ == std::numeric_limits<std::size_t>::max();
}

}  // namespace basalt
