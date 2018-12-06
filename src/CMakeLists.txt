set(basalt_SOURCES
    basalt/connections.cpp
    basalt/graph.hpp
    basalt/network.cpp
    basalt/network_impl.cpp
    basalt/network_impl.hpp
    basalt/nodes.cpp
    basalt/node_iterator_impl.hpp
    basalt/node_iterator_impl.cpp
    basalt/node_iterator.cpp
    basalt/status.cpp)
set(basalt_HEADERS
    ${basalt_include_directory}/basalt/connections.hpp
    ${basalt_include_directory}/basalt/fwd.hpp
    ${basalt_include_directory}/basalt/network.hpp
    ${basalt_include_directory}/basalt/nodes.hxx
    ${basalt_include_directory}/basalt/node_iterator.hpp
    ${basalt_include_directory}/basalt/nodes.hpp)
include_directories(${spdlog_include_directory}
                    ${CMAKE_CURRENT_SOURCE_DIR}/basalt)

set(PYBIND11_SOURCES
    basalt/circuit_payloads.hpp
    basalt/circuit_payloads.cpp
    basalt/py_basalt.cpp
    basalt/py_helpers.hpp)
foreach(pybind_cpp basalt/py_basalt.cpp basalt/circuit_payloads.cpp)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_source_files_properties(
      ${pybind_cpp}
      PROPERTIES
      COMPILE_FLAGS
      "-Wno-undef -Wno-extra-semi -Wno-duplicate-enum -Wno-undefined-reinterpret-cast -Wno-missing-prototypes -Wno-reserved-id-macro -Wno-old-style-cast -Wno-deprecated -Wno-shadow-field -Wno-covered-switch-default -Wno-range-loop-analysis -Wno-zero-as-null-pointer-constant"
      )
  else()
    set_source_files_properties(
      ${pybind_cpp}
      PROPERTIES
      COMPILE_FLAGS
      "-Wno-old-style-cast -Wno-deprecated -Wno-zero-as-null-pointer-constant")
  endif()
endforeach(pybind_cpp)
set(PYBIND11_CPP_STANDARD -std=c++11)

message(STATUS "PYTHON_PACKAGEBUILD: ${Basalt_BUILD_PYTHON_PACKAGE}")
if(Basalt_BUILD_PYTHON_PACKAGE)
  add_subdirectory(third_party/pybind11)
  pybind11_add_module(_basalt
                      ${PYBIND11_SOURCES}
                      ${basalt_SOURCES}
                      ${basalt_HEADERS})
  target_link_libraries(_basalt PRIVATE ${RocksDB_LIBRARIES})
else()
  add_library(basalt ${basalt_SOURCES} ${basalt_HEADERS})
  bob_library_includes(basalt)
  bob_export_target(basalt)
  target_link_libraries(basalt ${RocksDB_LIBRARIES})
  install(FILES ${basalt_HEADERS} DESTINATION include)

  if(Basalt_USE_pybind11)
    add_subdirectory(third_party/pybind11)
    pybind11_add_module(pybasalt
                        ${PYBIND11_SOURCES}
                        ${basalt_SOURCES}
                        ${basalt_HEADERS})
    target_link_libraries(pybasalt PRIVATE ${RocksDB_LIBRARIES})
  endif()
endif()
