export module lib2.type_traits:function_traits;

import std;

namespace lib2
{
    export
    template<typename T>
    struct function_traits;

    // Function pointer
    template<typename R, typename... Args>
    struct function_traits<R(*)(Args...)>
    {
        using return_type = R;

        static constexpr std::size_t arity {sizeof...(Args)};

        using args = std::tuple<Args...>;

        template<std::size_t N>
        using arg = std::tuple_element_t<N, args>;
    };

    // Function reference
    template<typename R, typename... Args>
    struct function_traits<R(&)(Args...)> : function_traits<R(*)(Args...)> {};

    // std::function
    template<typename R, typename... Args>
    struct function_traits<std::function<R(Args)...>> : function_traits<R(*)(Args...)> {};

    // Member function pointer
    template<typename C, typename R, typename... Args>
    struct function_traits<R(C::*)(Args...)> : function_traits<R(*)(Args...)> {};

    template<typename C, typename R, typename... Args>
    struct function_traits<R(C::*)(Args...) const> : function_traits<R(*)(Args...)> {};

    // Generic functor or lambda
    template<typename F>
    struct function_traits : function_traits<decltype(&F::operator())> {};

    export
    template<class F>
    concept has_function_traits =
        requires
    {
        typename function_traits<F>::return_type;
    };

    export
    template<class F, std::size_t N>
    concept is_function_arity = has_function_traits<F> && (function_traits<F>::arity == N);
}