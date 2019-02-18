# .rst: FindRocksDB
# -----------
#
# Find RocksDB
#
# Find RocksDB headers and find_library
#
# ::
#
# * RocksDB_FOUND           - True if RocksDB library is found.
# * RocksDB_INCLUDE_DIR     - Directory where RocksDB headers are located.
# * RocksDB_LIBRARIES       - RocksDB libraries to link against.
# * RocksDB_VERSION_MAJOR   - The major version of RocksDB.
# * RocksDB_VERSION_MINOR   - The minor version of RocksDB.
# * RocksDB_VERSION_PATCH   - The patch version of RocksDB.
# * RocksDB_VERSION_STRING  - version number as a string (ex: "4.3.2").

find_path(RocksDB_INCLUDE_DIR NAMES rocksdb/db.h)
find_library(RocksDB_LIBRARIES NAMES rocksdb)

if(RocksDB_INCLUDE_DIR AND EXISTS ${RocksDB_INCLUDE_DIR}/rocksdb/version.h)
  file(STRINGS ${RocksDB_INCLUDE_DIR}/rocksdb/version.h rocksdb_version_str
       REGEX "^#define ROCKSDB_MAJOR")
  string(REGEX
         REPLACE "^.*ROCKSDB_MAJOR (.*)$"
                 "\\1"
                 RocksDB_VERSION_MAJOR
                 "${rocksdb_version_str}")

  file(STRINGS ${RocksDB_INCLUDE_DIR}/rocksdb/version.h rocksdb_version_str
       REGEX "^#define ROCKSDB_MINOR")
  string(REGEX
         REPLACE "^.*ROCKSDB_MINOR (.*)$"
                 "\\1"
                 RocksDB_VERSION_MINOR
                 "${rocksdb_version_str}")

  file(STRINGS ${RocksDB_INCLUDE_DIR}/rocksdb/version.h rocksdb_version_str
       REGEX "^#define ROCKSDB_PATCH")
  string(REGEX
         REPLACE "^.*ROCKSDB_PATCH (.*)$"
                 "\\1"
                 RocksDB_VERSION_PATCH
                 "${rocksdb_version_str}")

  set(ROCKSDB_VERSION_STRING
      "${RocksDB_VERSION_MAJOR}.${RocksDB_VERSION_MINOR}.${RocksDB_VERSION_PATCH}")
  unset(rocksdb_version_str)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RocksDB
                                  FOUND_VAR
                                  RocksDB_FOUND
                                  REQUIRED_VARS
                                  RocksDB_LIBRARIES
                                  RocksDB_INCLUDE_DIR
                                  VERSION_VAR
                                  ROCKSDB_VERSION_STRING)

mark_as_advanced(RocksDB_INCLUDE_DIR RocksDB_LIBRARIES)
