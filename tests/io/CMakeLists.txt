file(COPY read_test_empty.txt DESTINATION ${CMAKE_BINARY_DIR}/tests)
file(COPY read_test.txt       DESTINATION ${CMAKE_BINARY_DIR}/tests)
file(COPY write_test.txt      DESTINATION ${CMAKE_BINARY_DIR}/tests)

target_sources(lib2_tests PUBLIC
    FILE_SET lib2_tests_io_modules TYPE CXX_MODULES FILES
        file.ixx
        ostream.ixx
        iostream.ixx
        stringstream.ixx
        fstream.ixx
        io.ixx
)