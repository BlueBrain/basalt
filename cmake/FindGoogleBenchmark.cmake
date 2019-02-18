# .rst: FindGoogleBenchmark
# -----------
#
# Find GoogleBenchmark
#
# Find GoogleBenchmark headers and find_library
#
# ::
#
# * GoogleBenchmark_FOUND           - True if GoogleBenchmark library is found.
# * GoogleBenchmark_INCLUDE_DIR     - Directory where GoogleBenchmark headers are located.
# * GoogleBenchmark_LIBRARY         - GoogleBenchmark libraries to link against.
# * GoogleBenchmark_LIBRARY_DIR     - Directory where GoogleBenchmark libraries are located.
# * GoogleBenchmark_VERSION         - The version of Google Benchmark.
# * GoogleBenchmark_VERSION_STRING  - The version number as a string (ex: "1.4.0").

find_path(GoogleBenchmark_INCLUDE_DIR NAMES benchmark/benchmark.h)
find_library(GoogleBenchmark_LIBRARY NAMES benchmark)

if(GoogleBenchmark_LIBRARY)
  get_filename_component(GoogleBenchmark_LIB_DIR ${GoogleBenchmark_LIBRARY} DIRECTORY)
  if(EXISTS ${GoogleBenchmark_LIB_DIR}/cmake/benchmark/benchmarkConfigVersion.cmake)
    include(${GoogleBenchmark_LIB_DIR}/cmake/benchmark/benchmarkConfigVersion.cmake)
    set(GoogleBenchmark_VERSION ${PACKAGE_VERSION})
    set(GoogleBenchmark_VERSION_STRING "${GoogleBenchmark_VERSION}")
  endif()
endif()

# Checks 'REQUIRED', 'QUIET' and versions.
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GoogleBenchmark
                                  FOUND_VAR
                                  GoogleBenchmark_FOUND
                                  REQUIRED_VARS
                                  GoogleBenchmark_INCLUDE_DIR
                                  GoogleBenchmark_LIBRARY
                                  GoogleBenchmark_LIB_DIR
                                  VERSION_VAR
                                  GoogleBenchmark_VERSION_STRING)

mark_as_advanced(GoogleBenchmark_INCLUDE_DIR GoogleBenchmark_LIBRARY GoogleBenchmark_LIB_DIR)
