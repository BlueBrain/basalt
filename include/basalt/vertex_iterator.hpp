/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
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
     * \{
     */

    /**
     * Create an iterator over vertices
     * \param pimpl Pointer to implementation
     * \param from Move iterator at specified index
     */
    template <EdgeOrientation Orientation>
    VertexIterator(const GraphImpl<Orientation>& pimpl, size_t from);

    /**
     * Copy constructor
     * \param other Other iterator
     */
    VertexIterator(const VertexIterator& other);

    /** \} */

    /**
     * \brief Move iterator to next element
     * \return this instance
     */
    VertexIterator& operator++();

    /**
     * \brief Advance the iterator by specified element positions.
     * \return copy of the iterator before the operation
     */
    const VertexIterator operator++(int);

    /** \brief compare 2 vertex iterators. */
    bool operator==(const VertexIterator& other) const;

    /** \brief compare 2 vertex iterators. */
    bool operator!=(const VertexIterator& other) const;

    /**
     * Get vertex at the current iterator position
     * \return constant reference to element
     */
    const value_type& operator*();

    using VertexIteratorImpl_ptr = std::shared_ptr<VertexIteratorImpl>;

  private:
    VertexIteratorImpl_ptr pimpl_;
};

extern template VertexIterator::VertexIterator(
    const basalt::GraphImpl<EdgeOrientation::undirected>& pimpl,
    size_t from);
extern template VertexIterator::VertexIterator(
    const basalt::GraphImpl<EdgeOrientation::directed>& pimpl,
    size_t from);

}  // namespace basalt
