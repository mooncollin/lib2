target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
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
    istream.cpp
)
endif()