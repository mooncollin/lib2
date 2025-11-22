target_sources(lib2_tests
PUBLIC
FILE_SET CXX_MODULES FILES
    meta.ixx
    value_list.ixx
PRIVATE
    meta.cpp
)