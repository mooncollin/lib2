target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
    chrono.ixx
    formatter.ixx
    context.ixx
    parsers.ixx
    format.ixx
    string.ixx
    arithmetic.ixx
    misc.ixx
    ranges.ixx
    fmt.ixx
PRIVATE
    string.cpp
    format.cpp
)
