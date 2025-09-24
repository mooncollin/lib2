export module lib2.test:assert;

import std;

import lib2.concepts;
import lib2.strings;
import lib2.io;
import lib2.fmt;

import :assert_exception;

namespace lib2::test
{
	namespace details
	{
		template<class T>
		[[nodiscard]] static inline std::string output(const T& value)
		{
			if constexpr (character<T>)
			{
				if (value == std::decay_t<T>{})
				{
					return "'\\0'";
				}
				else
				{
					if constexpr (std::same_as<std::decay_t<T>, char>)
					{
						return {1, value};
					}
					else
					{
						return lib2::format("<{}>'\\x{:x}'", typeid(value).name(), static_cast<std::intmax_t>(value));
					}
				}
			}
			else if constexpr (lib2::formattable<std::decay_t<T>, char>)
			{
				return lib2::format("{}", value);
			}
			else if constexpr (lib2::formattable<std::decay_t<T>, wchar_t>)
			{
				return lib2::narrow(lib2::format(L"{}", value));
			}
			else
			{
				return lib2::format("<{} at {}>", typeid(value).name(), static_cast<const void*>(std::addressof(value)));
			}
		}

		[[nodiscard]] static inline std::string output(const auto& actual, const auto& expected)
		{
			return lib2::format("Actual:\n{}\nExpected:\n{}", output(actual), output(expected));
		}

		template<std::input_iterator I1,
				std::sentinel_for<I1> S1,
				std::input_iterator I2,
				std::sentinel_for<I2> S2>
		[[nodiscard]] static inline std::string output(I1 actual, I1 mismatch1, S1 end1, I2 expected, I2 mismatch2, S2 end2)
		{
			if constexpr (std::forward_iterator<I1> && std::forward_iterator<I2>)
			{
				std::string actual_str {"["};
				std::size_t actual_end_size {1};

				if (actual != end1)
				{
					if (actual == mismatch1)
					{
						++actual_end_size;
					}

					actual_str += output(*actual);
					++actual;
				}

				for (; actual != end1; ++actual)
				{
					actual_str += ", ";
					actual_str += output(*actual);
					if (actual == mismatch1)
					{
						actual_end_size = actual_str.size();
					}
				}

				actual_str += ']';

				std::string expected_str {"["};
				std::size_t expected_end_size {1};

				if (expected != end2)
				{
					if (expected == mismatch2)
					{
						++expected_end_size;
					}
					
					expected_str += output(*expected);
					++expected;
				}

				for (; expected != end2; ++expected)
				{
					expected_str += ", ";
					expected_str += output(*expected);
					if (expected == mismatch2)
					{
						expected_end_size = expected_str.size();
					}
				}
				
				expected_str += ']';

				return lib2::format("Actual:\n{}\n{:>{}}\n\nExpected:\n{}\n{:>{}}\n",
						actual_str,
						'^', actual_end_size,
						expected_str,
						'^', expected_end_size);
			}
			else
			{
				return {};
			}
		}
	}

	export
	void fail(const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		throw assert_exception{lib2::format("({}:{}) failure: {}", location.file_name(), location.line(), message)};
	}

	export
	void error(const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		throw error_exception{lib2::format("({}:{}) error: {}", location.file_name(), location.line(), message)};
	}

	export
	template<class T, class F>
		requires(requires(const T& t, const F& f) {
			{ t == f } -> std::convertible_to<bool>;
		})
	void assert_equal(const T& actual, const F& expected, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if(!(actual == expected))
		{
			throw assert_exception{lib2::format("({}:{}) assert_equal failed: {}\n{}", location.file_name(), location.line(), message, details::output(actual, expected))};
		}
	}

	export
	template<class T, class F>
		requires(requires(const T& t, const F& f) {
			{ t != f } -> std::convertible_to<bool>;
		})
	void assert_not_equal(const T& actual, const F& expected, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (!(actual != expected))
		{
			throw assert_exception{lib2::format("({}:{}) assert_not_equal failed: {}\n{}", location.file_name(), location.line(), message, details::output(actual, expected))};
		}
	}

	export
	template<character CharT>
	void assert_cstrings_equal(const CharT* const actual, const CharT* const expected, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (std::basic_string_view{actual} != std::basic_string_view{expected})
		{
			throw assert_exception{lib2::format("({}:{}) assert_cstrings_equal failed: {}\n{}", location.file_name(), location.line(), message, details::output(actual, expected))};
		}
	}
	
	export
	template<character CharT>
	void assert_cstrings_not_equal(const CharT* const actual, const CharT* const expected, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (std::basic_string_view{actual} == std::basic_string_view{expected})
		{
			throw assert_exception{lib2::format("({}:{}) assert_cstrings_not_equal failed: {}\n{}", location.file_name(), location.line(), message, details::output(actual, expected))};
		}
	}

