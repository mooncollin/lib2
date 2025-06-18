add_executable(fmt_character_write char_write.cpp)
target_link_libraries(fmt_character_write PRIVATE lib2)

add_executable(fmt_int_base10 int_base10.cpp)
target_link_libraries(fmt_int_base10 PRIVATE lib2)

add_executable(fmt_int_base2 int_base2.cpp)
target_link_libraries(fmt_int_base2 PRIVATE lib2)