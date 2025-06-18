add_executable(lib2_tests lib2_tests.cpp)

target_link_libraries(lib2_tests
    PRIVATE
        lib2
)

add_subdirectory(type_traits)
add_subdirectory(strings)
add_subdirectory(io)
add_subdirectory(fmt)
# add_subdirectory(match)

target_sources(lib2_tests PUBLIC
  FILE_SET lib2_tests_compact_optional_modules TYPE CXX_MODULES FILES
    compact_optional.ixx
)

set(GENERATED_TESTS_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated_tests.cmake")

add_custom_command(
  TARGET lib2_tests POST_BUILD
  COMMAND lib2_tests -l > tests.txt
  COMMAND ${CMAKE_COMMAND} -DINPUT_FILE=tests.txt -DOUTPUT_FILE=${GENERATED_TESTS_FILE}
          -DTEST_EXECUTABLE=$<TARGET_FILE:lib2_tests> -P ${CMAKE_CURRENT_SOURCE_DIR}/generate_tests.cmake
  COMMENT "Discovering tests from lib2_tests"
)
add_custom_target(discover_my_tests DEPENDS lib2_tests)
include(${GENERATED_TESTS_FILE} OPTIONAL)