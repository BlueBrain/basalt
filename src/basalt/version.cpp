#include <rocksdb/version.h>

#include "basalt/version.hpp"

namespace basalt {

std::array<int, 3> rocksdb_version() {
    return {ROCKSDB_MAJOR, ROCKSDB_MINOR, ROCKSDB_PATCH};
}

} // namespace basalt
