cmake_minimum_required(VERSION 4.1.0)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD 1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(lib2 LANGUAGES CXX)
if (MSVC)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

enable_testing()
add_subdirectory(lib2)
add_subdirectory(tests)
add_subdirectory(benchmarks)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/lib2Config.cmake.in"
    "${CMAKE_BINARY_DIR}/lib2Config.cmake"
    INSTALL_DESTINATION cmake
)