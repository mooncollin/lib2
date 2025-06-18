target_sources(lib2 PUBLIC
  FILE_SET lib2_io_modules TYPE CXX_MODULES FILES
    character.ixx
	stream_traits.ixx
	file.ixx
	ostream.ixx
	istream.ixx
	iostream.ixx
	stringstream.ixx
	spanstream.ixx
	fstream.ixx
    io.ixx
)

target_sources(lib2 PRIVATE
  file.cpp
)