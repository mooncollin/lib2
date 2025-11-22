export module lib2.fmt:format;

import std;

import lib2.type_traits;
import lib2.concepts;
import lib2.utility;
import lib2.compact_optional;
import lib2.strings;
import lib2.io;

import :utility;
import :context;
import :parsers;

namespace std
{
    export
    lib2::str_fmt format_of(std::string_view);

    export
    template<class Allocator>
    lib2::str_fmt format_of(const std::basic_string<char, std::char_traits<char>, Allocator>&);
}

namespace lib2
{
    export
    struct str_fmt
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        optional_size precision {};
        bool debug {false};

        constexpr static str_fmt parse(format_context& ctx)
        {
            str_fmt fmt;
            auto it {parse_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);
            ctx.advance_to(it);
            it = parse_precision(ctx, fmt.precision);
            
            if (it != ctx.end())
            {
                if (*it == '?')
                {
                    fmt.debug = true;
                    ++it;
                }
            }

            ctx.advance_to(it);
            return fmt;
        }
    };

    export
    text_ostream& to_stream(text_ostream& os, std::string_view, const str_fmt&);

    export
    struct int_fmt
    {
        char fill {' '};
        align_type align {align_type::right};
        sign_type sign {sign_type::minus};
        bool alt {false};
        bool zero {false};
        std::size_t width {0};
        unsigned int base {10};
        bool upper {false};
        bool use_locale {false};
        const std::locale* locale {nullptr};

        constexpr static int_fmt parse(format_context& ctx)
        {
            int_fmt fmt;

            auto it {parse_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_sign(ctx, fmt.sign);
            ctx.advance_to(it);
            it = parse_alt(ctx, fmt.alt, fmt.zero);
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);
            ctx.advance_to(it);
            it = parse_locale(ctx, fmt.use_locale);
            fmt.locale = ctx.locale();
            
            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'B':
                    fmt.upper = true;
                case 'b':
                    fmt.base = 2;
                    ++it;
                    break;
                case 'o':
                    fmt.base = 8;
                    ++it;
                    break;
                case 'd':
                    fmt.base = 10;
                    ++it;
                    break;
                case 'X':
                    fmt.upper = true;
                case 'x':
                    fmt.base = 16;
                    ++it;
                    break;
                }
            }

            ctx.advance_to(it);
            return fmt;
        }
    };

    template<std::ranges::view V>
    struct escaped_t
    {
        V base;
    };

    export
    template<std::ranges::input_range R>
    constexpr auto escaped(R&& r) noexcept
    {
        return escaped_t{std::ranges::views::all(std::forward<R>(r))};
    }

    export
    template<class V>
    text_ostream& operator<<(text_ostream& os, const escaped_t<V>& e)
    {
        constexpr int_fmt codepoint_fmt{.base=16};
        constexpr int_fmt invalid_fmt{.fill='0', .align=align_type::right, .width=2, .base=16};

        os.put('"');

        const auto utf_view {views::utf_codepoints(e.base)};
        for (auto it {utf_view.begin()}; it != utf_view.end(); ++it)
        {
            switch (*it)
            {
            case '\t':
                os << "\\t";
                break;
            case '\n':
                os << "\\n";
                break;
            case '\r':
                os << "\\r";
                break;
            case '"':
                os << "\\\"";
                break;
            case '\\':
                os << "\\\\";
                break;
            default:
                if (it.invalid())
                {
                    os << "\\x";
                    to_stream(os, *it, invalid_fmt);
                }
                else if (*it <= 0x7F)
                {
                    os.put(static_cast<char>(*it));
                }
                else
                {
                    os << "\\\u";
                    to_stream(os, *it, codepoint_fmt);
                }
            }
        }

        os.put('"');
        return os;
    }

    template<std::unsigned_integral T>
    static constexpr char* to_chars_base2(char* begin, char* end, const T val) noexcept
    {
        constexpr unsigned char digit_table[][5] {
            "0000", "0001", "0010", "0011",
            "0100", "0101", "0110", "0111",
            "1000", "1001", "1010", "1011",
            "1100", "1101", "1110", "1111"
        };
        
        const auto p {reinterpret_cast<const char*>(&val)};
        constexpr std::size_t N {4};
        constexpr std::size_t byte_size {sizeof(T)};

        if (val == 0)
        {
            *--end = '0';
            return end;
        }
        
        for (std::size_t i {0}; i < byte_size; ++i)
        {
            std::copy_n(digit_table[(p[byte_size - i - 1] & 0xF0) >> 4], N, &begin[i * 8]);
            std::copy_n(digit_table[(p[byte_size - i - 1] & 0x0F)], N, &begin[i * 8 + N]);
        }

        return begin + std::countl_zero(val);
    }

    template<std::unsigned_integral T>
    static constexpr char* to_chars_base8(char* const, char* end, T val) noexcept
    {
        constexpr unsigned char digit_table[][3] {
            "00", "01", "02", "03",
            "04", "05", "06", "07",
            "10", "11", "12", "13",
            "14", "15", "16", "17",
            "20", "21", "22", "23",
            "24", "25", "26", "27",
            "30", "31", "32", "33",
            "34", "35", "36", "37",
            "40", "41", "42", "43",
            "44", "45", "46", "47",
            "50", "51", "52", "53",
            "54", "55", "56", "57",
            "60", "61", "62", "63",
            "64", "65", "66", "67",
            "70", "71", "72", "73",
            "74", "75", "76", "77"
        };

        while (val >= 64)
        {
            const auto byte {val & 0x3F};
            val >>= 6;
            end -= 2;
            end[0] = digit_table[byte][0];
            end[1] = digit_table[byte][1];
        }

        if (val < 8)
        {
            *--end = '0' + val;
        }
        else
        {
            end -= 2;
            end[0] = digit_table[val][0];
            end[1] = digit_table[val][1];
        }

        return end;
    }

    template<std::unsigned_integral T>
    static constexpr char* to_chars_base10(char* const, char* end, T val) noexcept
    {
        constexpr unsigned char digit_table[][3] {
            "00", "01", "02", "03",
            "04", "05", "06", "07",
            "08", "09", "10", "11",
            "12", "13", "14", "15",
            "16", "17", "18", "19",
            "20", "21", "22", "23",
            "24", "25", "26", "27",
            "28", "29", "30", "31",
            "32", "33", "34", "35",
            "36", "37", "38", "39",
            "40", "41", "42", "43",
            "44", "45", "46", "47",
            "48", "49", "50", "51",
            "52", "53", "54", "55",
            "56", "57", "58", "59",
            "60", "61", "62", "63",
            "64", "65", "66", "67",
            "68", "69", "70", "71",
            "72", "73", "74", "75",
            "76", "77", "78", "79",
            "80", "81", "82", "83",
            "84", "85", "86", "87",
            "88", "89", "90", "91",
            "92", "93", "94", "95",
            "96", "97", "98", "99"
        };

        while (val >= 100)
        {
            const auto remainder {val % 100};
            val /= 100;
            end -= 2;
            end[0] = digit_table[remainder][0];
            end[1] = digit_table[remainder][1];
        }

        if (val < 10)
        {
            *--end = '0' + val;
        }
        else
        {
            end -= 2;
            end[0] = digit_table[val][0];
            end[1] = digit_table[val][1];
        }

        return end;
    }

    template<std::unsigned_integral T>
    static constexpr char* to_chars_base16(char* const, char* end, T val) noexcept
    {
        constexpr unsigned char digit_table[][3] {
            "00", "01", "02", "03",
            "04", "05", "06", "07",
            "08", "09", "0a", "0b",
            "0c", "0d", "0e", "0f",
            "10", "11", "12", "13",
            "14", "15", "16", "17",
            "18", "19", "1a", "1b",
            "1c", "1d", "1e", "1f",
            "20", "21", "22", "23",
            "24", "25", "26", "27",
            "28", "29", "2a", "2b",
            "2c", "2d", "2e", "2f",
            "30", "31", "32", "33",
            "34", "35", "36", "37",
            "38", "39", "3a", "3b",
            "3c", "3d", "3e", "3f",
            "40", "41", "42", "43",
            "44", "45", "46", "47",
            "48", "49", "4a", "4b",
            "4c", "4d", "4e", "4f",
            "50", "51", "52", "53",
            "54", "55", "56", "57",
            "58", "59", "5a", "5b",
            "5c", "5d", "5e", "5f",
            "60", "61", "62", "63",
            "64", "65", "66", "67",
            "68", "69", "6a", "6b",
            "6c", "6d", "6e", "6f",
            "70", "71", "72", "73",
            "74", "75", "76", "77",
            "78", "79", "7a", "7b",
            "7c", "7d", "7e", "7f",
            "80", "81", "82", "83",
            "84", "85", "86", "87",
            "88", "89", "8a", "8b",
            "8c", "8d", "8e", "8f",
            "90", "91", "92", "93",
            "94", "95", "96", "97",
            "98", "99", "9a", "9b",
            "9c", "9d", "9e", "9f",
            "a0", "a1", "a2", "a3",
            "a4", "a5", "a6", "a7",
            "a8", "a9", "aa", "ab",
            "ac", "ad", "ae", "af",
            "b0", "b1", "b2", "b3",
            "b4", "b5", "b6", "b7",
            "b8", "b9", "ba", "bb",
            "bc", "bd", "be", "bf",
            "c0", "c1", "c2", "c3",
            "c4", "c5", "c6", "c7",
            "c8", "c9", "ca", "cb",
            "cc", "cd", "ce", "cf",
            "d0", "d1", "d2", "d3",
            "d4", "d5", "d6", "d7",
            "d8", "d9", "da", "db",
            "dc", "dd", "de", "df",
            "e0", "e1", "e2", "e3",
            "e4", "e5", "e6", "e7",
            "e8", "e9", "ea", "eb",
            "ec", "ed", "ee", "ef",
            "f0", "f1", "f2", "f3",
            "f4", "f5", "f6", "f7",
            "f8", "f9", "fa", "fb",
            "fc", "fd", "fe", "ff"
        };

        while (val >= 256)
        {
            const auto byte {val & 0xFF};
            val >>= 8;
            end -= 2;
            end[0] = digit_table[byte][0];
            end[1] = digit_table[byte][1];
        }

        if (val < 16)
        {
            *--end = digit_table[val][1];
        }
        else
        {
            end -= 2;
            end[0] = digit_table[val][0];
            end[1] = digit_table[val][1];
        }

        return end;
    }

    template<std::unsigned_integral T>
    static constexpr char* to_chars_base16_uppercase(char* const, char* end, T val) noexcept
    {
        constexpr unsigned char digit_table[][3] {
            "00", "01", "02", "03",
            "04", "05", "06", "07",
            "08", "09", "0A", "0B",
            "0C", "0D", "0E", "0F",
            "10", "11", "12", "13",
            "14", "15", "16", "17",
            "18", "19", "1A", "1B",
            "1C", "1D", "1E", "1F",
            "20", "21", "22", "23",
            "24", "25", "26", "27",
            "28", "29", "2A", "2B",
            "2C", "2D", "2E", "2F",
            "30", "31", "32", "33",
            "34", "35", "36", "37",
            "38", "39", "3A", "3B",
            "3C", "3D", "3E", "3F",
            "40", "41", "42", "43",
            "44", "45", "46", "47",
            "48", "49", "4A", "4B",
            "4C", "4D", "4E", "4F",
            "50", "51", "52", "53",
            "54", "55", "56", "57",
            "58", "59", "5A", "5B",
            "5C", "5D", "5E", "5F",
            "60", "61", "62", "63",
            "64", "65", "66", "67",
            "68", "69", "6A", "6B",
            "6C", "6D", "6E", "6F",
            "70", "71", "72", "73",
            "74", "75", "76", "77",
            "78", "79", "7A", "7B",
            "7C", "7D", "7E", "7F",
            "80", "81", "82", "83",
            "84", "85", "86", "87",
            "88", "89", "8A", "8B",
            "8C", "8D", "8E", "8F",
            "90", "91", "92", "93",
            "94", "95", "96", "97",
            "98", "99", "9A", "9B",
            "9C", "9D", "9E", "9F",
            "A0", "A1", "A2", "A3",
            "A4", "A5", "A6", "A7",
            "A8", "A9", "AA", "AB",
            "AC", "AD", "AE", "AF",
            "B0", "B1", "B2", "B3",
            "B4", "B5", "B6", "B7",
            "B8", "B9", "BA", "BB",
            "BC", "BD", "BE", "BF",
            "C0", "C1", "C2", "C3",
            "C4", "C5", "C6", "C7",
            "C8", "C9", "CA", "CB",
            "CC", "CD", "CE", "CF",
            "D0", "D1", "D2", "D3",
            "D4", "D5", "D6", "D7",
            "D8", "D9", "DA", "DB",
            "DC", "DD", "DE", "DF",
            "E0", "E1", "E2", "E3",
            "E4", "E5", "E6", "E7",
            "E8", "E9", "EA", "EB",
            "EC", "ED", "EE", "EF",
            "F0", "F1", "F2", "F3",
            "F4", "F5", "F6", "F7",
            "F8", "F9", "FA", "FB",
            "FC", "FD", "FE", "FF"
        };

        while (val >= 256)
        {
            const auto byte {val & 0xFF};
            val >>= 8;
            end -= 2;
            end[0] = digit_table[byte][0];
            end[1] = digit_table[byte][1];
        }

        if (val < 16)
        {
            *--end = digit_table[val][1];
        }
        else
        {
            end -= 2;
            end[0] = digit_table[val][0];
            end[1] = digit_table[val][1];
        }

        return end;
    }

    template<std::unsigned_integral T>
    static constexpr char* to_chars(char* const, char* end, T val, const unsigned int base) noexcept
    {
        constexpr unsigned char digit_table[] {"0123456789abcdefghijklmnopqrstuvwxyz"};

        do
        {
            const auto remainder {val % base};
            val /= base;
            *--end = digit_table[remainder];
        } while (val);
        
        return end;
    }

    template<std::unsigned_integral T>
    static constexpr char* to_chars_uppercase(char* const, char* end, T val, const unsigned int base) noexcept
    {
        constexpr unsigned char digit_table[] {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};

        while (val != 0)
        {
            const auto remainder {val % base};
            val /= remainder;
            *--end = digit_table[remainder];
        }
        
        return end;
    }

    template<std::floating_point F>
    static char* to_chars(char* const begin, char* const end, const F val) noexcept
    {
        return std::to_chars(begin, end, val).ptr;
    }

    template<std::floating_point F>
    static char* to_chars(char* const begin, char* const end, const F val, const std::chars_format fmt) noexcept
    {
        return std::to_chars(begin, end, val, fmt).ptr;
    }

    template<std::floating_point F>
    static char* to_chars(char* const begin, char* const end, const F val, const std::chars_format fmt, const int precision) noexcept
    {
        return std::to_chars(begin, end, val, fmt, precision).ptr;
    }

    export
    template<std::unsigned_integral T>
        requires(!lib2::character<T> && !std::same_as<T, bool>)
    text_ostream& operator<<(text_ostream& os, const T val)
    {
        char buf[std::numeric_limits<T>::digits10 + 1];
        const auto begin {to_chars_base10(buf, buf + std::size(buf), val)};
        return os << std::string_view{begin, buf + std::size(buf)};
    }

    export
    template<std::signed_integral T>
        requires(!lib2::character<T> && !std::same_as<T, bool>)
    text_ostream& operator<<(text_ostream& os, const T val)
    {
        char buf[std::numeric_limits<T>::digits10 + 2];
        char* begin;

        if (val < 0)
        {
            begin = to_chars_base10(buf, buf + std::size(buf), std::make_unsigned_t<T>(-val));
            *--begin = '-';
        }
        else
        {
            begin = to_chars_base10(buf, buf + std::size(buf), std::make_unsigned_t<T>(val));
        }

        return os << std::string_view{begin, buf + std::size(buf)};
    }

    export
    template<std::integral T>
    text_ostream& to_stream(text_ostream& os, T val, const int_fmt& fmt)
    {
        char buf[std::numeric_limits<T>::digits + 4];
        auto begin {buf + 3};
        auto extra_begin {begin};
        bool neg {false};

        switch (fmt.base)
        {
        case 2:
            if constexpr (std::signed_integral<T>)
            {
                if ((neg = val < 0))
                {
                    val = -val;
                }
            }
            
            extra_begin = begin = to_chars_base2(begin, buf + std::size(buf), static_cast<std::make_unsigned_t<T>>(val));

            if (fmt.alt)
            {
                *--extra_begin = fmt.upper ? 'B' : 'b';
                *--extra_begin = '0';
            }
            break;
        case 8:
            if constexpr (std::signed_integral<T>)
            {
                if ((neg = val < 0))
                {
                    val = -val;
                }
            }
            
            extra_begin = begin = to_chars_base8(begin, buf + std::size(buf), static_cast<std::make_unsigned_t<T>>(val));

            if (fmt.alt && val != 0)
            {
                *--extra_begin = '0';
            }
            break;
        case 10:
            if constexpr (std::signed_integral<T>)
            {
                if ((neg = val < 0))
                {
                    val = -val;
                }
            }
            
            extra_begin = begin = to_chars_base10(begin, buf + std::size(buf), static_cast<std::make_unsigned_t<T>>(val));
            break;
        case 16:
            if constexpr (std::signed_integral<T>)
            {
                if ((neg = val < 0))
                {
                    val = -val;
                }
            }
            
            if (fmt.upper)
            {
                extra_begin = begin = to_chars_base16_uppercase(begin, buf + std::size(buf), static_cast<std::make_unsigned_t<T>>(val));
            }
            else
            {
                extra_begin = begin = to_chars_base16(begin, buf + std::size(buf), static_cast<std::make_unsigned_t<T>>(val));
            }

            if (fmt.alt)
            {
                *--extra_begin = fmt.upper ? 'X' : 'x';
                *--extra_begin = '0';
            }
            break;
        default:
            if constexpr (std::signed_integral<T>)
            {
                if ((neg = val < 0))
                {
                    val = -val;
                }
            }

            if (fmt.upper)
            {
                extra_begin = begin = to_chars_uppercase(begin, buf + std::size(buf), static_cast<std::make_unsigned_t<T>>(val), fmt.base);
            }
            else
            {
                extra_begin = begin = to_chars(begin, buf + std::size(buf), static_cast<std::make_unsigned_t<T>>(val), fmt.base);
            }
        }

        if constexpr (std::signed_integral<T>)
        {
            if (neg)
            {
                *--extra_begin = '-';
            }
            else
            {
                switch (fmt.sign)
                {
                case sign_type::plus:
                case sign_type::space:
                    *--extra_begin = std::to_underlying(fmt.sign);
                }
            }
        }
        else
        {
            switch (fmt.sign)
            {
            case sign_type::plus:
            case sign_type::space:
                *--extra_begin = std::to_underlying(fmt.sign);
            }
        }

        str_fmt str_fmtting {
            .align = fmt.align,
            .width = fmt.width
        };
        
        if (fmt.zero)
        {
            str_fmtting.fill = '0';
            const auto extra_size {static_cast<std::size_t>(begin - extra_begin)};
            if (extra_size > 0)
            {
                os << std::string_view{extra_begin, extra_begin + extra_size};
                if (str_fmtting.width > extra_size)
                {
                    str_fmtting.width -= extra_size;
                }
            }
            to_stream(os, std::string_view{begin, buf + std::size(buf)}, str_fmtting);
        }
        else
        {
            str_fmtting.fill = fmt.fill;
            to_stream(os, std::string_view{extra_begin, buf + std::size(buf)}, str_fmtting);
        }

        return os;
    }

    export
    struct float_fmt
    {
        char fill {' '};
        align_type align {align_type::left};
        sign_type sign {sign_type::minus};
        bool alt {false};
        bool zero {false};
        std::optional<std::chars_format> type {};
        std::size_t width {0};
        optional_size precision {};
        bool upper {false};
        bool use_locale {false};
        const std::locale* locale {nullptr};

        constexpr static float_fmt parse(format_context& ctx)
        {
            float_fmt fmt;

            auto it {parse_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_sign(ctx, fmt.sign);
            ctx.advance_to(it);
            it = parse_alt(ctx, fmt.alt, fmt.zero);
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);
            ctx.advance_to(it);
            it = parse_precision(ctx, fmt.precision);
            ctx.advance_to(it);
            it = parse_locale(ctx, fmt.use_locale);
            fmt.locale = ctx.locale();
            
            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'A':
                    fmt.upper = true;
                case 'a':
                    fmt.type = std::chars_format::hex;
                    ++it;
                    break;
                case 'E':
                    fmt.upper = true;
                case 'e':
                    fmt.type = std::chars_format::scientific;
                    ++it;
                    break;
                case 'F':
                    fmt.upper = true;
                case 'f':
                    fmt.type = std::chars_format::fixed;
                    ++it;
                    break;
                case 'g':
                    fmt.upper = true;
                case 'G':
                    fmt.type = std::chars_format::general;
                    ++it;
                    break;
                }
            }

            ctx.advance_to(it);
            return fmt;
        }
    };

    export
    template<std::floating_point F>
    text_ostream& operator<<(text_ostream& os, const F val)
    {
        char buf[30];
        const auto ptr {to_chars(buf, buf + std::size(buf), val)};
        return os << std::string_view{buf, ptr};
    }

    export
    template<std::floating_point T>
    text_ostream& to_stream(text_ostream& os, T val, const float_fmt& fmt)
    {
        constexpr std::size_t extra {1};
        char buf[29 + extra];
        char* begin {buf + extra};
        char* extra_begin {begin};
        char* end;
        bool neg;

        if ((neg = val < 0))
        {
            val = -val;
        }

        if (fmt.type)
        {
            if (fmt.precision)
            {
                end = lib2::to_chars(begin, buf + std::size(buf), val, *fmt.type, *fmt.precision);
            }
            else
            {
                end = lib2::to_chars(begin, buf + std::size(buf), val, *fmt.type);
            }
        }
        else
        {
            if (fmt.precision)
            {
                end = lib2::to_chars(begin, buf + std::size(buf), val, std::chars_format::general, *fmt.precision);
            }
            else
            {
                end = lib2::to_chars(begin, buf + std::size(buf), val);
            }
        }

        if (neg)
        {
            *--extra_begin = '-';
        }
        else
        {
            switch (fmt.sign)
            {
            case sign_type::plus:
            case sign_type::space:
                *--extra_begin = std::to_underlying(fmt.sign);
                break;
            }
        }

        str_fmt str_fmtting {
            .align = fmt.align,
            .width = fmt.width
        };
        
        if (fmt.zero)
        {
            str_fmtting.fill = '0';
            const auto extra_size {static_cast<std::size_t>(begin - extra_begin)};
            if (extra_size > 0)
            {
                os << std::string_view{extra_begin, extra_begin + extra_size};
                if (fmt.width > extra_size)
                {
                    str_fmtting.width -= extra_size;
                }
            }
            to_stream(os, std::string_view{begin, end}, str_fmtting);
        }
        else
        {
            str_fmtting.fill = fmt.fill;
            to_stream(os, std::string_view{extra_begin, end}, str_fmtting);
        }

        return os;
    }

    export
    struct pointer_fmt
    {
        char fill {' '};
        align_type align {align_type::left};
        sign_type sign {sign_type::minus};
        std::size_t width {0};
        bool upper {false};

        constexpr static pointer_fmt parse(format_context& ctx)
        {
            pointer_fmt fmt;

            auto it {parse_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_sign(ctx, fmt.sign);
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);
            
            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'P':
                    fmt.upper = true;
                case 'p':
                    ++it;
                    break;
                }
            }

            ctx.advance_to(it);
            return fmt;
        }
    };

    export
    text_ostream& operator<<(text_ostream& os, const void* const val)
    {
        using T = std::uintptr_t;
        char buf[std::numeric_limits<T>::digits + 3];
        auto begin {to_chars_base16(buf, buf + std::size(buf), reinterpret_cast<T>(val))};
        *--begin = 'x';
        *--begin = '0';
        return os << std::string_view{begin, buf + std::size(buf)};
    }

    export
    text_ostream& operator<<(text_ostream& os, const volatile void* const val)
    {
        return os << const_cast<const void*>(val);
    }

    export
    text_ostream& operator<<(text_ostream& os, const std::nullptr_t)
    {
        return os << "nullptr";
    }

    export
    text_ostream& to_stream(text_ostream& os, const void* const value, const pointer_fmt& fmt)
    {
        const int_fmt int_fmtting {
            .fill=fmt.fill,
            .align=fmt.align,
            .sign=fmt.sign,
            .alt=true,
            .width=fmt.width,
            .base=16,
            .upper=fmt.upper
        };

        return to_stream(os, reinterpret_cast<std::uintptr_t>(value), int_fmtting);
    }

    export
    text_ostream& to_stream(text_ostream& os, std::string_view str, const str_fmt& fmt)
    {
        auto utf_view {views::utf_codepoints(str)};

        if (fmt.precision)
        {
            auto utf_it {utf_view.begin()};
            std::ranges::advance(utf_it, *fmt.precision, utf_view.end());
            const auto leftover {utf_it.leftover()};
            str.remove_prefix(str.size() - std::string_view{leftover.begin(), leftover.end()}.size());
        }

        utf_view = views::utf_codepoints(str);

        lib2::ostringstream debug_str;
        if (fmt.debug)
        {
            debug_str << escaped(str);
            str = debug_str.view();
        }

        const auto size {std::ranges::distance(utf_view)};

        if (fmt.width > size)
        {
            const auto padding {fmt.width - size};
            switch (fmt.align)
            {
            case align_type::left:
                os << str << io_fill(fmt.fill, padding);
                break;
            case align_type::right:
                os << io_fill(fmt.fill, padding) << str;
                break;
            case align_type::center:
                {
                    const auto left_pad {padding / 2};
                    const auto right_pad {padding - left_pad};

                    os << io_fill(fmt.fill, left_pad)
                        << str
                        << io_fill(fmt.fill, right_pad);
                }
                break;
            }
        }
        else
        {
            os << str;
        }

        return os;
    }

    export
    template<class Allocator>
    str_fmt format_of(const basic_ostringstream<char, std::char_traits<char>, Allocator>&);

    export
    template<class Allocator>
    text_ostream& to_stream(text_ostream& os, const std::basic_string<char, std::char_traits<char>, Allocator>& str, const str_fmt& fmt)
    {
        return to_stream(os, std::string_view{str}, fmt);
    }

    export
    text_ostream& to_stream(text_ostream& os, const char* const str, const str_fmt& fmt)
    {
        return to_stream(os, std::string_view{str}, fmt);
    }

    export
    template<class Allocator>
    text_ostream& to_stream(text_ostream& os, const basic_ostringstream<char, std::char_traits<char>, Allocator>& value, const str_fmt& fmt)
    {
        return to_stream(os, value.view(), fmt);
    }

    export
    struct char_fmt
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        unsigned int base {0};
        bool upper {false};
        bool use_locale {false};
        const std::locale* locale {nullptr};

        constexpr static char_fmt parse(format_context& ctx)
        {
            char_fmt fmt;

            auto it {parse_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);
            ctx.advance_to(it);
            it = parse_locale(ctx, fmt.use_locale);
            fmt.locale = ctx.locale();
            
            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'B':
                    fmt.upper = true;
                case 'b':
                    fmt.base = 2;
                    ++it;
                    break;
                case 'o':
                    fmt.base = 8;
                    ++it;
                    break;
                case 'd':
                    fmt.base = 10;
                    ++it;
                    break;
                case 'X':
                    fmt.upper = true;
                case 'x':
                    fmt.base = 16;
                    ++it;
                    break;
                }
            }

            ctx.advance_to(it);
            return fmt;
        }
    };

    export
    text_ostream& to_stream(text_ostream& os, const char value, const char_fmt& fmt)
    {
        if (fmt.base)
        {
            using un_t = std::make_unsigned_t<char>;
            const int_fmt int_fmtting {
                .fill=fmt.fill,
                .align=fmt.align,
                .width=fmt.width,
                .base=fmt.base,
                .upper=fmt.upper
            };

            return to_stream(os, static_cast<un_t>(value), int_fmtting);
        }
        
        if (fmt.width > 1)
        {
            const auto padding {fmt.width - 1};
            switch (fmt.align)
            {
            case align_type::left:
                os << value << io_fill(fmt.fill, padding);
                break;
            case align_type::right:
                os << io_fill(fmt.fill, padding) << value;
                break;
            case align_type::center:
                {
                    const auto left_pad {padding / 2};
                    const auto right_pad {padding - left_pad};

                    os << io_fill(fmt.fill, left_pad)
                        << value
                        << io_fill(fmt.fill, right_pad);
                }
                break;
            }
        }
        else
        {
            os << value;
        }

        return os;
    }

    export
    struct bool_fmt
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        unsigned int base {0};
        bool upper {false};
        bool use_locale {false};
        const std::locale* locale {nullptr};

        constexpr static bool_fmt parse(format_context& ctx)
        {
            bool_fmt fmt;

            auto it {parse_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);
            ctx.advance_to(it);
            it = parse_locale(ctx, fmt.use_locale);
            fmt.locale = ctx.locale();
            
            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'B':
                    fmt.upper = true;
                case 'b':
                    fmt.base = 2;
                    ++it;
                    break;
                case 'o':
                    fmt.base = 8;
                    ++it;
                    break;
                case 'd':
                    fmt.base = 10;
                    ++it;
                    break;
                case 'X':
                    fmt.upper = true;
                case 'x':
                    fmt.base = 16;
                    ++it;
                    break;
                }
            }

            ctx.advance_to(it);
            return fmt;
        }
    };

    export
    text_ostream& operator<<(text_ostream& os, const bool b)
    {
        if (b)
        {
            return os << "true";
        }

        return os << "false";
    }

    export
    text_ostream& to_stream(text_ostream& os, const bool value, const bool_fmt& fmt)
    {
        if (fmt.base)
        {
            using un_t = std::make_unsigned_t<char>;
            const int_fmt int_fmtting {
                .fill=fmt.fill,
                .align=fmt.align,
                .width=fmt.width,
                .base=fmt.base,
                .upper=fmt.upper
            };

            return to_stream(os, static_cast<un_t>(value), int_fmtting);
        }

        const str_fmt str_fmtting {
            .fill=fmt.fill,
            .align=fmt.align,
            .width=fmt.width
        };

        if (fmt.use_locale)
        {
            const auto loc {fmt.locale ? *fmt.locale : std::locale{}};
            const auto& facet {std::use_facet<std::numpunct<char>>(loc)};

            if (value)
            {
                return to_stream(os, facet.truename(), str_fmtting);
            }

            return to_stream(os, facet.falsename(), str_fmtting);
        }

        if (value)
        {
            return to_stream(os, "true", str_fmtting);
        }

        return to_stream(os, "false", str_fmtting);
    }

