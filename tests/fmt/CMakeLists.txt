target_sources(lib2_tests
PUBLIC
FILE_SET CXX_MODULES FILES
    character.ixx
    integral.ixx
    floating.ixx
    string.ixx
    chrono.ixx
    fmt.ixx
PRIVATE
    fmt.cpp
)