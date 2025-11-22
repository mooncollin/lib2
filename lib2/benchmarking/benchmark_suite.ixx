export module lib2.benchmarking:benchmark_suite;

import std;

import lib2.iterator;

import :benchmark;

namespace lib2::benchmarking
{
    export
    class benchmark_suite
    {
    public:
        benchmark_suite() noexcept = default;
        
        template<std::derived_from<benchmark> T, class... Args>
        void add_benchmark(Args&&... args)
        {
            benchmarks_.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

        template<template<class...> class T, class... Args>
        void add_benchmark(Args&&... args)
        {
            using benchmark_t = decltype(T{std::forward<Args>(args)...});
            add_benchmark<benchmark_t>(std::forward<Args>(args)...);
        }

        // template<std::derived_from<benchmark> T>
        // void add_benchmark(T&& bench)
        // {
        //     benchmarks_.push_back(std::make_unique<T>(std::forward<T>(bench)));
        // }

        void add_benchmark_suite(benchmark_suite&& suite) noexcept
        {
            benchmarks_.splice(benchmarks_.end(), std::move(suite.benchmarks_));
        }

        [[nodiscard]] auto begin() noexcept
        {
            return indirect_iterator{benchmarks_.begin()};
        }

        [[nodiscard]] auto end() noexcept
        {
            return indirect_iterator{benchmarks_.end()};
        }

        [[nodiscard]] std::size_t size() const noexcept
        {
            return benchmarks_.size();
        }

        void clear() noexcept
        {
            benchmarks_.clear();
        }
    private:
        std::list<std::unique_ptr<benchmark>> benchmarks_;
    };
}