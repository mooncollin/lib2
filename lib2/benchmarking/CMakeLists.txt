target_sources(lib2
PUBLIC
FILE_SET CXX_MODULES FILES
    do_not_optimize.ixx
    benchmark.ixx
    benchmarking_context.ixx
    benchmark_suite.ixx
    benchmark_output.ixx
    benchmarking.ixx
PRIVATE
    benchmark_output.cpp
)