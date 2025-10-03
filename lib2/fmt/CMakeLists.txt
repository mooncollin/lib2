target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
    unicode.ixx
    format.ixx
    chrono.ixx
    fmt.ixx
FILE_SET HEADERS FILES
    parsers.ixx
    string.ixx
    arithmetic.ixx
    misc.ixx
    ranges.ixx
PRIVATE
    arithmetic.cpp
)
