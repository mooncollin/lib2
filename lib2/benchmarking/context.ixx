export module lib2.benchmarking:context;

import std;

import lib2.utility;
import lib2.meta;

namespace lib2::benchmarking
{
    export
    struct result
    {
        std::size_t num_iterations {0};
        std::chrono::nanoseconds total_time {0};
    };

    export
    class context
    {
    public:
        constexpr context() noexcept = default;
        virtual ~context() noexcept = default;
        
        constexpr virtual bool is_done(const result&) const = 0;
        constexpr virtual void add_iteration(result& result, const std::chrono::nanoseconds iteration) const
        {
            result.total_time += iteration;
            ++result.num_iterations;
        }
    };

    export
    class benchmarking_iterations : public context
    {
    public:
        constexpr benchmarking_iterations(const std::size_t max_iterations) noexcept
            : max_iterations{max_iterations} {}

        std::size_t max_iterations;

        constexpr bool is_done(const result& r) const override
        {
            return r.num_iterations == max_iterations;
        }
    private:
    };

    export
    class benchmarking_min_time : public context
    {
    public:
        constexpr benchmarking_min_time(const std::chrono::nanoseconds min_time) noexcept
            : min_time{min_time} {}

        std::chrono::nanoseconds min_time;

        constexpr bool is_done(const result& r) const override
        {
            return r.total_time >= min_time;
        }

        constexpr void add_iteration(result& result, const std::chrono::nanoseconds iteration) const override
        {
            context::add_iteration(result, iteration);
            if (is_done(result))
            {
                result.total_time = min_time;
            }
        }
    };

    export
    class state
    {
        class iterator
        {
        public:
            iterator& operator++()
            {
                owner->ctx.add_iteration(owner->rslt, stopwatch.elapsed_time());
                stopwatch.reset();
                return *this;
            }

            iterator operator++(int)
            {
                const auto tmp {*this};
                ++*this;
                return tmp;
            }

            constexpr decltype(auto) operator*() const noexcept
            {
                return std::ignore;
            }

            [[nodiscard]] bool operator==(const std::default_sentinel_t) const noexcept
            {
                return owner->ctx.is_done(owner->rslt);
            }
        private:
            state* owner;
            lib2::stopwatch stopwatch;

            iterator(state& owner) noexcept
                : owner{&owner} {}

            friend class state;
        };
    public:
        state(const context& ctx) noexcept
            : ctx{ctx} {}

        state(const state&) = delete;
        state(state&&) = delete;

        state& operator=(const state&) = delete;
        state& operator=(state&&) = delete;

        [[nodiscard]] result result() const noexcept
        {
            return rslt;
        }

        [[nodiscard]] iterator begin() noexcept
        {
            return {*this};
        }

        static constexpr constexpr_value<std::default_sentinel> end {};
    private:
        const context& ctx;
        struct result rslt;
    };
}