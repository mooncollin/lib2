target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
    do_not_optimize.ixx
    context.ixx
    benchmark.ixx
    suite.ixx
    output.ixx
    benchmarking.ixx
PRIVATE
    output.cpp
)