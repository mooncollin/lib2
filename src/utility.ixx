export module lib2.utility;

import std;

namespace lib2
{
	export
    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };

	export
    template<std::integral T, std::integral U>
    [[nodiscard]] constexpr bool add_would_overflow(const T x, const U y) noexcept
    {
        return std::cmp_greater(x, std::numeric_limits<T>::max() - y);
    }

	export
	class stopwatch
	{
		public:
			using clock_t = std::chrono::steady_clock;

			stopwatch() noexcept
				: start_time_{clock_t::now()} {}

			[[nodiscard]] auto elapsed_time() const noexcept
			{
				return clock_t::now() - start_time_;
			}

			[[nodiscard]] clock_t::time_point start_time() const noexcept
			{
				return start_time_;
			}

			void reset() noexcept
			{
				start_time_ = clock_t::now();
			}
		private:
			clock_t::time_point start_time_;
	};
}