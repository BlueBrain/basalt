# pure C++ shared library
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

include_directories(SYSTEM ${cereal_include_directory}
                    ${spdlog_include_directory})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/basalt
                    ${CMAKE_CURRENT_BINARY_DIR})

add_library(basalt_obj OBJECT ${basalt_SOURCES} ${basalt_HEADERS})
set_property(TARGET basalt_obj PROPERTY POSITION_INDEPENDENT_CODE ON)

# Shared library
add_library(basalt SHARED $<TARGET_OBJECTS:basalt_obj>)
target_link_libraries(basalt ${RocksDB_LIBRARIES})
bob_library_includes(basalt)
bob_export_target(basalt)
install(FILES ${basalt_HEADERS} DESTINATION include)

# Shared library with Python bindings
set(PYBIND11_SOURCES
    basalt/circuit_payloads.hpp
    basalt/circuit_payloads.cpp
    basalt/py_basalt.cpp
    basalt/py_helpers.hpp)
set(PYBIND11_CPP_STANDARD -std=c++11)
add_subdirectory(${pybind11_project_directory})
pybind11_add_module(_basalt
                    SHARED
                    ${PYBIND11_SOURCES}
                    $<TARGET_OBJECTS:basalt_obj>)
target_link_libraries(_basalt PRIVATE ${RocksDB_LIBRARIES})
