#include "node_iterator_impl.hpp"

#include <rocksdb/db.h>
#include <rocksdb/slice_transform.h>

#include "graph.hpp"
#include "network_impl.hpp"

namespace basalt {

node_iterator_impl::node_iterator_impl(const basalt::db_t& db, rocksdb::ColumnFamilyHandle* nodes,
                                       const std::string& /*prefix*/, std::size_t position)
    : position_(position) {
    iter_ = db->NewIterator(rocksdb::ReadOptions(), nodes);
    iter_->SeekToFirst();
    if (!iter_->Valid()) {
        position_ = std::numeric_limits<std::size_t>::max();
    }
    network_impl_t::to_status(iter_->status()).raise_on_error();
}

bool node_iterator_impl::operator==(const basalt::node_iterator_impl& rhs) const {
    return this->iter_ == rhs.iter_ and this->position_ == rhs.position_;
}

node_iterator_impl& node_iterator_impl::operator++() {
    iter_->Next();
    if (!iter_->Valid()) {
        position_ = std::numeric_limits<std::size_t>::max();
    } else {
        ++position_;
    }
    return *this;
}

bool node_iterator_impl::operator!=(const basalt::node_iterator_impl& rhs) const {
    return !(*this == rhs);
}

const node_iterator_impl::value_type& node_iterator_impl::operator*() {
    auto const& slice = iter_->key();
    graph::decode_node(slice.data(), slice.size(), value);
    return value;
}

bool node_iterator_impl::end_reached() const {
    return position_ == std::numeric_limits<std::size_t>::max();
}

}  // namespace basalt
