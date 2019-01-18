#ifndef BASALT_NODE_ITERATOR_HPP
#define BASALT_NODE_ITERATOR_HPP

#include <iterator>
#include <memory>

#include <basalt/fwd.hpp>

namespace basalt {

/// \brief Iterator class for nodes
class node_iterator: public std::iterator<std::input_iterator_tag, const node_uid_t> {
  public:
    node_iterator(const network_impl_t& pimpl, size_t from);
    node_iterator(const node_iterator& rhs);
    node_iterator& operator++();
    node_iterator& operator++(int);
    bool operator==(const node_iterator& rhs) const;
    bool operator!=(const node_iterator& rhs) const;
    const value_type& operator*();

    using node_iterator_impl_ptr = std::shared_ptr<node_iterator_impl>;

  private:
    node_iterator_impl_ptr pimpl_;
};

}  // namespace basalt
#endif  // BASALT_NODE_ITERATOR_HPP
