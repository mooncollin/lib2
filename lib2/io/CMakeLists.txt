target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
    character.ixx
    stream_traits.ixx
    ostream.ixx
    istream.ixx
    iostream.ixx
    stringstream.ixx
    spanstream.ixx
    fstream.ixx
    io.ixx
)

if (WIN32)
target_sources(lib2
PRIVATE
    fstream_win32.cpp
)
endif()