include_directories(SYSTEM ${catch2_include_directory}
        ${cereal_include_directory} ${GoogleBenchmark_INCLUDE_DIR} ${GoogleBenchmark_LIB_DIR})

add_executable(benchmark Benchmark.cpp)
add_executable(gbench_memcpy memcpy.cpp)
add_executable(gbench_basalt basalt_g_bench.cpp)

target_link_libraries(benchmark PRIVATE _basalt ${PYTHON_LIBRARIES})
#add_library(gbench STATIC ${GoogleBenchmark_LIB_DIR})
target_link_libraries(gbench_memcpy ${GoogleBenchmark_LIBRARY} Threads::Threads _basalt ${PYTHON_LIBRARIES})
target_link_libraries(gbench_basalt ${GoogleBenchmark_LIBRARY} Threads::Threads _basalt ${PYTHON_LIBRARIES})


add_test(NAME benchmark COMMAND $<TARGET_FILE:benchmark>)
add_test(NAME gbench_memcpy COMMAND $<TARGET_FILE:gbench_memcpy>)
add_test(NAME gbench_basalt COMMAND $<TARGET_FILE:gbench_memcpy>)