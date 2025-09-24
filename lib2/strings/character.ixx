export module lib2.strings:character;

import std;

import :utility;

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
            return c == ' '  ||
                   c == '\t' ||
                   c == '\n' ||
                   c == '\v' ||
                   c == '\f' ||
                   c == '\r';
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
}