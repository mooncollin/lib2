module;

#include <climits>

export module lib2.platform:bits;

import std;

namespace lib2
{
	export
	inline constexpr std::size_t bits_in_byte {CHAR_BIT};

	export
	template<typename T>
	inline constexpr std::size_t bits_in_type {sizeof(T) * bits_in_byte};

	export
	template<std::size_t Bits>
	concept int_trivial_type = Bits > 0 && Bits <= bits_in_type<std::intmax_t>;

	export
	template<std::size_t Bits>
	concept uint_trivial_type = Bits > 0 && Bits <= bits_in_type<std::uintmax_t>;

	export
	template<std::size_t Bits>
		requires(int_trivial_type<Bits>)
	using least_bits_int = std::conditional_t<(Bits <= 8), 
								std::int_least8_t,
							std::conditional_t<(Bits <= 16),
								std::int_least16_t,
							std::conditional_t<(Bits <= 32),
								std::int_least32_t,
							std::conditional_t<(Bits <= 64),
								std::int_least64_t,
								std::intmax_t
							>>>>;

	export
	template<std::size_t Bits>
		requires(uint_trivial_type<Bits>)
	using least_bits_unsigned_int = std::conditional_t<(Bits <= 8), 
										std::uint_least8_t,
									std::conditional_t<(Bits <= 16),
										std::uint_least16_t,
									std::conditional_t<(Bits <= 32),
										std::uint_least32_t,
									std::conditional_t<(Bits <= 64),
										std::uint_least64_t,
										std::uintmax_t
									>>>>;

	export
	template<std::size_t Bits>
		requires(int_trivial_type<Bits>)
	using fast_bits_int = std::conditional_t<(Bits <= 8), 
								std::int_fast8_t,
							std::conditional_t<(Bits <= 16),
								std::int_fast16_t,
							std::conditional_t<(Bits <= 32),
								std::int_fast32_t,
							std::conditional_t<(Bits <= 64),
								std::int_fast64_t,
								std::intmax_t
							>>>>;

	export
	template<std::size_t Bits>
		requires(uint_trivial_type<Bits>)
	using fast_bits_unsigned_int = std::conditional_t<(Bits <= 8), 
										std::uint_fast8_t,
									std::conditional_t<(Bits <= 16),
										std::uint_fast16_t,
									std::conditional_t<(Bits <= 32),
										std::uint_fast32_t,
									std::conditional_t<(Bits <= 64),
										std::uint_fast64_t,
										std::uintmax_t
									>>>>;

	export
	template<std::intmax_t LeastValue>
	using least_value_int = std::conditional_t<(LeastValue >= std::numeric_limits<std::int_least8_t>::min() && LeastValue <= std::numeric_limits<std::int_least8_t>::max()), 
								std::int_least8_t,
							std::conditional_t<(LeastValue >= std::numeric_limits<std::int_least16_t>::min() && LeastValue <= std::numeric_limits<std::int_least16_t>::max()),
								std::int_least16_t,
							std::conditional_t<(LeastValue >= std::numeric_limits<std::int_least32_t>::min() && LeastValue <= std::numeric_limits<std::int_least32_t>::max()),
								std::int_least32_t,
							std::conditional_t<(LeastValue >= std::numeric_limits<std::int_least64_t>::min() && LeastValue <= std::numeric_limits<std::int_least64_t>::max()),
								std::int_least64_t,
								std::intmax_t
							>>>>;
	
	export
	template<std::uintmax_t LeastValue>
	using least_value_unsigned_int = std::conditional_t<(LeastValue <= std::numeric_limits<std::uint_least8_t>::max()), 
										std::uint_least8_t,
										std::conditional_t<(LeastValue <= std::numeric_limits<std::uint_least16_t>::max()),
										std::uint_least16_t,
										std::conditional_t<(LeastValue <= std::numeric_limits<std::uint_least32_t>::max()),
										std::uint_least32_t,
										std::conditional_t<(LeastValue <= std::numeric_limits<std::uint_least64_t>::max()),
										std::uint_least64_t,
										std::uintmax_t
										>>>>;

	export
	template<std::intmax_t LeastValue>
	using fast_least_value_int = std::conditional_t<(LeastValue >= std::numeric_limits<std::int_fast8_t>::min() && LeastValue <= std::numeric_limits<std::int_fast8_t>::max()), 
									std::int_fast8_t,
									std::conditional_t<(LeastValue >= std::numeric_limits<std::int_fast16_t>::min() && LeastValue <= std::numeric_limits<std::int_fast16_t>::max()),
									std::int_fast16_t,
									std::conditional_t<(LeastValue >= std::numeric_limits<std::int_fast32_t>::min() && LeastValue <= std::numeric_limits<std::int_fast32_t>::max()),
									std::int_fast32_t,
									std::conditional_t<(LeastValue >= std::numeric_limits<std::int_fast64_t>::min() && LeastValue <= std::numeric_limits<std::int_fast64_t>::max()),
									std::int_fast64_t,
									std::intmax_t
									>>>>;

	export	
	template<std::uintmax_t LeastValue>
	using fast_least_value_unsigned_int = std::conditional_t<(LeastValue <= std::numeric_limits<std::uint_fast8_t>::max()), 
											std::uint_fast8_t,
											std::conditional_t<(LeastValue <= std::numeric_limits<std::uint_fast16_t>::max()),
											std::uint_fast16_t,
											std::conditional_t<(LeastValue <= std::numeric_limits<std::uint_fast32_t>::max()),
											std::uint_fast32_t,
											std::conditional_t<(LeastValue <= std::numeric_limits<std::uint_fast64_t>::max()),
											std::uint_fast64_t,
											std::uintmax_t
											>>>>;

	export
	template<long double LeastValue>
	using least_value_float = std::conditional_t<(LeastValue <= std::numeric_limits<float>::max()),
								float,
								std::conditional_t<(LeastValue <= std::numeric_limits<double>::max()),
								double,
								long double
								>>;

	export
	template<std::size_t AmountDigits>
	concept float_trivial_type = AmountDigits <= std::numeric_limits<long double>::max_exponent10;

	export
	template<std::size_t AmountDigits>
		requires(float_trivial_type<AmountDigits>)
	using least_digits_float = std::conditional_t<(AmountDigits <= std::numeric_limits<float>::max_exponent10),
									float,
								std::conditional_t<(AmountDigits <= std::numeric_limits<double>::max_exponent10),
									double,
									long double>>;

	export
	template<class T>
		requires(std::is_arithmetic_v<T>)
	using least_int_type = least_value_int<static_cast<std::intmax_t>(std::numeric_limits<T>::max())>;

	export
	template<class T>
		requires(std::is_arithmetic_v<T>)
	using least_float_type = least_value_float<static_cast<long double>(std::numeric_limits<T>::max())>;
}