#ifndef BASALT_NODE_ITERATOR_IMPL_HPP
#define BASALT_NODE_ITERATOR_IMPL_HPP

#include "fwd.hpp"

namespace basalt {

class node_iterator_impl {
  public:
    explicit node_iterator_impl(const db_t& db, const std::string& prefix,
                                std::size_t position);

  private:
    rocksdb::Iterator* iter_;
};

} // namespace basalt

#endif // BASALT_NODE_ITERATOR_IMPL_HPP
