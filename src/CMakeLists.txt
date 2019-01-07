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
    basalt/status.cpp
    basalt/settings.hpp
    basalt/version.cpp)
set(basalt_HEADERS
    ${basalt_include_directory}/basalt/connections.hpp
    ${basalt_include_directory}/basalt/fwd.hpp
    ${basalt_include_directory}/basalt/network.hpp
    ${basalt_include_directory}/basalt/nodes.hxx
    ${basalt_include_directory}/basalt/node_iterator.hpp
    ${basalt_include_directory}/basalt/nodes.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/basalt/version.hpp)

configure_file(basalt/version.hpp.in basalt/version.hpp @ONLY)
configure_file(basalt/settings.hpp.in basalt/settings.hpp @ONLY)

include_directories(${cereal_include_directory}
                    ${spdlog_include_directory}
                    ${CMAKE_CURRENT_SOURCE_DIR}/basalt
                    ${CMAKE_CURRENT_BINARY_DIR})

set(PYBIND11_SOURCES
    basalt/circuit_payloads.hpp
    basalt/circuit_payloads.cpp
    basalt/py_basalt.cpp
    basalt/py_helpers.hpp)
set(PYBIND11_CPP_STANDARD -std=c++11)

if(Basalt_BUILD_PYTHON_PACKAGE)
  add_subdirectory(${pybind11_project_directory})
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
