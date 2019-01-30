#ifndef BASALT_NODE_ITERATOR_IMPL_HPP
#define BASALT_NODE_ITERATOR_IMPL_HPP

#include <type_traits>

#include <basalt/node_iterator.hpp>

#include "fwd.hpp"

namespace basalt {

class node_iterator_impl {
  public:
    using value_type = node_iterator::value_type;
    explicit node_iterator_impl(const db_t& db,
                                rocksdb::ColumnFamilyHandle* nodes,
                                const std::string& prefix,
                                std::size_t position);

    inline std::size_t position_get() const {
        return position_;
    }

    node_iterator_impl& operator++();
    bool operator==(const node_iterator_impl& rhs) const;
    bool operator!=(const node_iterator_impl& rhs) const;
    const value_type& operator*();

    bool end_reached() const;

  private:
    rocksdb::Iterator* iter_;
    std::size_t position_;
    std::remove_const<value_type>::type value;
};

}  // namespace basalt

#endif  // BASALT_NODE_ITERATOR_IMPL_HPP
