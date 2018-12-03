#ifndef BASALT_IMPL_FWD_HPP
#define BASALT_IMPL_FWD_HPP

#include <memory>

namespace rocksdb {

/// forward declaration
class ColumnFamilyHandle;
class DB;
class Iterator;
class Status;
class WriteBatch;
} // namespace rocksdb

namespace spdlog {

/// forward declaration
class logger;

} // namespace spdlog

namespace basalt {

using db_t = std::unique_ptr<rocksdb::DB>;

}

#endif // BASALT_IMPL_FWD_HPP
