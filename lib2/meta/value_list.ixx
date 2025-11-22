export module lib2.meta:value_list;

import std;

import :constexpr_value;
import :type_list;

namespace lib2
{
    export
    template<auto... Vs>
    struct value_list : public type_list<constexpr_value<Vs>...>
    {
    private:
        using base = type_list<constexpr_value<Vs>...>;
    public:
        using base::size;
        using base::empty;

        template<std::size_t I>
            requires(I < size())
        using type_at = typename base::template at<I>::value_type;

        template<std::size_t I>
            requires(I < size())
        static constexpr base::template at<I> at {};
    };
}

export
template<auto... Vs>
struct std::tuple_size<lib2::value_list<Vs...>> : decltype(lib2::value_list<Vs...>::size) {};

export
template<std::size_t I, auto... Vs>
struct std::tuple_element<I, lib2::value_list<Vs...>> : lib2::value_list<Vs...>::template type_at<I> {};