include_directories(${catch2_include_directory})

add_executable(101 101.cpp)
target_link_libraries(101 PRIVATE basalt)
add_test(NAME 101 COMMAND $<TARGET_FILE:101>)
