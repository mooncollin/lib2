export module lib2.benchmarking:benchmark_output;

import std;

import lib2.concepts;
import lib2.io;
import lib2.fmt;

import :benchmark;
import :benchmark_suite;
import :benchmarking_context;

namespace lib2::benchmarking
{
    void print_benchmarks(lib2::text_ostream& stream, const std::vector<std::pair<std::string_view, benchmark_result>>& results);

    export
    void print_benchmarks(lib2::text_ostream& stream, const benchmarking_context& ctx, benchmark_suite& suite)
    {
        std::vector<std::pair<std::string_view, benchmark_result>> runs;

        for (auto& bench : suite)
        {
            runs.emplace_back(std::piecewise_construct,
                std::forward_as_tuple(bench.name()),
                std::forward_as_tuple(ctx.run_benchmark(bench))
            );
        }

        std::ranges::sort(runs, [](const auto& r1, const auto& r2) {
            return (r1.total_time < r2.total_time) ||
                   (r1.total_time == r2.total_time &&
                    r1.num_iterations > r2.num_iterations
                   );
        }, [](const auto& pair) {
            return pair.second;
        });
        
        print_benchmarks(stream, runs);
    }

    export
    void print_benchmarks(const benchmarking_context& ctx, benchmark_suite& suite)
    {
        print_benchmarks(lib2::cout, ctx, suite);
    }

    export
    template<std::derived_from<benchmark>... Benches>
    void print_benchmarks(lib2::text_ostream& stream, const benchmarking_context& ctx, Benches&... benches)
    {
        std::vector<std::pair<std::string_view, benchmark_result>> runs;

        const auto add_run {[&](auto& bench) {
            runs.emplace_back(std::piecewise_construct,
                std::forward_as_tuple(bench.name()),
                std::forward_as_tuple(ctx.run_benchmark(bench))
            );
        }};

        (add_run(benches), ...);

        std::ranges::sort(runs, [](const auto& r1, const auto& r2) {
            return (r1.total_time < r2.total_time) ||
                   (r1.total_time == r2.total_time &&
                    r1.num_iterations > r2.num_iterations
                   );
        }, [](const auto& pair) {
            return pair.second;
        });
        
        print_benchmarks(stream, runs);
    }

    export
    template<std::derived_from<benchmark>... Benches>
    void print_benchmarks(const benchmarking_context& ctx, Benches&... benches)
    { 
        print_benchmarks(lib2::cout, ctx, benches...);
    }
}