export module lib2.fmt:arithmetic;

import std;

import lib2.strings;
import lib2.io;

import :formatter;
import :context;
import :parsers;
import :format;
import :string;

namespace lib2
{
    template<std::unsigned_integral T>
    constexpr char* to_chars_base2(char* begin, char* end, const T val) noexcept
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
    constexpr char* to_chars_base8(char* const, char* end, T val) noexcept
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
    constexpr char* to_chars_base10(char* const, char* end, T val) noexcept
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
    constexpr char* to_chars_base16(char* const, char* end, T val) noexcept
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
    constexpr char* to_chars_base16_uppercase(char* const, char* end, T val) noexcept
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
    constexpr char* to_chars(char* const, char* end, T val, const unsigned int base) noexcept
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
    constexpr char* to_chars_uppercase(char* const, char* end, T val, const unsigned int base) noexcept
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
    char* to_chars(char* const begin, char* const end, const F val) noexcept
    {
        return std::to_chars(begin, end, val).ptr;
    }

    template<std::floating_point F>
    char* to_chars(char* const begin, char* const end, const F val, const std::chars_format fmt) noexcept
    {
        return std::to_chars(begin, end, val, fmt).ptr;
    }

    template<std::floating_point F>
    char* to_chars(char* const begin, char* const end, const F val, const std::chars_format fmt, const int precision) noexcept
    {
        return std::to_chars(begin, end, val, fmt, precision).ptr;
    }

    export
    template<std::integral T>
        requires(!character<T> && !std::same_as<T, bool>)
    struct formatter<T> : public width_parser
    {
        char fill {' '};
        fill_align_parser::align_type align {fill_align_parser::align_type::right};
        sign_parser::sign_type sign {sign_parser::sign_type::minus};
        bool alt {false};
        bool zero {false};
        unsigned int base {10};
        bool upper {false};
        bool use_locale {false};

        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};

            ctx.begin = fill_align_parser::parse(ctx, fill, align);
            ctx.begin = sign_parser::parse(ctx, sign);
            ctx.begin = alt_parser::parse(ctx, alt, zero);
            ctx.begin = width_parser::parse(ctx);
            ctx.begin = locale_parser::parse(ctx, use_locale);
            
            if (ctx.begin != ctx.end)
            {
                switch (*ctx.begin)
                {
                case 'B':
                    upper = true;
                case 'b':
                    base = 2;
                    ++ctx.begin;
                    break;
                case 'o':
                    base = 8;
                    ++ctx.begin;
                    break;
                case 'd':
                    base = 10;
                    ++ctx.begin;
                    break;
                case 'X':
                    upper = true;
                case 'x':
                    base = 16;
                    ++ctx.begin;
                    break;
                }
            }

