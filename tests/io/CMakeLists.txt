file(COPY read_test_empty.txt DESTINATION ${CMAKE_BINARY_DIR}/tests)
file(COPY read_test.txt       DESTINATION ${CMAKE_BINARY_DIR}/tests)
file(COPY write_test.txt      DESTINATION ${CMAKE_BINARY_DIR}/tests)

target_sources(lib2_tests
PUBLIC
FILE_SET CXX_MODULES FILES
    ostream.ixx
    stringstream.ixx
    fstream.ixx
    io.ixx
)