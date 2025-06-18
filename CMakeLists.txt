cmake_minimum_required(VERSION 3.31)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
set(CMAKE_CXX_MODULE_STD 1)
project(lib2 LANGUAGES CXX)
if (MSVC)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

add_library(lib2 STATIC)

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(benchmarks)

# target_include_directories(lib2 PUBLIC
# $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
# $<INSTALL_INTERFACE:include>
# )

# configure_package_config_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/lib2Config.cmake.in
#     ${CMAKE_CURRENT_BINARY_DIR}/lib2Config.cmake
#     INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
# )

# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib2Config.cmake DESTINATION ${CONFIG_INSTALL_DIR})