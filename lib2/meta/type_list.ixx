export module lib2.meta:type_list;

import std;

import :constexpr_value;

namespace lib2
{
    export
    template<typename... Ts>
    struct type_list
    {
    private:
        template<std::size_t I, typename... Tail>
        struct at_;

        template<std::size_t I, typename Head, typename... Tail>
        struct at_<I, Head, Tail...> : at_<I-1, Tail...> {};

        template<typename Head, typename... Tail>
        struct at_<0, Head, Tail...> : std::type_identity<Head> {};
    public:
        static constexpr constexpr_value<sizeof...(Ts)> size;
        static constexpr constexpr_value<size() == 0> empty;

        template<std::size_t I>
            requires(I < size())
        using at = typename at_<I, Ts...>::type;
    };
}

export
template<typename... Ts>
struct std::tuple_size<lib2::type_list<Ts...>> : decltype(lib2::type_list<Ts...>::size) {};

export
template<std::size_t I, typename... Ts>
struct std::tuple_element<I, lib2::type_list<Ts...>> : lib2::type_list<Ts...>::template at<I> {};