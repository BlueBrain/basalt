/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

#include <type_traits>

#include <basalt/vertex_iterator.hpp>

#include "fwd.hpp"

namespace basalt {

class VertexIteratorImpl {
  public:
    using value_type = VertexIterator::value_type;
    explicit VertexIteratorImpl(const db_t& db,
                                rocksdb::ColumnFamilyHandle* vertices,
                                const std::string& prefix,
                                std::size_t position);

    inline std::size_t position_get() const {
        return position_;
    }

    VertexIteratorImpl& operator++();
    bool operator==(const VertexIteratorImpl& rhs) const;
    bool operator!=(const VertexIteratorImpl& rhs) const;
    const value_type& operator*();

    bool end_reached() const;

  private:
    rocksdb::Iterator* iter_;
    std::size_t position_;
    std::remove_const<value_type>::type value;
};

}  // namespace basalt
