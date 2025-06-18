add_subdirectory(platform)
add_subdirectory(type_traits)
add_subdirectory(strings)
add_subdirectory(io)
add_subdirectory(fmt)
add_subdirectory(test)
add_subdirectory(benchmarking)
# add_subdirectory(match)
# add_subdirectory(scan)

target_sources(lib2 PUBLIC
  FILE_SET lib2_modules TYPE CXX_MODULES FILES
    concepts.ixx
    utility.ixx
    indirect_iterator.ixx
    compact_optional.ixx
    lib2.ixx
)

install(TARGETS lib2
  EXPORT lib2_modules
  FILE_SET lib2_modules              DESTINATION include
  FILE_SET lib2_platform_modules     DESTINATION include/platform
  FILE_SET lib2_type_traits_modules  DESTINATION include/type_traits
  FILE_SET lib2_strings_modules      DESTINATION include/strings
  FILE_SET lib2_strings_headers      DESTINATION include/strings
  FILE_SET lib2_io_modules           DESTINATION include/io
  FILE_SET lib2_fmt_modules          DESTINATION include/fmt
  FILE_SET lib2_fmt_headers          DESTINATION include/fmt
  FILE_SET lib2_test_modules         DESTINATION include/test
  FILE_SET lib2_benchmarking_modules DESTINATION include/benchmarking
  FILE_SET lib2_match_modules        DESTINATION include/match
)