target_sources(lib2
	PUBLIC FILE_SET lib2_benchmarking_modules TYPE CXX_MODULES FILES
		do_not_optimize.ixx
		benchmark.ixx
		benchmarking_context.ixx
		benchmark_suite.ixx
		benchmark_output.ixx
		benchmarking.ixx
)