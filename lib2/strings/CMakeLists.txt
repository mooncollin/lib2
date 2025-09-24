target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
    utility.ixx
    character.ixx
    algorithm.ixx
    string_literal.ixx
    lazy_string.ixx
    strings.ixx
FILE_SET HEADERS FILES
    widen.h
)