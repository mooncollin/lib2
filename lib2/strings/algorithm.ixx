module;

#include <climits>

export module lib2.strings:algorithm;

import std;

import :character;

namespace lib2
{
    export
    template<class CharT, class Traits>
    [[nodiscard]] constexpr std::basic_string_view<CharT, Traits> trim_front(std::basic_string_view<CharT, Traits> s, const std::basic_string_view<CharT, Traits> trim_str) noexcept
    {
        if (s.starts_with(trim_str))
        {
            s.remove_prefix(std::size(trim_str));
            return trim_front(s, trim_str);
        }

        return s;
    }

    export
    template<class CharT, class Traits>
    [[nodiscard]] constexpr std::basic_string_view<CharT, Traits> trim_front(std::basic_string_view<CharT, Traits> s, const CharT trim_char) noexcept
    {
        if (const auto position {s.find_first_not_of(trim_char)};
            position != std::basic_string_view<CharT, Traits>::npos)
        {
            s.remove_prefix(position);
            return s;
        }

        return {};
    }

    export
    template<class CharT, class Traits>
    [[nodiscard]] constexpr std::basic_string_view<CharT, Traits> trim_back(std::basic_string_view<CharT, Traits> s, const std::basic_string_view<CharT, Traits> trim_str) noexcept
    {
        if (s.ends_with(trim_str))
        {
            s.remove_suffix(trim_str.size());
            return trim_back(s, trim_str);
        }
        
        return s;
    }

    export
    template<class CharT, class Traits>
    [[nodiscard]] constexpr std::basic_string_view<CharT, Traits> trim_back(std::basic_string_view<CharT, Traits> s, const CharT trim_char) noexcept
    {
        if (const auto end_position {s.find_last_not_of(trim_char)};
            end_position != std::string_view::npos)
        {
            s.remove_suffix(std::size(s) - end_position - 1);
            return s;
        }

        return {};
    }

    export
    template<class CharT, class Traits>
    [[nodiscard]] constexpr std::string_view trim(const std::basic_string_view<CharT, Traits> s, const std::basic_string_view<CharT, Traits> trim_str) noexcept
    {
        return trim_back(trim_front(s, trim_str), trim_str);
    }

    export
    template<class CharT, class Traits, class Alloc>
    void find_and_replace(std::basic_string<CharT, Traits, Alloc>& input, const std::basic_string_view<CharT, Traits> find, const std::basic_string_view<CharT, Traits> replace)
    {
        auto current_location {input.find(find)};

        while (current_location != std::basic_string<CharT, Traits>::npos)
        {
            input.replace(current_location, std::size(find), replace);
            current_location = input.find(find, current_location + 1);
        }
    }
}