	export
	template<class T>
		requires(std::constructible_from<bool, T>)
	void assert_true(const T& actual, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (!actual)
		{
			throw assert_exception{lib2::format("({}:{}) assert_true failed: {}", location.file_name(), location.line(), message)};
		}
	}

	export
	template<class T>
		requires(std::constructible_from<bool, T>)
	void assert_false(const T& actual, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (actual)
		{
			throw assert_exception{lib2::format("({}:{}) assert_false failed: {}", location.file_name(), location.line(), message)};
		}
	}

	export
	template<class T>
	void assert_is(const T& actual, const T& expected, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (std::addressof(actual) != std::addressof(expected))
		{
			throw assert_exception{lib2::format("({}:{}) assert_is failed: {}\n{}", location.file_name(), location.line(), message, details::output(std::addressof(actual), std::addressof(expected)))};
		}
	}

	export
	template<class T, class F>
		requires(std::equality_comparable_with<T*, F*>)
	void assert_is_not(const T& actual, const F& expected, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (std::addressof(actual) == std::addressof(expected))
		{
			throw assert_exception{lib2::format("({}:{}) assert_is_not failed: {}\n{}", location.file_name(), location.line(), message, details::output(std::addressof(actual), std::addressof(expected)))};
		}
	}

	export
	template<class Actual, class Expected>
	void assert_is_type(const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if constexpr (!std::same_as<Actual, Expected>)
		{
			throw assert_exception{lib2::format("({}:{}) assert_is_type failed: {}\n{}", location.file_name(), location.line(), message, details::output(typeid(Actual).name(), typeid(Expected).name()))};
		}
	}

	export
	template<class Actual, class Expected>
	void assert_is_not_type(const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if constexpr (std::same_as<Actual, Expected>)
		{
			throw assert_exception{lib2::format("({}:{}) assert_is_not_type failed: {}\n{}", location.file_name(), location.line(), message, details::output(typeid(Actual).name(), typeid(Expected).name()))};
		}
	}

	export
	template<class Actual, template<class...> class Expected>
	void assert_is_specialization_of(const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if constexpr (!lib2::specialization_of<Actual, Expected>)
		{
			throw assert_exception{lib2::format("({}:{}) assert_is_specialization_of failed: {}\n{}", location.file_name(), location.line(), message, details::output(typeid(Actual).name(), typeid(Expected).name()))};
		}
	}

	export
	template<class Actual, template<class...> class Expected>
	void assert_is_not_specialization_of(const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if constexpr (lib2::specialization_of<Actual, Expected>)
		{
			throw assert_exception{lib2::format("({}:{}) assert_is_not_specialization_of failed: {}\n{}", location.file_name(), location.line(), message, details::output(typeid(Actual).name(), typeid(Expected).name()))};
		}
	}

	export
	template<class Expected, class Actual>
	void assert_is_instance(const Actual& actual, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		const auto casted {dynamic_cast<const Expected*>(std::addressof(actual))};
		if (casted == nullptr)
		{
			throw assert_exception{lib2::format("({}:{}) assert_is_instance failed: {}\n{}", location.file_name(), location.line(), message, details::output(typeid(Actual).name(), typeid(Expected).name()))};
		}
	}

	export
	template<class Expected, class Actual>
	void assert_is_not_instance(const Actual& actual, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		const auto casted = dynamic_cast<const Expected*>(std::addressof(actual));
		if (casted != nullptr)
		{
			throw assert_exception{lib2::format("({}:{}) assert_is_not_instance failed: {}\n{}", location.file_name(), location.line(), message, details::output(typeid(Actual).name(), typeid(Expected).name()))};
		}
	}

	export
	template<class T, std::ranges::input_range R>
	void assert_in(const T& member, R&& r, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (std::ranges::find(r, member) == std::ranges::end(r))
		{
			throw assert_exception{lib2::format("({}:{}) assert_in failed: {}\n", location.file_name(), location.line(), message)};
		}
	}

	export
	template<class T, std::ranges::forward_range R>
	void assert_not_in(const T& member, R&& r, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (std::ranges::find(r, member) != std::end(r))
		{
			throw assert_exception{lib2::format("({}:{}) assert_not_in failed: {}\n", location.file_name(), location.line(), message)};
		}
	}

