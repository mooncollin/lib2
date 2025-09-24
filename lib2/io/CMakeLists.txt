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
PRIVATE
    fstream_windows.cpp
)