// ==============================================================================================================================
// Format strings
// ==============================================================================================================================
    void do_format(text_ostream& os, format_context& ctx)
    {
        constexpr std::string_view braces {"{}"};

        auto begin {ctx.begin()};
        while (begin != ctx.end())
        {
            auto iter {std::ranges::find_first_of(begin, ctx.end(), braces.begin(), braces.end())};
            
            if (iter == ctx.end())
            {
                os << std::string_view{begin, iter};
                break;
            }
            
            if (*iter == '}')
            {
                ++iter;
                if (iter == ctx.end() || *iter != '}')
                {
                    throw std::format_error{"Unmatched '}' in format string."}; 
                }

                os << std::string_view{begin, iter};
                ++iter;
                begin = iter;
                continue;
            }

            ++iter;
            if (iter == ctx.end())
            {
                throw std::format_error{"Unmatched '{' in format string."}; 
            }

            if (*iter == '{')
            {
                os << std::string_view{begin, iter};
                begin = iter;
                continue;
            }
            
            os << std::string_view{begin, iter - 1};

            std::size_t arg_id;
            
            switch (*iter)
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
                arg_id = parse_arg_id(iter, ctx.end());
                if (iter == ctx.end())
                {
                    throw std::format_error{"Unexpected end in format string"};
                }
                ctx.check_arg_id(arg_id);

                switch (*iter)
                {
                case ':':
                    ++iter;
                    if (iter == ctx.end())
                    {
                        throw std::format_error{"Unexpected end in format string"};
                    }
                case '}':
                    break;
                default:
                    throw std::format_error{"Unexpected character in format string"};
                }
                break;
            case ':':
                ++iter;
                if (iter == ctx.end())
                {
                    throw std::format_error{"Unexpected end in format string"};
                }
            case '}':
                arg_id = ctx.next_arg_id();
                break;
            default:
                throw std::format_error{"Unexpected character in format string"};
            }

            ctx.advance_to(iter);
            iter = ctx.begin();

            const auto arg {ctx.arg(arg_id)};
            arg.visit(lib2::overloaded {
                [](const std::monostate) {},
                [&](const bool value) {
                    if (*iter == '}')
                    {
                        os << value;
                    }
                    else
                    {
                        to_stream(os, value, bool_fmt::parse(ctx));
                    }
                },
                [&](const char value) {
                    if (*iter == '}')
                    {
                        os << value;
                    }
                    else
                    {
                        to_stream(os, value, char_fmt::parse(ctx));
                    }
                },
                [&](const std::intmax_t value) {
                    if (*iter == '}')
                    {
                        os << value;
                    }
                    else
                    {
                        to_stream(os, value, int_fmt::parse(ctx));
                    }
                },
                [&](const std::uintmax_t value) {
                    if (*iter == '}')
                    {
                        os << value;
                    }
                    else
                    {
                        to_stream(os, value, int_fmt::parse(ctx));
                    }
                },
                [&](const float value) {
                    if (*iter == '}')
                    {
                        os << value;
                    }
                    else
                    {
                        to_stream(os, value, float_fmt::parse(ctx));
                    }
                },
                [&](const double value) {
                    if (*iter == '}')
                    {
                        os << value;
                    }
                    else
                    {
                        to_stream(os, value, float_fmt::parse(ctx));
                    }
                },
                [&](const long double value) {
                    if (*iter == '}')
                    {
                        os << value;
                    }
                    else
                    {
                        to_stream(os, value, float_fmt::parse(ctx));
                    }
                },
                [&](const std::string_view value) {
                    if (*iter == '}')
                    {
                        os << value;
                    }
                    else
                    {
                        to_stream(os, value, str_fmt::parse(ctx));
                    }
                },
                [&](const void* const value) {
                    if (*iter == '}')
                    {
                        os << value;
                    }
                    else
                    {
                        to_stream(os, value, pointer_fmt::parse(ctx));
                    }
                },
                [&](const format_arg::handle& h) {
                    h.format(os, ctx);
                }
            });

            begin = ctx.begin();

            if (begin != ctx.end())
            {
                if (*begin != '}')
                {
                    throw std::format_error{std::format("Formatter at [{}] parsed incorrectly", arg_id)};
                }
                ++begin;
            }
        }
    }

    // template<class... Args, class CharT>
    //     requires(sizeof...(Args) == 0)
    // [[noreturn]] consteval typename basic_format_parse_context<CharT>::iterator
    // parse_format_compiletime(basic_format_parse_context<CharT>&, const std::size_t)
    // {
    //     throw std::format_error{"No format argument"};
    // }

    // template<class Arg, class... Args, class CharT>
    // consteval typename basic_format_parse_context<CharT>::iterator
    // parse_format_compiletime(basic_format_parse_context<CharT>& parse_ctx, const std::size_t arg_id)
    // {
    //     if (arg_id)
    //     {
    //         return parse_format_compiletime<Args...>(parse_ctx, arg_id - 1);
    //     }

    //     formatter<Arg, CharT> fmt;
    //     return fmt.parse(parse_ctx);
    // }

    // template<class... Args, class CharT>
    // consteval void do_format_compiletime(basic_format_parse_context<CharT>& parse_ctx)
    // {
    //     constexpr auto braces {STATICALLY_WIDEN(CharT, "{}")};

    //     auto begin {parse_ctx.begin()};
    //     while (begin != parse_ctx.end())
    //     {
    //         auto iter {std::ranges::find_first_of(begin, parse_ctx.end(), braces.begin(), braces.end())};
            
    //         if (iter == parse_ctx.end())
    //         {
    //             break;
    //         }
            
    //         if (*iter == '}')
    //         {
    //             ++iter;
    //             if (iter == parse_ctx.end() || *iter != '}')
    //             {
    //                 throw std::format_error{"Unmatched '}' in format string."}; 
    //             }

    //             ++iter;
    //             begin = iter;
    //             continue;
    //         }

    //         ++iter;
    //         if (iter == parse_ctx.end())
    //         {
    //             throw std::format_error{"Unmatched '{' in format string."}; 
    //         }

    //         if (*iter == '{')
    //         {
    //             begin = iter;
    //             continue;
    //         }

    //         std::size_t arg_id;
            
    //         switch (*iter)
    //         {
    //         case '0':
    //         case '1':
    //         case '2':
    //         case '3':
    //         case '4':
    //         case '5':
    //         case '6':
    //         case '7':
    //         case '8':
    //         case '9':
    //             iter = parse_num(iter, parse_ctx.end(), arg_id);
    //             if (iter == parse_ctx.end())
    //             {
    //                 throw std::format_error{"Unexpected end in format string"};
    //             }
    //             parse_ctx.check_arg_id(arg_id);

    //             switch (*iter)
    //             {
    //             case ':':
    //                 ++iter;
    //                 if (iter == parse_ctx.end())
    //                 {
    //                     throw std::format_error{"Unexpected end in format string"};
    //                 }
    //             case '}':
    //                 break;
    //             default:
    //                 throw std::format_error{"Unexpected character in format string"};
    //             }
    //             break;
    //         case ':':
    //             ++iter;
    //             if (iter == parse_ctx.end())
    //             {
    //                 throw std::format_error{"Unexpected end in format string"};
    //             }
    //         case '}':
    //             arg_id = parse_ctx.next_arg_id();
    //             break;
    //         default:
    //             throw std::format_error{"Unexpected character in format string"};
    //         }

    //         parse_ctx.advance_to_to(iter);
    //         begin = parse_format_compiletime<Args...>(parse_ctx, arg_id);

    //         if (begin != parse_ctx.end())
    //         {
    //             if (*begin != '}')
    //             {
    //                 throw std::format_error{std::format("Formatter at [{}] parsed incorrectly", arg_id)};
    //             }
    //             ++begin;
    //         }
    //     }
    // }

    export
    template<class... Args>
    struct format_string;

    struct runtime_format_string
    {
    public:
        runtime_format_string(const runtime_format_string&) = delete;
        runtime_format_string& operator=(const runtime_format_string&) = delete;

        friend runtime_format_string runtime_format(std::string_view fmt) noexcept;
    private:
        std::string_view str;

        runtime_format_string(std::string_view s) noexcept
            : str{s} {};

        template<class... Args>
        friend struct format_string;
    };

    export
    inline runtime_format_string runtime_format(const std::string_view fmt) noexcept
    {
        return {fmt};
    }

    export
    template<class... Args>
    struct format_string
    {
        // template<class Arg>
        // struct targ
        // {
        //     using type = std::remove_cvref_t<Arg>;
            
        //     static constexpr auto value {basic_format_arg<CharT>::fmt_type::handle_t};
        // };

        // template<class Arg>
        //     requires(requires{typename basic_format_arg<CharT>::template transform_arg<Arg>::type;})
        // struct targ<Arg> : public basic_format_arg<CharT>::template transform_arg<Arg> {};
    public:
        template<class T>
        consteval format_string(const T& s)
            : str{s}
        {
            // constexpr typename basic_format_arg<CharT>::fmt_type types[] {targ<Args>::value...};
            // compile_parse_context<CharT> parse_ctx{str, types};
            // do_format_compiletime<typename targ<Args>::type...>(parse_ctx);
        }

        format_string(const runtime_format_string rstr) noexcept
            : str{rstr.str} {}

        constexpr std::string_view get() const noexcept
        {
            return str;
        }
    private:
        std::string_view str;
    };

    export
    inline text_ostream& vformat_to(text_ostream& os, const std::string_view fmt, const format_args args)
    {
        format_context ctx {fmt, args};
        do_format(os, ctx);
        return os;
    }

    export
    inline text_ostream& vformat_to(text_ostream& os, const std::locale& loc, const std::string_view fmt, const format_args args)
    {
        format_context ctx {fmt, args, loc};
        do_format(os, ctx);
        return os;
    }

    export
    inline std::string vformat(const std::string_view fmt, const format_args args)
    {
        ostringstream ss;
        vformat_to(ss, fmt, args);
        return std::move(ss).str();
    }

    export
    inline std::string vformat(const std::locale& loc, const std::string_view fmt, const format_args args)
    {
        ostringstream ss;
        vformat_to(ss, loc, fmt, args);
        return std::move(ss).str();
    }

    export
    template<class... Args>
    inline text_ostream& format_to(text_ostream& os, const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        return vformat_to(os, fmt.get(), make_format_args(args...));
    }

    export
    template<class... Args>
    inline text_ostream& format_to(text_ostream& os, const std::locale& loc, const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        return vformat_to(os, loc, fmt.get(), make_format_args(args...));
    }

    export
    template<class... Args>
    inline std::string format(const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        return vformat(fmt.get(), make_format_args(args...));
    }

    export
    template<class... Args>
    inline std::string format(const std::locale& loc, const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        return vformat(loc, fmt.get(), make_format_args(args...));
    }

    export
    template<class... Args>
    inline std::size_t formatted_size(const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        lib2::size_ostream<char> sos;
        format_to(sos, fmt, args...);
        return sos.size();
    }

    export
    template<class... Args>
    inline std::size_t formatted_size(const std::locale& loc, const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        lib2::size_ostream<char> sos;
        format_to(sos, loc, fmt, args...);
        return sos.size();
    }
}