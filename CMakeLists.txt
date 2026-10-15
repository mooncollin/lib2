cmake_minimum_required(VERSION 4.1.0)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
set(CMAKE_CXX_MODULE_STD 1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(lib2 LANGUAGES CXX)
if (MSVC)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
    add_compile_options(/GL)
    add_link_options(/LTCG)
endif()

add_subdirectory(lib2)
add_subdirectory(benchmarks)
add_subdirectory(tests)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/lib2Config.cmake.in"
    "${CMAKE_BINARY_DIR}/lib2Config.cmake"
    INSTALL_DESTINATION cmake
)