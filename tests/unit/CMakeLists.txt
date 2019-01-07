include_directories(${catch2_include_directory} ${cereal_include_directory})

add_executable(unit-tests 101.cpp)
target_link_libraries(unit-tests PRIVATE basalt)

add_test(NAME unit-tests COMMAND $<TARGET_FILE:unit-tests>)
