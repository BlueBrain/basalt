/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#include <rocksdb/version.h>

#include "basalt/version.hpp"

namespace basalt {

std::array<int, 3> rocksdb_version() {
    return {{ROCKSDB_MAJOR, ROCKSDB_MINOR, ROCKSDB_PATCH}};
}

}  // namespace basalt
