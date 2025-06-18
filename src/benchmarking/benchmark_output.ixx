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
    template<class Rep, class Period>
    std::string best_fit_duration_string(const std::chrono::duration<Rep, Period> dur)
    {
        if constexpr (std::integral<Rep>)
        {
            if (dur.count() < 10000 && (dur.count() % 1000 || dur.count() == 0))
            {
                return lib2::format("{:%Q%q}", dur);
            }
        }
        else
        {
            if (dur.count() < 10000)
            {
                return lib2::format("{:.2f}{:%q}", dur.count(), dur);
            }
        }

        if constexpr (Period::num < std::exa::num)
        {
            if constexpr (std::integral<Rep>)
            {
                if (dur.count() % 1000)
                {
                    return best_fit_duration_string(
                        std::chrono::duration_cast<std::chrono::duration<float, std::ratio_multiply<Period, std::ratio<1000>>>>(dur)
                    );
                }
                else
                {
                    return best_fit_duration_string(
                        std::chrono::duration_cast<std::chrono::duration<Rep, std::ratio_multiply<Period, std::ratio<1000>>>>(dur)
                    );
                }
            }
            else
            {
                return best_fit_duration_string(
                    std::chrono::duration_cast<std::chrono::duration<Rep, std::ratio_multiply<Period, std::ratio<1000>>>>(dur)
                );
            }
        }
        else
        {
            if constexpr (std::integral<Rep>)
            {
                return lib2::format("{:%Q%q}", dur);
            }
            else
            {
                return lib2::format("{:.2f}{:%q}", dur.count(), dur);
            }
        }
    }

    float calc_perf_multiple(const benchmark_result& r1, const benchmark_result& r2) noexcept
    {
        const auto time_multiple {static_cast<float>(r2.total_time.count()) / static_cast<float>(r1.total_time.count())};
        const auto iteration_multiple {static_cast<float>(r1.num_iterations) / static_cast<float>(r2.num_iterations)};

        return std::max(time_multiple, iteration_multiple);
    }

    void print_benchmarks(lib2::text_ostream& stream, const std::vector<std::pair<std::string_view, benchmark_result>>& results)
    {
        std::size_t name_column_width {9};
        std::size_t time_column_width {5};
        std::size_t iterations_column_width {10};
        std::size_t multiple_column_width {8};

        for (const auto& [name, result] : results)
        {
            name_column_width       = std::max(name_column_width, name.size());
            time_column_width       = std::max(time_column_width, best_fit_duration_string(result.total_time).size());
            iterations_column_width = std::max(iterations_column_width, lib2::formatted_size("{}", result.num_iterations));
            multiple_column_width   = std::max(multiple_column_width, lib2::formatted_size("{:.2f}", calc_perf_multiple(result, results.back().second)));
        }

        const auto banner_width {name_column_width + 1 + time_column_width + 1 + iterations_column_width + 1 + multiple_column_width};
        const auto banner_fill {io_fill('-', banner_width)};
        
        stream << banner_fill;
        lib2::format_to(stream, "\n{:<{}} {:>{}} {:>{}} {:<{}}\n", "Benchmark", name_column_width, "Time", time_column_width, "Iterations", iterations_column_width, "Multiple", multiple_column_width);
        stream << banner_fill << '\n';
        for (const auto& [name, result] : results)
        {
            lib2::format_to(stream, "{:<{}} {:>{}} {:>{}} {:<{}.2f}\n", name, name_column_width, best_fit_duration_string(result.total_time), time_column_width, result.num_iterations, iterations_column_width, calc_perf_multiple(result, results.back().second), multiple_column_width);
        }
    }

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