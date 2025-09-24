export module lib2.strings;

import std;

export import :utility;
export import :character;
export import :algorithm;
export import :string_literal;
export import :lazy_string;

namespace lib2
{
    export
    template<character CharT, std::size_t N>
    [[nodiscard]] consteval std::basic_string_view<CharT> s_widen(
        const char(&char_data)[N]
    ,   const wchar_t(&wchar_data)[N]
    ,   const char8_t(&char8_data)[N]
    ,   const char16_t(&char16_data)[N]
    ,   const char32_t(&char32_data)[N]
    )
    {
        using t = std::remove_const_t<CharT>;
        if constexpr (std::same_as<t, char>)
        {
            return char_data;
        }
        else if constexpr (std::same_as<t, wchar_t>)
        {
            return wchar_data;
        }
        else if constexpr (std::same_as<t, char8_t>)
        {
            return char8_data;
        }
        else if constexpr (std::same_as<t, char16_t>)
        {
            return char16_data;
        }
        else if constexpr (std::same_as<t, char32_t>)
        {
            return char32_data;
        }
        else
        {
            static_assert(false, "widen does not support this character type");
        }
    }
}