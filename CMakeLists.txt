cmake_minimum_required(VERSION 3.5.1)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
project(Basalt VERSION 0.0.1 LANGUAGES CXX)

include(cmake/bob.cmake)

bob_begin_package()

bob_option(Basalt_USE_pybind11 "Build Python bindings" OFF)
bob_option(Basalt_BUILD_PYTHON_PACKAGE "Build Python package" OFF)

set(Basalt_SERIALIZATION_METHODS CEREAL SSTREAM)
bob_input(Basalt_SERIALIZATION
          CEREAL
          STRING
          "Serialization method: ${Basalt_SERIALIZATION_METHODS}")
set_property(CACHE Basalt_SERIALIZATION
             PROPERTY STRINGS "${Basalt_SERIALIZATION_METHODS}")
list(FIND Basalt_SERIALIZATION_METHODS ${Basalt_SERIALIZATION} index)
if(index EQUAL -1)
  message(
    FATAL_ERROR
      "Unknown serialization '${Basalt_SERIALIZATION}'. Expected one of : ${Basalt_SERIALIZATION_METHODS}"
    )
endif()
unset(Basalt_SERIALIZATION_METHODS)
unset(index)

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

set(Basalt_KEY_INTS
    Basalt_VERSION_MAJOR
    Basalt_VERSION_MINOR
    Basalt_VERSION_PATCH)

set(Basalt_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(Basalt_KEY_STRINGS
    Basalt_SEMVER
    Basalt_COMMIT
    Basalt_CXX_FLAGS
    Basalt_CMAKE_ARGS)

bob_get_commit()
bob_get_semver()

find_package(RocksDB 4.1.0 REQUIRED)
include_directories(${RocksDB_INCLUDE_DIR})

set(basalt_include_directory ${PROJECT_SOURCE_DIR}/include)
include_directories(${basalt_include_directory})

set(catch2_include_directory
    ${PROJECT_SOURCE_DIR}/src/third_party/catch2/include)
set(cereal_include_directory
    ${PROJECT_SOURCE_DIR}/src/third_party/cereal/include)
set(pybind11_project_directory ${PROJECT_SOURCE_DIR}/src/third_party/pybind11)
set(pybind11_include_directory ${pybind11_project_directory}/include)
set(spdlog_include_directory
    ${PROJECT_SOURCE_DIR}/src/third_party/spdlog/include)

add_subdirectory(src)
if(NOT Basalt_BUILD_PYTHON_PACKAGE)
  add_subdirectory(tests/unit)
endif()

bob_end_package()
