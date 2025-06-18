#pragma once

namespace lib2
{
    template<class CharT>
    struct base2_lookup;

    template<>
    struct base2_lookup<char>
    {
        static const char value[16][5];
    };

    template<>
    struct base2_lookup<wchar_t>
    {
        static const wchar_t value[16][5];
    };

    template<>
    struct base2_lookup<char8_t>
    {
        static const char8_t value[16][5];
    };

    template<>
    struct base2_lookup<char16_t>
    {
        static const char16_t value[16][5];
    };
    
    template<>
    struct base2_lookup<char32_t>
    {
        static const char32_t value[16][5];
    };
    
    template<character CharT, std::unsigned_integral T>
    constexpr CharT* to_chars_base2(CharT* begin, CharT* end, const T val) noexcept
    {
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
            std::copy_n(base2_lookup<CharT>::value[(p[byte_size - i - 1] & 0xF0) >> 4], N, &begin[i * 8]);
            std::copy_n(base2_lookup<CharT>::value[(p[byte_size - i - 1] & 0x0F)], N, &begin[i * 8 + N]);
        }

        return begin + std::countl_zero(val);
    }

    template<character CharT, std::unsigned_integral T>
    constexpr CharT* to_chars_base8(CharT*, CharT* end, T val) noexcept
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

    template<class CharT>
    struct base10_lookup;

    template<>
    struct base10_lookup<char>
    {
        static const char value[100][3];
    };

    template<>
    struct base10_lookup<wchar_t>
    {
        static const wchar_t value[100][3];
    };

    template<>
    struct base10_lookup<char8_t>
    {
        static const char8_t value[100][3];
    };

    template<>
    struct base10_lookup<char16_t>
    {
        static const char16_t value[100][3];
    };

    template<>
    struct base10_lookup<char32_t>
    {
        static const char32_t value[100][3];
    };

    template<character CharT, std::unsigned_integral T>
    constexpr CharT* to_chars_base10(CharT*, CharT* end, T val) noexcept
    {
        while (val >= 100)
        {
            const auto remainder {val % 100};
            val /= 100;
            end -= 2;
            end[0] = base10_lookup<CharT>::value[remainder][0];
            end[1] = base10_lookup<CharT>::value[remainder][1];
        }

        if (val < 10)
        {
            *--end = '0' + val;
        }
        else
        {
            end -= 2;
            end[0] = base10_lookup<CharT>::value[val][0];
            end[1] = base10_lookup<CharT>::value[val][1];
        }

        return end;
    }

    template<character CharT, std::unsigned_integral T>
    constexpr CharT* to_chars_base16(CharT*, CharT* end, T val) noexcept
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

    template<character CharT, std::unsigned_integral T>
    constexpr CharT* to_chars_base16_uppercase(CharT*, CharT* end, T val) noexcept
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

    template<character CharT, std::unsigned_integral T>
    constexpr CharT* to_chars(CharT*, CharT* end, T val, const unsigned int base) noexcept
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

    template<character CharT, std::unsigned_integral T>
    constexpr CharT* to_chars_uppercase(CharT*, CharT* end, T val, const unsigned int base) noexcept
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

    template<character CharT, std::floating_point F>
    CharT* to_chars(CharT* const begin, CharT* const, const F val) noexcept
    {
        std::array<char, 30> buf;
        const auto ptr {lib2::to_chars(buf.data(), buf.data() + buf.size(), val)};
        return std::copy(buf.data(), ptr, begin);
    }

    template<class CharT, std::floating_point F>
    CharT* to_chars(CharT* const begin, CharT* const, const F val, const std::chars_format fmt) noexcept
    {
        std::array<char, 30> buf;
        const auto ptr {lib2::to_chars(buf.data(), buf.data() + buf.size(), val, fmt)};
        return std::copy(buf.data(), ptr, begin);
    }

    template<class CharT, std::floating_point F>
    CharT* to_chars(CharT* const begin, CharT* const, const F val, const std::chars_format fmt, const int precision) noexcept
    {
        std::array<char, 30> buf;
        const auto ptr {lib2::to_chars(buf.data(), buf.data() + buf.size(), val, fmt, precision)};
        return std::copy(buf.data(), ptr, begin);
    }

    export
    template<class CharT, std::unsigned_integral T>
        requires(!lib2::character<T> && !std::same_as<T, bool>)
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const T val)
    {
        std::array<CharT, std::numeric_limits<T>::digits10 + 1> buf;
        const auto begin {to_chars_base10(buf.data(), buf.data() + buf.size(), val)};
        return os << std::basic_string_view<CharT>{begin, buf.data() + buf.size()};
    }

    export
    template<class CharT, std::signed_integral T>
        requires(!lib2::character<T> && !std::same_as<T, bool>)
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const T val)
    {
        std::array<CharT, std::numeric_limits<T>::digits10 + 2> buf;
        CharT* begin;

        if (val < 0)
        {
            begin = to_chars_base10(buf.data(), buf.data() + buf.size(), std::make_unsigned_t<T>(-val));
            *--begin = '-';
        }
        else
        {
            begin = to_chars_base10(buf.data(), buf.data() + buf.size(), std::make_unsigned_t<T>(val));
        }

        return os << std::basic_string_view<CharT>{begin, buf.data() + buf.size()};
    }

    export
    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const void* const val)
    {
        using T = std::uintptr_t;
        std::array<CharT, std::numeric_limits<T>::digits + 3> buf;
        auto begin {to_chars_base16(buf.data(), buf.data() + buf.size(), reinterpret_cast<T>(val))};
        *--begin = 'x';
        *--begin = '0';
        return os << std::basic_string_view<CharT>{begin, buf.data() + buf.size()};
    }

    export
    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const volatile void* const val)
    {
        return os << const_cast<const void*>(val);
    }

    export
    template<class CharT, std::floating_point F>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const F val)
    {
        std::array<CharT, 30> buf;
        const auto ptr {to_chars(buf.data(), buf.data() + buf.size(), val)};
        return os << std::basic_string_view<CharT>{buf.data(), ptr};
    }

    export
    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const std::nullptr_t)
    {
        return os << STATICALLY_WIDEN(CharT, "nullptr");
    }

    export
    template<std::integral T, class CharT>
        requires(!character<T> && !std::same_as<T, bool>)
    struct formatter<T, CharT> : public fill_align_parser<CharT>, public sign_parser<CharT>, public alt_parser<CharT>, public width_parser<CharT>
    {
    public:
        [[nodiscard]] constexpr unsigned int get_base() const noexcept
        {
            return base;
        }

        constexpr void set_base(unsigned int b)
        {
            if (b < 2 || b > 36)
            {
                throw std::invalid_argument{"Base must be between 2 and 35"};
            }

            base = b;
        }

        [[nodiscard]] constexpr bool is_upper_format() const noexcept
        {
            return upper;
        }

        constexpr void set_upper_format(const bool u = true) noexcept
        {
            upper = u;
        }

        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            auto it {fill_align_parser<CharT>::parse(ctx)};
            ctx.advance_to(it);
            it = sign_parser<CharT>::parse(ctx);
            ctx.advance_to(it);
            it = alt_parser<CharT>::parse(ctx);
            ctx.advance_to(it);
            it = width_parser<CharT>::parse(ctx);

            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'B':
                    upper = true;
                case 'b':
                    base = 2;
                    ++it;
                    break;
                case 'o':
                    base = 8;
                    ++it;
                    break;
                case 'd':
                    base = 10;
                    ++it;
                    break;
                case 'X':
                    upper = true;
                case 'x':
                    base = 16;
                    ++it;
                    break;
                }
            }
            
            return it;
        }

        void format(T val, basic_format_context<CharT>& ctx) const
        {
            std::array<CharT, std::numeric_limits<T>::digits + 4> buf;
            auto begin {buf.data() + 3};
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
                
                extra_begin = begin = to_chars_base2(begin, buf.data() + buf.size(), static_cast<std::make_unsigned_t<T>>(val));

                if (this->is_alt_format())
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
                
                extra_begin = begin = to_chars_base8(begin, buf.data() + buf.size(), static_cast<std::make_unsigned_t<T>>(val));

                if (this->is_alt_format() && val != 0)
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
                
                extra_begin = begin = to_chars_base10(begin, buf.data() + buf.size(), static_cast<std::make_unsigned_t<T>>(val));
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
                    extra_begin = begin = to_chars_base16_uppercase(begin, buf.data() + buf.size(), static_cast<std::make_unsigned_t<T>>(val));
                }
                else
                {
                    extra_begin = begin = to_chars_base16(begin, buf.data() + buf.size(), static_cast<std::make_unsigned_t<T>>(val));
                }

                if (this->is_alt_format())
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
                    extra_begin = begin = to_chars_uppercase(begin, buf.data() + buf.size(), static_cast<std::make_unsigned_t<T>>(val), base);
                }
                else
                {
                    extra_begin = begin = to_chars(begin, buf.data() + buf.size(), static_cast<std::make_unsigned_t<T>>(val), base);
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
                    switch (this->get_sign())
                    {
                    case sign_parser<CharT>::sign_type::plus:
                    case sign_parser<CharT>::sign_type::space:
                        *--extra_begin = std::to_underlying(this->get_sign());
                    }
                }
            }
            else
            {
                switch (this->get_sign())
                {
                case sign_parser<CharT>::sign_type::plus:
                case sign_parser<CharT>::sign_type::space:
                    *--extra_begin = std::to_underlying(this->get_sign());
                }
            }

            formatter<std::basic_string_view<CharT>, CharT> str_fmt;
            const auto width {this->get_width(ctx)};
            str_fmt.set_align(this->get_align());
            
            if (this->is_zero_format())
            {
                str_fmt.set_fill('0');
                const auto extra_size {static_cast<std::size_t>(begin - extra_begin)};
                if (extra_size > 0)
                {
                    ctx.stream() << std::basic_string_view<CharT>{extra_begin, extra_begin + extra_size};
                    if (width > extra_size)
                    {
                        str_fmt.set_width(width - extra_size);
                    }
                    else
                    {
                        str_fmt.set_width(width);
                    }
                }
                return str_fmt.format(std::basic_string_view<CharT>{begin, buf.data() + buf.size()}, ctx);
            }

            str_fmt.set_fill(this->get_fill());
            str_fmt.set_width(width);
            return str_fmt.format(std::basic_string_view<CharT>{extra_begin, buf.data() + buf.size()}, ctx);
        }
    private:
        unsigned int base {10};
        bool upper {false};
    };

    export
    template<std::floating_point T, class CharT>
    struct formatter<T, CharT> : public fill_align_parser<CharT>, public sign_parser<CharT>, public alt_parser<CharT>, public width_parser<CharT>, public precision_parser<CharT>
    {
    public:
        [[nodiscard]] constexpr std::optional<std::chars_format> get_type() const noexcept
        {
            return type;
        }

        constexpr void set_type(const std::chars_format t) noexcept
        {
            type = t;
        }

        constexpr void set_no_type() noexcept
        {
            type.reset();
        }

        [[nodiscard]] constexpr bool is_upper_format() const noexcept
        {
            return upper;
        }

        constexpr void set_upper_format(const bool u = true) noexcept
        {
            upper = u;
        }

        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            auto it {fill_align_parser<CharT>::parse(ctx)};
            ctx.advance_to(it);
            it = sign_parser<CharT>::parse(ctx);
            ctx.advance_to(it);
            it = alt_parser<CharT>::parse(ctx);
            ctx.advance_to(it);
            it = width_parser<CharT>::parse(ctx);
            ctx.advance_to(it);
            it = precision_parser<CharT>::parse(ctx);

            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'A':
                    upper = true;
                case 'a':
                    type = std::chars_format::hex;
                    ++it;
                    break;
                case 'E':
                    upper = true;
                case 'e':
                    type = std::chars_format::scientific;
                    ++it;
                    break;
                case 'F':
                    upper = true;
                case 'f':
                    type = std::chars_format::fixed;
                    ++it;
                    break;
                case 'G':
                    upper = true;
                case 'g':
                    type = std::chars_format::general;
                    ++it;
                    break;
                }
            }
            
            return it;
        }

        void format(T val, basic_format_context<CharT>& ctx) const
        {
            constexpr std::size_t extra {1};
            std::array<CharT, 29 + extra> buf;
            CharT* begin {buf.data() + extra};
            CharT* extra_begin {begin};
            CharT* end;
            bool neg;

            if ((neg = val < 0))
            {
                val = -val;
            }

            const auto precision {this->get_precision(ctx)};

            if (type)
            {
                if (precision)
                {
                    end = lib2::to_chars(begin, buf.data() + buf.size(), val, *type, *precision);
                }
                else
                {
                    end = lib2::to_chars(begin, buf.data() + buf.size(), val, *type);
                }
            }
            else
            {
                if (precision)
                {
                    end = lib2::to_chars(begin, buf.data() + buf.size(), val, std::chars_format::general, *precision);
                }
                else
                {
                    end = lib2::to_chars(begin, buf.data() + buf.size(), val);
                }
            }

            if (neg)
            {
                *--extra_begin = '-';
            }
            else
            {
                switch (this->get_sign())
                {
                case sign_parser<CharT>::sign_type::plus:
                case sign_parser<CharT>::sign_type::space:
                    *--extra_begin = std::to_underlying(this->get_sign());
                    break;
                }
            }

            const auto width {this->get_width(ctx)};
            formatter<std::basic_string_view<CharT>, CharT> str_fmt;
            str_fmt.set_align(this->get_align());
            
            if (this->is_zero_format())
            {
                str_fmt.set_fill('0');
                const auto extra_size {static_cast<std::size_t>(begin - extra_begin)};
                if (extra_size > 0)
                {
                    ctx.stream() << std::basic_string_view<CharT>{extra_begin, extra_begin + extra_size};
                    if (width > extra_size)
                    {
                        str_fmt.set_width(width - extra_size);
                    }
                    else
                    {
                        str_fmt.set_width(width);
                    }
                }
                return str_fmt.format(std::basic_string_view<CharT>{begin, end}, ctx);
            }

            str_fmt.set_fill(this->get_fill());
            str_fmt.set_width(width);
            return str_fmt.format(std::basic_string_view<CharT>{extra_begin, end}, ctx);
        }
    private:
        std::optional<std::chars_format> type;
        bool upper {false};
    };

    export
    template<class CharT>
    struct formatter<const void*, CharT> : public formatter<std::uintptr_t, CharT>
    {
        constexpr formatter() noexcept
        {
            this->set_base(16);
            this->set_alt_format();
        }

        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            auto it {fill_align_parser<CharT>::parse(ctx)};
            ctx.advance_to(it);
            it = sign_parser<CharT>::parse(ctx);
            ctx.advance_to(it);
            it = width_parser<CharT>::parse(ctx);

            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'p':
                    break;
                case 'P':
                    this->set_upper_format();
                    break;
                }
            }

            return it;
        }

        void format(const void* const val, basic_format_context<CharT>& ctx) const
        {
            return formatter<std::uintptr_t, CharT>::format(reinterpret_cast<std::uintptr_t>(val), ctx);
        }
    };

    export
    template<class CharT>
    struct formatter<void*, CharT> : public formatter<const void*, CharT> {};
}