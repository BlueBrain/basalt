cmake_minimum_required(VERSION 3.5.1)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# get version from Python package
find_package(PythonInterp 3.5 REQUIRED)
execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py --version
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE PY_VERSION_FAILED
                OUTPUT_VARIABLE PY_PKG_VERSION_FULL
                ERROR_VARIABLE PY_VERSION_ERROR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if(PY_VERSION_FAILED)
  message(
    FATAL_ERROR
      "Could not retrieve version from command '${PYTHON_EXECUTABLE} setup.py --version'\n"
      ${PY_VERSION_ERROR})
endif()
# keep last line of command output
string(REPLACE "\n"
               ";"
               PY_PKG_VERSION_FULL
               "${PY_PKG_VERSION_FULL}")
list(GET PY_PKG_VERSION_FULL -1 PY_PKG_VERSION_FULL)
# keep MAJOR.MINOR.PATCH
string(REGEX
       REPLACE "([0-9]+\\.[0-9]+\\.[0-9]+).*"
               "\\1"
               PY_PKG_VERSION
               "${PY_PKG_VERSION_FULL}")

project(Basalt VERSION ${PY_PKG_VERSION} LANGUAGES CXX)

if(NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake/hpc-coding-conventions)
  message(FATAL_ERROR "It seems the git modules are missing.\n"
                      "You can retrieve them by running the following command:\n"
                      "    git submodule update --recursive --init")
endif()
add_subdirectory(cmake/hpc-coding-conventions/cpp)

include(cmake/DartConfig.cmake)

bob_begin_package()

bob_option(Basalt_USE_pybind11 "Build Python bindings" OFF)
bob_option(USE_CLANG_TIDY "Perform C++ static analysis while compiling" OFF)

set(Basalt_SERIALIZATION_METHODS CEREAL SSTREAM)
bob_input(Basalt_SERIALIZATION
          CEREAL
          STRING
          "Serialization method: ${Basalt_SERIALIZATION_METHODS}")
set_property(CACHE Basalt_SERIALIZATION PROPERTY STRINGS "${Basalt_SERIALIZATION_METHODS}")
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
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(STATUS "Detected Clang compiler")
  set(FLAGS "${FLAGS} -Wlifetime")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  message(STATUS "Detected GNU compiler")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  message(STATUS "Detected Intel compiler")
endif()
bob_cxx11_flags()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-float-equal")
endif()
bob_end_cxx_flags()

set(Basalt_KEY_INTS Basalt_VERSION_MAJOR Basalt_VERSION_MINOR Basalt_VERSION_PATCH)

set(Basalt_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(Basalt_KEY_STRINGS Basalt_SEMVER Basalt_COMMIT Basalt_CXX_FLAGS Basalt_CMAKE_ARGS)

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/.git)
  bob_get_commit()
  bob_get_semver()
endif()

find_package(RocksDB 4.1.0 REQUIRED)

find_package(GoogleBenchmark)
if(GoogleBenchmark_FOUND)
  include_directories(SYSTEM ${GoogleBenchmark_INCLUDE_DIR})
endif()

find_package(Boost)
if(Boost_FOUND)
  include_directories(SYSTEM ${Boost_INCLUDE_DIR})
endif()

set(basalt_include_directory ${PROJECT_SOURCE_DIR}/include)
include_directories(${basalt_include_directory})

set(catch2_include_directory ${PROJECT_SOURCE_DIR}/src/third_party/catch2/include)
set(cereal_include_directory ${PROJECT_SOURCE_DIR}/src/third_party/cereal/include)
set(gsl_include_directory ${PROJECT_SOURCE_DIR}/src/third_party/gsl-lite/include)
set(pybind11_project_directory ${PROJECT_SOURCE_DIR}/src/third_party/pybind11)
set(pybind11_include_directory ${pybind11_project_directory}/include)
set(spdlog_include_directory ${PROJECT_SOURCE_DIR}/src/third_party/spdlog/include)
set(nlohmann_include_directory ${PROJECT_SOURCE_DIR}/src/third_party/nlohmann/include)

add_subdirectory(src)
if(NOT Basalt_BUILD_PYTHON_PACKAGE)
  add_subdirectory(tests/unit)
endif()
add_subdirectory(tests/benchmarks)

add_custom_target(build_python_ext DEPENDS _basalt unit-tests)

bob_end_package()
