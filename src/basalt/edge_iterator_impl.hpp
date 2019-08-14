/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

#include <type_traits>

#include <basalt/edge_iterator.hpp>

#include "fwd.hpp"

namespace basalt {

class EdgeIteratorImpl {
  public:
    using value_type = EdgeIterator::value_type;
    explicit EdgeIteratorImpl(const db_t& db,
                              rocksdb::ColumnFamilyHandle* edges,
                              const std::string& prefix,
                              std::size_t position);

    inline std::size_t position_get() const {
        return position_;
    }

    EdgeIteratorImpl& operator++();
    bool operator==(const EdgeIteratorImpl& other) const;
    bool operator!=(const EdgeIteratorImpl& other) const;
    const value_type& operator*();

    bool end_reached() const;

  private:
    rocksdb::Iterator* iter_;
    std::size_t position_;
    std::remove_const<value_type>::type value;
};

}  // namespace basalt
