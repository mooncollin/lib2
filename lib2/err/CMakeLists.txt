target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
    err.ixx
)

if (WIN32)
target_sources(lib2
PRIVATE
    err_win32.cpp
)
endif()