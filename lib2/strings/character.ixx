export module lib2.strings:character;

import std;

import lib2.platform;

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

    template<class T, std::size_t N>
    struct is_string_literal<const T[N]> : std::bool_constant<character<T>> {};

    export
    template<class T>
    concept string_like = std::convertible_to<T, std::string_view>;

    export
    struct cstring_sentinel_t
    {
        template<std::input_iterator It>
            requires(character<std::iter_value_t<It>>)
        constexpr bool operator==(const It it) const noexcept(noexcept(*it))
        {
            return *it == std::iter_value_t<It>{0};
        }

        template<std::input_iterator It>
            requires(character<std::iter_value_t<It>>)
        constexpr bool operator!=(const It it) const noexcept(noexcept(*it))
        {
            return *it != std::iter_value_t<It>{0};
        }
    };

    export
    template<std::input_iterator It>
        requires(character<std::iter_value_t<It>>)
    constexpr bool operator==(const It it, const cstring_sentinel_t s) noexcept(noexcept(*it))
    {
        return s == it;
    }

    export
    template<std::input_iterator It>
        requires(character<std::iter_value_t<It>>)
    constexpr bool operator!=(const It it, const cstring_sentinel_t s) noexcept(noexcept(*it))
    {
        return s != it;
    }

    export
    constexpr cstring_sentinel_t cstring_sentinel {};

    [[nodiscard]] constexpr bool is_ascii(const char c) noexcept
    {
        return c <= 127;
    }

    export
    [[nodiscard]] constexpr bool isupper_ascii(const char c) noexcept
    {
        return c >= 'A' && c <= 'Z';
    }

    export
    [[nodiscard]] constexpr bool islower_ascii(const char c) noexcept
    {
        return c >= 'a' && c <= 'z';
    }

    export
    [[nodiscard]] constexpr bool isalpha_ascii(const char c) noexcept
    {
        return isupper_ascii(c & 0xDF);
    }

    export
    constexpr bool isdigit_ascii(const char c) noexcept
    {
        return c >= '0' && c <= '9';
    }

    export
    [[nodiscard]] constexpr bool isbdigit_ascii(const char c) noexcept
    {
        return c == '0' || c == '1';
    }

    export
    [[nodiscard]] constexpr bool isxdigit_ascii(char c) noexcept
    {
        if (isdigit_ascii(c)) return true;
        c &= 0xDF;
        return c >= 'A' && c <= 'F';
    }

    export
    [[nodiscard]] constexpr bool isodigit_ascii(const char c) noexcept
    {
        return c >= '0' && c <= '7';
    }

    export
    [[nodiscard]] constexpr bool isalnum_ascii(const char c) noexcept
    {
        return isalpha_ascii(c) || isdigit_ascii(c);
    }

    export
    [[nodiscard]] constexpr bool isspace_ascii(const char c) noexcept
    {
        return (c == ' ') || (c >= '\t' && c <= '\r');
    }

    export
    [[nodiscard]] constexpr bool isblank_ascii(const char c) noexcept
    {
        return c == ' ' || c == '\t';
    }

    export
    [[nodiscard]] constexpr bool iscntrl_ascii(const char c) noexcept
    {
        return c < 32 || c == 127;
    }

    export
    [[nodiscard]] constexpr bool isgraph_ascii(const char c) noexcept
    {
        return c > 32 && c < 127;
    }

    export
    [[nodiscard]] constexpr bool isprint_ascii(const char c) noexcept
    {
        return c >= 32 && c < 127;
    }

    export
    [[nodiscard]] constexpr bool ispunct_ascii(const char c) noexcept
    {
        return isgraph_ascii(c) && !isalnum_ascii(c);
    }

    export
    [[nodiscard]] constexpr char tolower_ascii(char c) noexcept
    {
        return c += isupper_ascii(c) * 0x20;
    }

    export
    [[nodiscard]] constexpr char toupper_ascii(char c) noexcept
    {
        return c -= islower_ascii(c) * 0x20;
    }

    struct icompare_ascii_fn
    {
        [[nodiscard]] constexpr std::strong_ordering operator()(const char a, const char b) const noexcept
        {
            const auto lower_a {tolower_ascii(a)};
            const auto lower_b {toupper_ascii(b)};
            if (lower_a < lower_b)
            {
                return std::strong_ordering::less;
            }
            else if (lower_a > lower_b)
            {
                return std::strong_ordering::greater;
            }

            return std::strong_ordering::equal;
        }

        [[nodiscard]] constexpr std::strong_ordering operator()(const std::string_view a, const std::string_view b) const noexcept
        {
            const auto min_size {std::min(a.size(), b.size())};
            for (std::size_t i {0}; i < min_size; ++i)
            {
                const auto char_order {(*this)(a[i], b[i])};
                if (char_order != std::strong_ordering::equal)
                {
                    return char_order;
                }
            }

            if (a.size() < b.size())
            {
                return std::strong_ordering::less;
            }
            else if (a.size() > b.size())
            {
                return std::strong_ordering::greater;
            }

            return std::strong_ordering::equal;
        }
    };

    export
    constexpr icompare_ascii_fn icompare_ascii {};

    struct iequal_ascii_fn
    {
        [[nodiscard]] constexpr bool operator()(const char a, const char b) const noexcept
        {
            return tolower_ascii(a) == tolower_ascii(b);
        }

        [[nodiscard]] constexpr bool operator()(const std::string_view a, const std::string_view b) const noexcept
        {
            return std::ranges::equal(a, b, {}, tolower_ascii, tolower_ascii);
        }
    };

    export
    constexpr iequal_ascii_fn iequal_ascii {};

    struct icontains_ascii_fn
    {
        [[nodiscard]] constexpr bool operator()(const std::string_view haystack, const std::string_view needle) const noexcept
        {
            if (needle.empty())
            {
                return true;
            }

            if (haystack.size() < needle.size())
            {
                return false;
            }

            for (std::size_t i {0}; i <= haystack.size() - needle.size(); ++i)
            {
                bool found {true};
                for (std::size_t j {0}; j < needle.size(); ++j)
                {
                    if (!iequal_ascii(haystack[i + j], needle[j]))
                    {
                        found = false;
                        break;
                    }
                }

                if (found)
                {
                    return true;
                }
            }

            return false;
        }
    };

    export
    constexpr icontains_ascii_fn icontains_ascii {};
}