add_executable(map_benchmark map_benchmark.cpp)
target_link_libraries(map_benchmark PRIVATE -lpthread ${GoogleBenchmark_LIBRARY})
