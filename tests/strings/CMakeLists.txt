target_sources(lib2_tests
PUBLIC
FILE_SET CXX_MODULES FILES
    strings.ixx
    character.ixx
    lazy_string.ixx
PRIVATE
    strings.cpp
)