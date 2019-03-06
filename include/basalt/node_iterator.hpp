#pragma once

#include <iterator>
#include <memory>

#include <basalt/fwd.hpp>

namespace basalt {

/// \brief Iterator class for nodes
class node_iterator: public std::iterator<std::input_iterator_tag, const node_uid_t> {
  public:
    /**
     * \brief Ctors and dtors.
     *  \{
     */

    /**
     * Create an iterator over vertices
     * \param pimpl Pointer to implementation
     * \param from Move iterator at specified index
     */
    node_iterator(const network_impl_t& pimpl, size_t from);

    /**
     * Create an iterator over vertices
     * \param rhs Other iterator
     */
    node_iterator(const node_iterator& rhs);

    /**
     * \brief Move iterator to next element
     * \return this instance
     */
    node_iterator& operator++();

    /**
     * \brief Advance the iterator by specified element positions.
     * \return this instance
     */
    node_iterator& operator++(int);  // NOLINT(cert-dcl21-cpp)

    /** \brief compare 2 vertices iterators.
     */
    bool operator==(const node_iterator& rhs) const;

    /** \brief compare 2 vertices iterators.
     */
    bool operator!=(const node_iterator& rhs) const;

    /**
     * Get vertex at the current iterator position
     * \return constant reference to element
     */
    const value_type& operator*();

    using node_iterator_impl_ptr = std::shared_ptr<node_iterator_impl>;

  private:
    node_iterator_impl_ptr pimpl_;
};

}  // namespace basalt
