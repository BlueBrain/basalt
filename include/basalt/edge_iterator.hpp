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

/// \brief Iterator class for edges
class EdgeIterator: public std::iterator<std::input_iterator_tag, const edge_uid_t> {
  public:
    /**
     * \brief Ctors and dtors.
     * \{
     */

    /**
     * Create an iterator over edges
     * \param pimpl  Pointer to implementation
     * \param from Move iterator a specified index
     */
    template <EdgeOrientation Orientation>
    EdgeIterator(const GraphImpl<Orientation>& pimpl, size_t from);

    /**
     * Copy constructor
     * \param other Other iterator
     */
    EdgeIterator(const EdgeIterator& other);

    /** \} */

    /**
     * \brief Move iterator to next element
     * \return this instance
     */
    EdgeIterator& operator++();

    /**
     * \brief Advance the iterator by specified element positions.
     * \return copy of the iterator before the operation
     */
    const EdgeIterator operator++(int);

    /** \brief compare 2 edge iterators */
    bool operator==(const EdgeIterator& other) const;

    /** \brief compare 2 edge iterators */
    bool operator!=(const EdgeIterator& other) const;

    /**
     * Get edge at the current iterator position
     * \return constant reference to element
     */
    const value_type& operator*();

    using EdgeIteratorImpl_ptr = std::shared_ptr<EdgeIteratorImpl>;

  private:
    EdgeIteratorImpl_ptr pimpl_;
};

extern template EdgeIterator::EdgeIterator(
    const basalt::GraphImpl<EdgeOrientation::directed>& pimpl,
    size_t from);
extern template EdgeIterator::EdgeIterator(
    const basalt::GraphImpl<EdgeOrientation::undirected>& pimpl,
    size_t from);

}  // namespace basalt