            return ctx.begin;
        }

        void format(T val, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            char buf[std::numeric_limits<T>::digits + 4];
            auto begin {buf + 3};
            auto extra_begin {begin};
            bool neg {false};

            switch (base)
            {
            case 2:
                if constexpr (std::signed_integral<T>)
                {
                    if ((neg = val < 0))
                    {
                        val = -val;
                    }
                }
                
                extra_begin = begin = to_chars_base2(begin, std::ranges::end(buf), static_cast<std::make_unsigned_t<T>>(val));

                if (alt)
                {
                    *--extra_begin = upper ? 'B' : 'b';
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
                
                extra_begin = begin = to_chars_base8(begin, std::ranges::end(buf), static_cast<std::make_unsigned_t<T>>(val));

                if (alt && val != 0)
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
                
                extra_begin = begin = to_chars_base10(begin, std::ranges::end(buf), static_cast<std::make_unsigned_t<T>>(val));
                break;
            case 16:
                if constexpr (std::signed_integral<T>)
                {
                    if ((neg = val < 0))
                    {
                        val = -val;
                    }
                }
                
                if (upper)
                {
                    extra_begin = begin = to_chars_base16_uppercase(begin, std::ranges::end(buf), static_cast<std::make_unsigned_t<T>>(val));
                }
                else
                {
                    extra_begin = begin = to_chars_base16(begin, std::ranges::end(buf), static_cast<std::make_unsigned_t<T>>(val));
                }

                if (alt)
                {
                    *--extra_begin = upper ? 'X' : 'x';
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

                if (upper)
                {
                    extra_begin = begin = to_chars_uppercase(begin, std::ranges::end(buf), static_cast<std::make_unsigned_t<T>>(val), base);
                }
                else
                {
                    extra_begin = begin = to_chars(begin, std::ranges::end(buf), static_cast<std::make_unsigned_t<T>>(val), base);
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
                    switch (sign)
                    {
                    case sign_parser::sign_type::plus:
                    case sign_parser::sign_type::space:
                        *--extra_begin = std::to_underlying(sign);
                    }
                }
            }
            else
            {
                switch (sign)
                {
                case sign_parser::sign_type::plus:
                case sign_parser::sign_type::space:
                    *--extra_begin = std::to_underlying(sign);
                }
            }

            formatter<std::string_view> str_fmt;
            str_fmt.fill  = fill;
            str_fmt.align = align;
            str_fmt.set_width(width);
            
            if (zero)
            {
                str_fmt.fill = '0';
                const auto extra_size {static_cast<std::size_t>(begin - extra_begin)};
                if (extra_size > 0)
                {
                    ctx.stream.write(extra_begin, extra_size);
                    if (width > extra_size)
                    {
                        str_fmt.set_width(width - extra_size);
                    }
                }
                str_fmt.format(std::string_view{begin, std::ranges::end(buf)}, ctx);
            }
            else
            {
                str_fmt.format(std::string_view{extra_begin, std::ranges::end(buf)}, ctx);
            }
        }

        static void default_format(const T val, format_context& ctx)
        {
            char buf[std::numeric_limits<T>::digits10 + 1 + std::signed_integral<T>];
            char* begin;

            if constexpr (std::unsigned_integral<T>)
            {
                begin = to_chars_base10(buf, std::ranges::end(buf), val);
            }
            else
            {
                if (val < 0)
                {
                    begin = to_chars_base10(buf, std::ranges::end(buf), std::make_unsigned_t<T>(-val));
                    *--begin = '-';
                }
                else
                {
                    begin = to_chars_base10(buf, std::ranges::end(buf), std::make_unsigned_t<T>(val));
                }
            }

            ctx.stream.write(std::string_view{begin, std::ranges::end(buf)});
        }
    };

    export
    template<std::floating_point F>
    struct formatter<F> : public width_parser, public precision_parser
    {
        char fill {' '};
        fill_align_parser::align_type align {fill_align_parser::align_type::left};
        sign_parser::sign_type sign {sign_parser::sign_type::minus};
        bool alt {false};
        bool zero {false};
        std::optional<std::chars_format> type {};
        bool upper {false};
        bool use_locale {false};

        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};

            ctx.begin = fill_align_parser::parse(ctx, fill, align);
            ctx.begin = sign_parser::parse(ctx, sign);
            ctx.begin = alt_parser::parse(ctx, alt, zero);
            ctx.begin = width_parser::parse(ctx);
            ctx.begin = precision_parser::parse(ctx);
            ctx.begin = locale_parser::parse(ctx, use_locale);
            
            if (ctx.begin != ctx.end)
            {
                switch (*ctx.begin)
                {
                case 'A':
                    upper = true;
                case 'a':
                    type = std::chars_format::hex;
                    ++ctx.begin;
                    break;
                case 'E':
                    upper = true;
                case 'e':
                    type = std::chars_format::scientific;
                    ++ctx.begin;
                    break;
                case 'F':
                    upper = true;
                case 'f':
                    type = std::chars_format::fixed;
                    ++ctx.begin;
                    break;
                case 'g':
                    upper = true;
                case 'G':
                    type = std::chars_format::general;
                    ++ctx.begin;
                    break;
                }
            }

            return ctx.begin;
        }

        void format(F val, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};
            const auto precision {this->get_precision(ctx)};

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

            if (type)
            {
                if (precision)
                {
                    end = lib2::to_chars(begin, std::ranges::end(buf), val, *type, *precision);
                }
                else
                {
                    end = lib2::to_chars(begin, std::ranges::end(buf), val, *type);
                }
            }
            else
            {
                if (precision)
                {
                    end = lib2::to_chars(begin, std::ranges::end(buf), val, std::chars_format::general, *precision);
                }
                else
                {
                    end = lib2::to_chars(begin, std::ranges::end(buf), val);
                }
            }

            if (neg)
            {
                *--extra_begin = '-';
            }
            else
            {
                switch (sign)
                {
                case sign_parser::sign_type::plus:
                case sign_parser::sign_type::space:
                    *--extra_begin = std::to_underlying(sign);
                    break;
                }
            }

            formatter<std::string_view> str_fmt;
            str_fmt.fill  = fill;
            str_fmt.align = align;
            str_fmt.set_width(width);
            
            if (zero)
            {
                str_fmt.fill = '0';
                const auto extra_size {static_cast<std::size_t>(begin - extra_begin)};
                if (extra_size > 0)
                {
                    ctx.stream.write(extra_begin, extra_size);
                    if (width > extra_size)
                    {
                        str_fmt.set_width(width - extra_size);
                    }
                }
                str_fmt.format(std::string_view{begin, end}, ctx);
            }
            else
            {
                str_fmt.format(std::string_view{extra_begin, end}, ctx);
            }
        }

        static void default_format(const F val, format_context& ctx)
        {
            char buf[30];
            const auto ptr {to_chars(buf, std::ranges::end(buf), val)};
            ctx.stream.write(std::string_view{buf, ptr});
        }
    };

    export
    template<>
    struct formatter<const void*> : public width_parser
    {
        char fill {' '};
        fill_align_parser::align_type align {fill_align_parser::align_type::left};
        sign_parser::sign_type sign {sign_parser::sign_type::minus};
        bool upper {false};

        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};

            ctx.begin = fill_align_parser::parse(ctx, fill, align);
            ctx.begin = sign_parser::parse(ctx, sign);
            ctx.begin = width_parser::parse(ctx);
            
            if (ctx.begin != ctx.end)
            {
                switch (*ctx.begin)
                {
                case 'P':
                    upper = true;
                case 'p':
                    ++ctx.begin;
                    break;
                }
            }

            return ctx.begin;
        }

        void format(const void* const value, format_context& ctx) const
        {
            formatter<std::uintptr_t> int_fmt;
            int_fmt.fill  = fill;
            int_fmt.align = align;
            int_fmt.sign  = sign;
            int_fmt.alt   = true;
            int_fmt.set_width(this->get_width(ctx));
            int_fmt.base  = 16;
            int_fmt.upper = upper;

            int_fmt.format(reinterpret_cast<std::uintptr_t>(value), ctx);
        }

        static void default_format(const void* const value, format_context& ctx)
        {
            char buf[std::numeric_limits<std::uintptr_t>::digits + 3];
            auto begin {to_chars_base16(buf, std::ranges::end(buf), reinterpret_cast<std::uintptr_t>(value))};
            *--begin = 'x';
            *--begin = '0';
            ctx.stream.write(std::string_view{begin, std::ranges::end(buf)});
        }
    };

    export
    template<>
    struct formatter<void*> : public formatter<const void*> {};

    export
    template<>
    struct formatter<const volatile void*> : public formatter<const void*>
    {
        void format(const volatile void* const value, format_context& ctx)
        {
            return formatter<const void*>::format(const_cast<const void*>(value), ctx);
        }

        static void default_format(const volatile void* const value, format_context& ctx)
        {
            return formatter<const void*>::default_format(const_cast<const void*>(value), ctx);
        }
    };

    export
    template<>
    struct formatter<volatile void*> : public formatter<const volatile void*> {};

    export
    template<>
    struct formatter<std::nullptr_t> : public formatter<const void*>
    {
        void format(const std::nullptr_t value, format_context& ctx)
        {
            return formatter<const void*>::format(nullptr, ctx);
        }

        static void default_format(const std::nullptr_t value, format_context& ctx)
        {
            ctx.stream.write("nullptr");
        }
    };

    export
    template<>
    struct formatter<char> : public width_parser
    {
        char fill {' '};
        fill_align_parser::align_type align {fill_align_parser::align_type::left};
        unsigned int base {0};
        bool upper {false};
        bool use_locale {false};

        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};

            ctx.begin = fill_align_parser::parse(ctx, fill, align);
            ctx.begin = width_parser::parse(ctx);
            ctx.begin = locale_parser::parse(ctx, use_locale);
            
            if (ctx.begin != ctx.end)
            {
                switch (*ctx.begin)
                {
                case 'B':
                    upper = true;
                case 'b':
                    base = 2;
                    ++ctx.begin;
                    break;
                case 'o':
                    base = 8;
                    ++ctx.begin;
                    break;
                case 'd':
                    base = 10;
                    ++ctx.begin;
                    break;
                case 'X':
                    upper = true;
                case 'x':
                    base = 16;
                    ++ctx.begin;
                    break;
                }
            }

            return ctx.begin;
        }

        void format(const char value, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            if (base)
            {
                using un_t = std::make_unsigned_t<char>;

                formatter<un_t> int_fmt;
                int_fmt.fill  = fill;
                int_fmt.align = align;
                int_fmt.set_width(width);
                int_fmt.base  = base;
                int_fmt.upper = upper;

                int_fmt.format(static_cast<un_t>(value), ctx);
            }
            else if (width > 1)
            {
                const auto padding {width - 1};
                switch (align)
                {
                case fill_align_parser::align_type::left:
                    ctx.stream.put(value);
                    ctx.stream.fill(fill, padding);
                    break;
                case fill_align_parser::align_type::right:
                    ctx.stream.fill(fill, padding);
                    ctx.stream.put(value);
                    break;
                case fill_align_parser::align_type::center:
                    {
                        const auto left_pad {padding / 2};
                        const auto right_pad {padding - left_pad};

                        ctx.stream.fill(fill, left_pad);
                        ctx.stream.put(value);
                        ctx.stream.fill(fill, right_pad);
                    }
                    break;
                }
            }
            else
            {
                ctx.stream.put(value);
            }
        }

        static void default_format(const char value, format_context& ctx)
        {
            ctx.stream.put(value);
        }
    };
}