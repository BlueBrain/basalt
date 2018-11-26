#ifndef BASALT_NODE_ITERATOR_HPP
#define BASALT_NODE_ITERATOR_HPP

#include <iterator>

#include <basalt/fwd.hpp>

namespace basalt {

/// \brief Iterator class for nodes
class node_iterator
    : public std::iterator<std::input_iterator_tag, const node_uid_t> {
  public:
    node_iterator(const network_impl_t& pimpl, size_t from);
    node_iterator(const node_iterator& rhs);
    node_iterator& operator++() { return *this; }
    node_iterator& operator++(int) { return *this; }
    bool operator==(const node_iterator& rhs) const;
    bool operator!=(const node_iterator& rhs) const;
    const value_type& operator*();

  private:
    const network_impl_t& pimpl_;
};

} // namespace basalt
#endif // BASALT_NODE_ITERATOR_HPP
