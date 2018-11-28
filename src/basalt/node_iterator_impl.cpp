#include "node_iterator_impl.hpp"

#include <rocksdb/db.h>
#include <rocksdb/slice_transform.h>

#include "network_impl.hpp"

namespace basalt {

node_iterator_impl::node_iterator_impl(const basalt::db_t& db,
                                       const std::string& prefix,
                                       std::size_t /*position*/) {
    iter_ = db->NewIterator(rocksdb::ReadOptions());
    iter_->Seek(rocksdb::Slice(prefix));
    network_impl_t::to_status(iter_->status()).raise_on_error();
}

} // namespace basalt
