export module lib2.strings:character;

import std;

namespace lib2
{
	export
	template<class T>
	concept character = std::same_as<T, char>
					 || std::same_as<T, wchar_t>
					 || std::same_as<T, char8_t>
					 || std::same_as<T, char16_t>
					 || std::same_as<T, char32_t>;
	
	export
	template<typename T>
	struct is_string_literal : std::false_type {};

	export
	template<class T, std::size_t N>
	struct is_string_literal<const T[N]> : std::bool_constant<character<T>> {};
}