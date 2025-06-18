target_sources(lib2 PUBLIC
  FILE_SET lib2_platform_modules TYPE CXX_MODULES
  FILES
    architecture.ixx
    operating_system.ixx
    compiler.ixx
    cpp_version.ixx
    platform.ixx
)