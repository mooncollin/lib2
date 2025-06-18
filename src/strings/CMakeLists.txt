target_sources(lib2 PUBLIC
  FILE_SET lib2_strings_modules TYPE CXX_MODULES FILES
	character.ixx
	algorithm.ixx
	string_literal.ixx
	lazy_string.ixx
  	strings.ixx
)

target_sources(lib2 PUBLIC
  FILE_SET lib2_strings_headers TYPE HEADERS FILES
	include/lib2/strings/widen.h
)

target_include_directories(lib2 PUBLIC include)