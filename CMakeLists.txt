cmake_minimum_required(VERSION 3.9.6)
project(basalt VERSION 0.1 LANGUAGES CXX)

include(cmake/bob.cmake)

bob_begin_package()

set(basalt_USE_pybind11_DEFAULT OFF)
bob_public_dep(pybind11)

bob_begin_cxx_flags()
bob_cxx11_flags()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(STATUS "Detected Clang compiler")
  set(FLAGS "${FLAGS} -Wlifetime")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  message(STATUS "Detected GNU compiler")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  message(STATUS "Detected Intel compiler")
endif()

bob_begin_cxx_flags()
bob_cxx11_flags()
bob_end_cxx_flags()

set(basalt_KEY_INTS
    basalt_VERSION_MAJOR
    basalt_VERSION_MINOR
    basalt_VERSION_PATCH)

set(basalt_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(basalt_KEY_STRINGS
    basalt_SEMVER
    basalt_COMMIT
    basalt_CXX_FLAGS
    basalt_CMAKE_ARGS)

bob_get_commit()
bob_get_semver()

set(basalt_include_directory ${PROJECT_SOURCE_DIR}/include)
include_directories(${basalt_include_directory})

set(catch2_include_directory ${PROJECT_SOURCE_DIR}/src/third_party/catch2/include)
set(spdlog_include_directory ${PROJECT_SOURCE_DIR}/src/third_party/spdlog/include)

add_subdirectory(src)
add_subdirectory(test/unit)

bob_end_package()
