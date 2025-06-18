module;

#include <climits>

export module lib2.strings:algorithm;

import std;

import :character;

namespace lib2
{
    export
	template<character CharT>
	void toupper(CharT* start, CharT* end, const std::locale& l = std::locale::classic())
	{
		const auto& facet {std::use_facet<std::ctype<CharT>>(l)};
		facet.toupper(start, end);
	}

    export
	template<class CharT, class Traits>
	void toupper(std::basic_string<CharT, Traits>& str, const std::locale l = std::locale::classic())
	{
        toupper(str.data(), str.data() + str.size(), l);
	}

    export
    template<character CharT>
	void tolower(CharT* start, CharT* end, const std::locale& l = std::locale::classic())
	{
		const auto& facet {std::use_facet<std::ctype<CharT>>(l)};
		facet.tolower(start, end);
	}

    export
	template<class CharT, class Traits>
	void tolower(std::basic_string<CharT, Traits>& str, const std::locale l = std::locale::classic())
	{
        tolower(str.data(), str.data() + str.size(), l);
	}

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
            s.remove_suffix(std::size(trim_str));
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
    template<class CharT, class Traits>
    void find_and_replace(std::basic_string<CharT, Traits>& input, const std::basic_string_view<CharT, Traits> find, const std::basic_string_view<CharT, Traits> replace)
    {
        auto current_location {input.find(find)};

        while (current_location != std::basic_string<CharT, Traits>::npos)
        {
            input.replace(current_location, std::size(find), replace);
            current_location = input.find(find, current_location + 1);
        }
    }

    // Narrow: wstring -> string
    export
    std::string narrow(const std::wstring_view wstr, const std::locale& loc = {})
    {
        const auto& cvt {std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc)};
        
        char buffer[MB_LEN_MAX];
        std::mbstate_t state {};
        auto from {wstr.data()};
        auto from_end {from + wstr.size()};
        
        std::string result;
        while (from < from_end)
        {
            auto to_next {buffer};
            const wchar_t* from_next {nullptr};

            const auto r {cvt.out(state, from, from_end, from_next, buffer, buffer + sizeof(buffer), to_next)};
            if (r == std::codecvt_base::error)
            {
                throw std::runtime_error{"Conversion failed in narrow()"};
            }

            result.append(buffer, to_next);
            from = from_next;
        }

        return result;
    }

    // Widen: string -> wstring
    export
    std::wstring widen(const std::string_view str, std::locale loc = std::locale(""))
    {
        const std::codecvt<wchar_t, char, std::mbstate_t>& cvt = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
        
        wchar_t buffer;
        std::mbstate_t state {};
        auto from {str.data()};
        auto from_end {from + str.size()};
        
        std::wstring result;
        while (from < from_end) {
            wchar_t* to_next = &buffer;
            const char* from_next = nullptr;

            const auto r {cvt.in(state, from, from_end, from_next, &buffer, &buffer + 1, to_next)};
            if (r == std::codecvt_base::error) {
                throw std::runtime_error("Conversion failed in widen()");
            }

            result.push_back(buffer);
            from = from_next;
        }

        return result;
    }
}