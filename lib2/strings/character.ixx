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

    export
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

    struct is_ascii_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            return c <= 127;
        }
    };

    export
    constexpr is_ascii_fn is_ascii {};

    struct is_ascii_alpha_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(CharT c) const noexcept
        {
            c &= 0xDF;
            return c >= 'A' && c <= 'Z';
        }
    };

    export
    constexpr is_ascii_alpha_fn is_ascii_alpha {};

    struct is_ascii_upper_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            return c >= 'A' && c <= 'Z';
        }
    };

    export
    constexpr is_ascii_upper_fn is_ascii_upper {};

    struct is_ascii_lower_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            return c >= 'a' && c <= 'z';
        }
    };

    export
    constexpr is_ascii_lower_fn is_ascii_lower {};

    struct is_ascii_digit_fn
    {
        template<character CharT>
        constexpr bool operator()(const CharT c) const noexcept
        {
            return c >= '0' && c <= '9';
        }

        template<character CharT>
        constexpr bool operator()(const CharT c, const int base) const
        {
            if (base < 2 || base > 36)
            {
                throw std::out_of_range{"base must be in range 2..36"};
            }
            
            switch (c)
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return static_cast<int>(c - '0') < base;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
                return static_cast<int>(10 + static_cast<int>(c - 'A')) < base;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
                return static_cast<int>(10 + static_cast<int>(c - 'a')) < base;
            default:
                return false;
            }
        }
    };

    export
    constexpr is_ascii_digit_fn is_ascii_digit {};

    struct is_ascii_bit_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            return c == '0' || c == '1';
        }
    };

    export
    constexpr is_ascii_bit_fn is_ascii_bit {};

    struct is_ascii_xdigit_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(CharT c) const noexcept
        {
            if (is_ascii_digit(c))
            {
                return true;
            }
            
            c &= 0xDF;
            return c >= 'A' && c <= 'F';
        }
    };

    export
    constexpr is_ascii_xdigit_fn is_ascii_xdigit {};

    struct is_ascii_odigit_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            return c >= '0' && c <= '7';
        }
    };

    export
    constexpr is_ascii_odigit_fn is_ascii_odigit {};

    struct is_ascii_alnum_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            return is_ascii_alpha(c) || is_ascii_digit(c);
        }
    };

    export
    constexpr is_ascii_alnum_fn is_ascii_alnum {};

    struct is_ascii_space_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            switch (c)
            {
            case ' ':
            case '\t':
            case '\n':
            case '\v':
            case '\f':
            case '\r':
                return true;
            default:
                return false;
            }
        }
    };

    export
    constexpr is_ascii_space_fn is_ascii_space {};

    struct is_ascii_blank_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            return c == ' ' || c == '\t';
        }
    };

    export
    constexpr is_ascii_blank_fn is_ascii_blank {};

    struct is_ascii_cntrl_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            return c < 32 || c == 127;
        }
    };

    export
    constexpr is_ascii_cntrl_fn is_ascii_cntrl {};

    struct is_ascii_graph_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            return c > 32 && c < 127;
        }
    };

    export
    constexpr is_ascii_graph_fn is_ascii_graph {};

    struct is_ascii_print_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            return c >= 32 && c < 127;
        }
    };

    export
    constexpr is_ascii_print_fn is_ascii_print {};

    struct is_ascii_punct_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT c) const noexcept
        {
            return is_ascii_graph(c) && !is_ascii_alnum(c);
        }
    };

    export
    constexpr is_ascii_punct_fn is_ascii_punct {};

    struct ascii_to_lower_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr CharT operator()(const CharT c) const noexcept
        {
            return (c >= 'A' && c <= 'Z') ? (c | 0x20) : c;
        }
    };

    export
    constexpr ascii_to_lower_fn ascii_to_lower {};

    struct ascii_to_upper_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr CharT operator()(const CharT c) const noexcept
        {
            return (c >= 'a' && c <= 'z') ? (c & ~0x20) : c;
        }
    };

    export
    constexpr ascii_to_upper_fn ascii_to_upper {};

    struct ascii_icompare_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr std::strong_ordering operator()(const CharT a, const CharT b) const noexcept
        {
            const auto lower_a {ascii_to_lower(a)};
            const auto lower_b {ascii_to_lower(b)};
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

        template<character CharT>
        [[nodiscard]] constexpr std::strong_ordering operator()(const std::basic_string_view<CharT> a, const std::basic_string_view<CharT> b) const noexcept
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
    constexpr ascii_icompare_fn ascii_icompare {};

    struct ascii_iequal_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const CharT a, const CharT b) const noexcept
        {
            return ascii_to_lower(a) == ascii_to_lower(b);
        }

        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const std::basic_string_view<CharT> a, const std::basic_string_view<CharT> b) const noexcept
        {
            if (a.size() != b.size())
            {
                return false;
            }

            for (std::size_t i {0}; i < a.size(); ++i)
            {
                if (!(*this)(a[i], b[i]))
                {
                    return false;
                }
            }

            return true;
        }
    };

    export
    constexpr ascii_iequal_fn ascii_iequal {};

    struct ascii_icontains_fn
    {
        template<character CharT>
        [[nodiscard]] constexpr bool operator()(const std::basic_string_view<CharT> haystack, const std::basic_string_view<CharT> needle) const noexcept
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
                    if (!ascii_iequal(haystack[i + j], needle[j]))
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
    constexpr ascii_icontains_fn ascii_icontains {};
}