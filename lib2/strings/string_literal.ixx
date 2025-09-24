export module lib2.strings:string_literal;

import std;

import :character;

namespace lib2
{
    export
    template<character CharT, std::size_t N>
    struct basic_string_literal
    {
        CharT data_[N + 1] {};

        using value_type      = CharT;
        using pointer         = CharT*;
        using const_pointer   = const CharT*;
        using reference       = CharT&;
        using const_reference = const CharT&;
        using const_iterator  = const CharT*;
        using iterator        = const_iterator;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;

        constexpr basic_string_literal(const CharT* const ptr, const std::integral_constant<std::size_t, N>) noexcept
        {
            std::copy_n(ptr, N, data_);
        }

        constexpr basic_string_literal(const CharT(&txt)[N + 1]) noexcept
            : basic_string_literal{txt, std::integral_constant<std::size_t, N>{}} {}

        static constexpr std::integral_constant<std::size_t, N> size {};

        [[nodiscard]] constexpr const CharT* data() const noexcept
        {
            return data_;
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> view() const noexcept
        {
            return {data(), size()};
        }

        [[nodiscard]] constexpr operator std::basic_string_view<CharT>() const noexcept
        {
            return view();
        }
    };

    export
    template<typename CharT, std::size_t N>
    basic_string_literal(const CharT (&str)[N]) -> basic_string_literal<CharT, N - 1>;

    export
    template<typename CharT, std::size_t N>
    basic_string_literal(const CharT* ptr, std::integral_constant<std::size_t, N>) -> basic_string_literal<CharT, N>;

    export
    template<std::size_t N>
    using string_literal = basic_string_literal<char, N>;

    export
    template<std::size_t N>
    using wstring_literal = basic_string_literal<wchar_t, N>;
}