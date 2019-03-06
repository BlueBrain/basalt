#pragma once

#include <iterator>
#include <memory>

#include <basalt/fwd.hpp>

namespace basalt {

/// \brief Iterator class for vertices
class VertexIterator: public std::iterator<std::input_iterator_tag, const vertex_uid_t> {
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
    VertexIterator(const GraphImpl& pimpl, size_t from);

    /**
     * Create an iterator over vertices
     * \param rhs Other iterator
     */
    VertexIterator(const VertexIterator& rhs);

    /**
     * \brief Move iterator to next element
     * \return this instance
     */
    VertexIterator& operator++();

    /**
     * \brief Advance the iterator by specified element positions.
     * \return this instance
     */
    VertexIterator& operator++(int);  // NOLINT(cert-dcl21-cpp)

    /** \brief compare 2 vertices iterators.
     */
    bool operator==(const VertexIterator& rhs) const;

    /** \brief compare 2 vertices iterators.
     */
    bool operator!=(const VertexIterator& rhs) const;

    /**
     * Get vertex at the current iterator position
     * \return constant reference to element
     */
    const value_type& operator*();

    using VertexIteratorImpl_ptr = std::shared_ptr<VertexIteratorImpl>;

  private:
    VertexIteratorImpl_ptr pimpl_;
};

}  // namespace basalt