	export
	template<class T, class F, class... Args>
		requires(std::invocable<F, Args...>)
	void assert_throws(F&& callable, const std::string_view message, Args&&... args, const std::source_location& location = std::source_location::current())
	{
		try
		{
			std::invoke(std::forward<F>(callable), std::forward<Args>(args)...);
		}
		catch (const std::decay_t<T>&)
		{
			// Do nothing, it passed.
			return;
		}

		throw assert_exception{lib2::format("({}:{}) assert_throws failed: {}\nExpected {} to be thrown, but didn't", location.file_name(), location.line(), message, typeid(T).name())};
	}

	export
	template<class T, class F, class... Args>
		requires(std::invocable<F, Args...>)
	void assert_throws(F&& callable, Args&&... args, const std::source_location& location = std::source_location::current())
	{
		assert_throws<T>(callable, std::string_view{}, std::forward<Args>(args)..., location);
	}

	export
	template<std::floating_point T, std::floating_point F>
	void assert_almost_equal(const T& actual, const F& expected, const std::common_type_t<T, F>& delta, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		const auto difference {std::abs(actual - expected)};
		if (difference > delta)
		{
			throw assert_exception{lib2::format("({}:{}) assert_almost_equal failed: {}\n{}Difference: {}\n", location.file_name(), location.line(), message, details::output(actual, expected), difference)};
		}
	}

	export
	template<std::floating_point T, std::floating_point F>
	void assert_not_almost_equal(const T& actual, const F& expected, const std::common_type_t<T, F>& delta, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		const auto difference {std::abs(actual - expected)};
		if (difference <= delta)
		{
			throw assert_exception{lib2::format("({}:{}) assert_not_almost_equal failed: {}\n{}Difference: {}\n", location.file_name(), location.line(), message, details::output(actual, expected), difference)};
		}
	}

	export
	template<class T, class F>
	void assert_greater(const T& actual, const F& expected, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (!(actual > expected))
		{
			throw assert_exception{lib2::format("({}:{}) assert_greater failed: {}\n{}", location.file_name(), location.line(), message, details::output(actual, expected))};
		}
	}

	export
	template<class T, class F>
	void assert_greater_equal(const T& actual, const F& expected, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (!(actual >= expected))
		{
			throw assert_exception{lib2::format("({}:{}) assert_greater_equal failed: {}\n{}", location.file_name(), location.line(), message, details::output(actual, expected))};
		}
	}

	export
	template<class T, class F>
	void assert_less(const T& actual, const F& expected, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (!(actual < expected))
		{
			throw assert_exception{lib2::format("({}:{}) assert_less failed: {}\n{}", location.file_name(), location.line(), message, details::output(actual, expected))};
		}
	}

	export
	template<class T, class F>
	void assert_less_equal(const T& actual, const F& expected, const std::string_view message = {}, const std::source_location& location = std::source_location::current())
	{
		if (!(actual <= expected))
		{
			throw assert_exception{lib2::format("({}:{}) assert_less_equal failed: {}\n{}", location.file_name(), location.line(), message, details::output(actual, expected))};
		}
	}

	export
	template<std::input_iterator I1,
			 std::sentinel_for<I1> S1,
			 std::input_iterator I2,
			 std::sentinel_for<I2> S2,
			 class Pred = std::ranges::equal_to,
			 class Proj1 = std::identity,
			 class Proj2 = std::identity>
		requires(std::indirectly_comparable<I1, I2, Pred, Proj1, Proj2>)
	void assert_ranges_equal(I1 actual, S1 end1, I2 expected, S2 end2, const std::string_view message = {}, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}, const std::source_location& location = std::source_location::current())
	{
		const auto mismatch {std::ranges::mismatch(actual, end1, expected, end2, std::move(pred), std::move(proj1), std::move(proj2))};
		if (mismatch.in1 != end1)
		{
			throw assert_exception{lib2::format("({}:{}) assert_ranges_equal failed: {}\n{}", location.file_name(), location.line(), message, details::output(actual, mismatch.in1, end1, expected, mismatch.in2, end2))};
		}
	}

	export
	template<std::ranges::input_range Range1, std::ranges::input_range Range2,
				class Pred = std::ranges::equal_to,
				class Proj1 = std::identity,
				class Proj2 = std::identity>
		requires(std::indirectly_comparable<std::ranges::iterator_t<Range1>, 
											std::ranges::iterator_t<Range2>, 
											Pred,
											Proj1,
											Proj2>)
	void assert_ranges_equal(Range1&& r1, Range2&& r2, const std::string_view message = {}, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}, const std::source_location& location = std::source_location::current())
	{
		assert_ranges_equal(std::ranges::begin(r1),
							std::ranges::end(r1),
							std::ranges::begin(r2),
							std::ranges::end(r2),
							message,
							std::move(pred),
							std::move(proj1),
							std::move(proj2),
							location);
	}
}