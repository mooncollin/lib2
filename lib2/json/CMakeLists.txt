target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
    tokenizer.ixx
    reader.ixx
    writer.ixx
    json.ixx
PRIVATE
    tokenizer.cpp
    reader.cpp
    writer.cpp
)