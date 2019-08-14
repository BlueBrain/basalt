/*************************************************************************
 * Copyright (C) 2019 Blue Brain Project
 *
 * This file is part of Basalt distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/
#pragma once

#include <memory>

namespace rocksdb {

/// forward declaration
class ColumnFamilyHandle;
class DB;
class Iterator;
struct Options;
class Statistics;
class Status;
class WriteBatch;
}  // namespace rocksdb

namespace spdlog {

/// forward declaration
class logger;

}  // namespace spdlog

namespace basalt {

using db_t = std::unique_ptr<rocksdb::DB>;
}
