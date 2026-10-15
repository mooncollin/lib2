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

	export
	class timer
	{
		public:
			using clock_t = std::chrono::steady_clock;

			timer() noexcept
				: future_time_{} {}

			timer(const clock_t::time_point t) noexcept
				: future_time_{t} {}

			timer(const clock_t::duration d) noexcept
				: future_time_{clock_t::now() + d} {}

			[[nodiscard]] bool expired() const noexcept
			{
				return clock_t::now() >= future_time_;
			}

			[[nodiscard]] clock_t::time_point future_time() const noexcept
			{
				return future_time_;
			}

			void reset(const clock_t::time_point t) noexcept
			{
				future_time_ = t;
			}

			void reset(const clock_t::duration d) noexcept
			{
				future_time_ = d + clock_t::now();
			}
		private:
			clock_t::time_point future_time_;
	};

	export
	template<class T>
	struct mutable_wrapper
	{
		constexpr mutable_wrapper() = default;

		constexpr mutable_wrapper(const T& other)
			: value{other} {}

		constexpr mutable_wrapper(T&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
			: value{std::move(other)} {}

		template<class... Args>
		constexpr mutable_wrapper(Args&&... args)
			: value{std::forward<Args>(args)...} {}

		mutable T value;

		constexpr operator T&() & noexcept
		{
			return value;
		}

		constexpr operator const T&() const& noexcept
		{
			return value;
		}

		constexpr operator T&&() && noexcept
		{
			return std::move(value);
		}

		constexpr mutable_wrapper& operator=(const T& other)
		{
			value = other;
			return *this;
		}

		constexpr mutable_wrapper& operator=(T&& other)
		{
			value = std::move(other);
			return *this;
		}
	};
}