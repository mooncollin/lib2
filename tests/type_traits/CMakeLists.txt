target_sources(lib2_tests
PUBLIC
FILE_SET CXX_MODULES FILES
    type_traits.ixx
PRIVATE
    type_traits.cpp
)