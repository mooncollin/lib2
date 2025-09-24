add_executable(io_simple_write simple_write.cpp)
target_link_libraries(io_simple_write PRIVATE lib2)

add_executable(io_simple_string_parse simple_string_parse.cpp)
target_link_libraries(io_simple_string_parse PRIVATE lib2)

add_executable(io_string_write string_write.cpp)
target_link_libraries(io_string_write PRIVATE lib2)

add_executable(fstream_write fstream_write.cpp)
target_link_libraries(fstream_write PRIVATE lib2)

add_executable(fstream_multiple_write fstream_multiple_write.cpp)
target_link_libraries(fstream_multiple_write PRIVATE lib2)