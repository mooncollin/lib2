# target_sources(lib2
# PUBLIC
# FILE_SET CXX_MODULES FILES
#     ostream.ixx
#     istream.ixx
#     iostream.ixx
#     stringstream.ixx
#     spanstream.ixx
#     fstream.ixx
#     io.ixx
# )

target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
    sink.ixx
    source.ixx
    iterator.ixx
    buffered.ixx
    ref.ixx
    string.ixx
    span.ixx
    file.ixx
    algorithm.ixx
    io.ixx
)

if(LINUX)
target_sources(lib2
PRIVATE
    file_linux.cpp
)
endif()

# if (WIN32)
# target_sources(lib2
# PRIVATE
#     fstream_win32.cpp
#     istream.cpp
# )
# endif()