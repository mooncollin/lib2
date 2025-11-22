add_library(lib2 STATIC)
target_compile_features(lib2 PUBLIC cxx_std_23)

target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
    concepts.ixx
    utility.ixx
    compact_optional.ixx
    shared.ixx
    lib2.ixx
FILE_SET HEADERS FILES
PRIVATE
    shared_win32.cpp
)
target_include_directories(lib2 PUBLIC ${CMAKE_SOURCE_DIR})

add_subdirectory(platform)
add_subdirectory(type_traits)
add_subdirectory(meta)
add_subdirectory(iterator)
add_subdirectory(memory)
add_subdirectory(strings)
add_subdirectory(err)
add_subdirectory(io)
add_subdirectory(fmt)
add_subdirectory(test)
add_subdirectory(benchmarking)
# add_subdirectory(match)
# add_subdirectory(scan)

export(TARGETS lib2 NAMESPACE lib2:: FILE "${CMAKE_BINARY_DIR}/lib2Targets.cmake")