set(basalt_SOURCES basalt/network.cc)
set(basalt_HEADERS ${basalt_include_directory}/basalt/network.hh)
include_directories(${spdlog_include_directory})
add_library(basalt ${basalt_SOURCES} ${basalt_HEADERS})
