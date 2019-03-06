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
