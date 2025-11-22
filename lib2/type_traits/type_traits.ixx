export module lib2.type_traits;

import std;

export import :copy_traits;
export import :detector;
export import :function_traits;

namespace lib2
{
	export
	template<typename T, typename... Ts>
	struct is_any_of : public std::disjunction<std::is_same<T, Ts>...> {};

	export
	template<typename T, typename... Ts>
	constexpr bool is_any_of_v = is_any_of<T, Ts...>::value;

	export
	template<typename...>
	struct is_unique;

	export
	template<>
	struct is_unique<> : std::true_type {};

	export
	template<typename F, typename... T>
	struct is_unique<F, T...> : std::conjunction<is_unique<T...>, std::negation<is_any_of<F, T...>>> {};

	export
	template<typename... T>
	constexpr bool is_unique_v = is_unique<T...>::value;

	export
	template<class Type, template<typename...> class Template>
	struct is_specialization : std::false_type {};

	export
	template<template<typename...> class Template, class... Types>
	struct is_specialization<Template<Types...>, Template> : std::true_type {};

	export
	template<class Type, template<typename...> class Template>
	inline constexpr bool is_specialization_v = is_specialization<Type, Template>::value;
}