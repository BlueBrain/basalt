set(basalt_SOURCES
    basalt/network.cpp
    basalt/network_impl.cpp
    basalt/network_impl.hpp
    basalt/nodes.cpp
    basalt/node_iterator_impl.hpp
    basalt/node_iterator_impl.cpp
    basalt/node_iterator.cpp
    basalt/connections.cpp
    basalt/status.cpp
    basalt/graph.hpp
)
set(basalt_HEADERS
    ${basalt_include_directory}/basalt/connections.hpp
    ${basalt_include_directory}/basalt/fwd.hpp
    ${basalt_include_directory}/basalt/network.hpp
    ${basalt_include_directory}/basalt/nodes.hxx
    ${basalt_include_directory}/basalt/node_iterator.hpp
    ${basalt_include_directory}/basalt/nodes.hpp)
include_directories(${spdlog_include_directory}
                    ${CMAKE_CURRENT_SOURCE_DIR}/basalt)
add_library(basalt ${basalt_SOURCES} ${basalt_HEADERS})
target_link_libraries(basalt ${RocksDB_LIBRARIES})
