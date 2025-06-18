target_sources(lib2_tests PUBLIC
  FILE_SET lib2_tests_strings_modules TYPE CXX_MODULES FILES
    widen.ixx
    lazy_string.ixx
    strings.ixx
)