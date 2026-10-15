export module lib2.benchmarking:suite;

import std;

import :benchmark;

namespace lib2::benchmarking
{
    struct entry
    {
        std::string name;
        std::function<void(state&)> func;
    };

    export
    class suite
    {
    public:
        suite() noexcept = default;

        template<benchmark F>
        void add(std::string name, F func)
        {
            benchmarks.emplace_back(std::move(name), std::move(func));
        }

        void add(suite&& other)
        {
            std::ranges::move(other.benchmarks, std::back_inserter(benchmarks));
        }

        [[nodiscard]] auto begin() const noexcept
        {
            return benchmarks.begin();
        }

        [[nodiscard]] auto end() const noexcept
        {
            return benchmarks.end();
        }

        [[nodiscard]] std::size_t size() const noexcept
        {
            return benchmarks.size();
        }

        void clear() noexcept
        {
            benchmarks.clear();
        }
    private:
        std::vector<entry> benchmarks;
    };
}