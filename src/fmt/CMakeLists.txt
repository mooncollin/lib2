target_sources(lib2 PUBLIC
  FILE_SET lib2_fmt_modules TYPE CXX_MODULES FILES
    unicode.ixx
    format.ixx
    chrono.ixx
    fmt.ixx
)

target_sources(lib2 PRIVATE
  FILE_SET lib2_fmt_headers TYPE HEADERS FILES
    parsers.ixx
    string.ixx
    arithmetic.ixx
    misc.ixx
)

target_sources(lib2 PRIVATE
  arithmetic.cpp
)