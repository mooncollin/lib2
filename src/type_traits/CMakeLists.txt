target_sources(lib2 PUBLIC
  FILE_SET lib2_type_traits_modules TYPE CXX_MODULES FILES
    copy_traits.ixx
    detector.ixx
    function_traits.ixx
    type_traits.ixx
)