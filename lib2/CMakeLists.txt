add_library(lib2 STATIC)
target_compile_features(lib2 PUBLIC cxx_std_23)

target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
    concepts.ixx
    utility.ixx
    indirect_iterator.ixx
    compact_optional.ixx
    locatable_object.ixx
    lib2.ixx
FILE_SET HEADERS FILES
)
target_include_directories(lib2 PUBLIC ${CMAKE_SOURCE_DIR})

add_subdirectory(platform)
add_subdirectory(type_traits)
add_subdirectory(memory)
add_subdirectory(strings)
add_subdirectory(io)
add_subdirectory(fmt)
add_subdirectory(test)
add_subdirectory(benchmarking)
# add_subdirectory(match)
# add_subdirectory(scan)

export(TARGETS lib2 NAMESPACE lib2:: FILE "${CMAKE_BINARY_DIR}/lib2Targets.cmake")