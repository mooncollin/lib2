export module lib2.benchmarking:benchmarking_context;

import std;

import lib2.utility;

import :benchmark;

namespace lib2::benchmarking
{
    class benchmarking_context;

    export
    struct benchmark_result
    {
        std::size_t num_iterations {0};
        std::chrono::nanoseconds total_time {0};
    };

    export
    class benchmarking_context
    {
    public:
        benchmarking_context() noexcept = default;
        virtual ~benchmarking_context() noexcept = default;
        
        virtual bool is_done(benchmark_result&) const = 0;
        virtual void add_iteration(benchmark_result& result, const std::chrono::nanoseconds iteration) const
        {
            result.total_time += iteration;
            ++result.num_iterations;
        }

        benchmark_result run_benchmark(benchmark& bench) const
        {
            benchmark_result result;
            bench.setup();

            try
            {
                while (!is_done(result))
                {
                    const lib2::stopwatch stopwatch;
                    bench();
                    add_iteration(result, stopwatch.elapsed_time());
                }
            }
            catch (...)
            {
                bench.tear_down();
                throw;
            }

            bench.tear_down();
            return result;
        }
    };

    export
    class benchmarking_iterations : public benchmarking_context
    {
    public:
        benchmarking_iterations(const std::size_t max_iterations) noexcept
            : max_iterations_{max_iterations} {}

        [[nodiscard]] std::size_t max_iterations() const noexcept
        {
            return max_iterations_;
        }

        bool is_done(benchmark_result& r) const override
        {
            return r.num_iterations == max_iterations();
        }
    private:
        std::size_t max_iterations_;
    };

    export
    class benchmarking_min_time : public benchmarking_context
    {
    public:
        benchmarking_min_time(const std::chrono::nanoseconds min_time) noexcept
            : min_time_{min_time} {}

        [[nodiscard]] std::chrono::nanoseconds min_time() const noexcept
        {
            return min_time_;
        }

        bool is_done(benchmark_result& r) const override
        {
            return r.total_time >= min_time();
        }

        void add_iteration(benchmark_result& result, const std::chrono::nanoseconds iteration) const override
        {
            benchmarking_context::add_iteration(result, iteration);
            if (is_done(result))
            {
                result.total_time = min_time();
            }
        }
    private:
        std::chrono::nanoseconds min_time_;
    };
}