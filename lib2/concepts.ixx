export module lib2.concepts;

import std;

import lib2.type_traits;

namespace lib2
{
	export
	template<class F, class... Args>
	concept function_object = std::is_object_v<F> && std::invocable<F, Args...>;

	export
	template<class Type, template<typename...> class Template>
	concept specialization_of = lib2::is_specialization_v<Type, Template>;

	export
	template<class T, class U>
    concept like_a = std::same_as<std::remove_cvref_t<T>, U> || std::convertible_to<T, U>;
}