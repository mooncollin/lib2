#pragma once

namespace lib2
{
    template<std::ranges::view V>
    struct escaped_t
    {
        V base;
    };

    export
    template<std::ranges::input_range R>
    constexpr auto escaped(R&& r)
    {
        return escaped_t{std::ranges::views::all(std::forward<R>(r))};
    }

    export
    template<class CharT, class V>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const escaped_t<V>& e)
    {
        basic_format_context<CharT> dummy_ctx {os, make_format_args<CharT>()};

        constexpr auto codepoint_formatter {[] {
            formatter<std::uint32_t, CharT> fmt;
            fmt.set_base(16);
            return fmt;
        }()};

        constexpr auto invalid_formatter{[] {
            formatter<std::uint32_t, CharT> fmt;
            fmt.set_base(16);
            fmt.set_align(fill_align_parser<CharT>::align_type::right);
            fmt.set_fill('0');
            fmt.set_width(2);
            return fmt;
        }()};

        os.put('"');

        const auto utf_view {views::utf_codepoints(e.base)};
        for (auto it {utf_view.begin()}; it != utf_view.end(); ++it)
        {
            switch (*it)
            {
            case '\t':
                os << STATICALLY_WIDEN(CharT, "\\t");
                break;
            case '\n':
                os << STATICALLY_WIDEN(CharT, "\\n");
                break;
            case '\r':
                os << STATICALLY_WIDEN(CharT, "\\r");
                break;
            case '"':
                os << STATICALLY_WIDEN(CharT, "\\\"");
                break;
            case '\\':
                os << STATICALLY_WIDEN(CharT, "\\\\");
                break;
            default:
                if (it.is_invalid())
                {
                    os << STATICALLY_WIDEN(CharT, "\\x");
                    invalid_formatter.format(*it, dummy_ctx);
                }
                else if (*it <= 0x7F)
                {
                    os << static_cast<CharT>(*it);
                }
                else
                {
                    os << STATICALLY_WIDEN(CharT, "\\\u");
                    codepoint_formatter.format(*it, dummy_ctx);
                }
            }
        }

        os.put('"');
        return os;
    }

    export
    template<class CharT, class Traits>
    struct formatter<std::basic_string_view<CharT, Traits>, CharT> : public fill_align_parser<CharT>, public width_parser<CharT>, public precision_parser<CharT>
    {
    public:
        [[nodiscard]] constexpr bool is_debug_format() const noexcept
        {
            return debug;
        }

        constexpr void set_debug_format(const bool d = true) noexcept
        {
            debug = d;
        }

        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            debug = false;

            auto it {fill_align_parser<CharT>::parse(ctx)};
            ctx.advance_to(it);
            it = width_parser<CharT>::parse(ctx);
            ctx.advance_to(it);
            it = precision_parser<CharT>::parse(ctx);
            if (it != ctx.end())
            {
                switch (*it)
                {
                case 's':
                    ++it;
                    break;
                case '?':
                    debug = true;
                    ++it;
                    break;
                }
            }

            return it;
        }

        void format(std::basic_string_view<CharT, Traits> str, basic_format_context<CharT>& ctx) const
        {
            do_format(str, ctx);
        }
    protected:
        template<class Allocator = std::allocator<CharT>>
        void do_format(std::basic_string_view<CharT, Traits> str, basic_format_context<CharT>& ctx, const Allocator a = {}) const
        {
            auto utf_view {views::utf_codepoints(str)};
            const auto precision {this->get_precision(ctx)};

            if (precision)
            {
                auto utf_it {utf_view.begin()};
                std::ranges::advance(utf_it, *precision, utf_view.end());
                const auto leftover {utf_it.leftover()};
                str.remove_prefix(str.size() - std::basic_string_view<CharT, Traits>{leftover.begin(), leftover.end()}.size());
            }

            utf_view = views::utf_codepoints(str);

            lib2::basic_ostringstream<CharT, Traits, Allocator> debug_str{a};
            if (debug)
            {
                debug_str << escaped(str);
                str = debug_str.view();
            }

            const auto size {std::ranges::distance(utf_view)};
            const auto width {this->get_width(ctx)};
            auto& os {ctx.stream()};

            if (width > size)
            {
                const auto padding {width - size};
                switch (this->get_align())
                {
                case fill_align_parser<CharT>::align_type::left:
                    os << str << io_fill(this->get_fill(), padding);
                    break;
                case fill_align_parser<CharT>::align_type::right:
                    os << io_fill(this->get_fill(), padding) << str;
                    break;
                case fill_align_parser<CharT>::align_type::center:
                    {
                        const auto left_pad {padding / 2};
                        const auto right_pad {padding - left_pad};

                        os << io_fill(this->get_fill(), left_pad)
                           << str
                           << io_fill(this->get_fill(), right_pad);
                    }
                    break;
                }
            }
            else
            {
                os << str;
            }
        }
    private:
        bool debug {false};
    };

    export
    template<class CharT>
    struct formatter<const CharT*, CharT> : public formatter<std::basic_string_view<CharT>, CharT>
    {
        void format(const CharT* const val, basic_format_context<CharT>& ctx) const
        {
            return formatter<std::basic_string_view<CharT>, CharT>::format(val, ctx);
        }
    };

    export
    template<class CharT>
    struct formatter<CharT*, CharT> : public formatter<const CharT*, CharT> {};

    export
    template<class CharT, class Traits, class Allocator>
    struct formatter<std::basic_string<CharT, Traits, Allocator>, CharT> : public formatter<std::basic_string_view<CharT>, CharT>
    {
        void format(const std::basic_string<CharT, Traits, Allocator>& str, basic_format_context<CharT>& ctx) const
        {
            this->do_format(str, ctx, str.get_allocator());
        }
    };

    export
    template<io_character CharT>
    struct formatter<CharT, CharT> : public fill_align_parser<CharT>, public width_parser<CharT>
    {
        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            auto it {fill_align_parser<CharT>::parse(ctx)};
            ctx.advance_to(it);
            it = width_parser<CharT>::parse(ctx);

            return it;
        }

        void format(const CharT val, basic_format_context<CharT>& ctx) const
        {
            const auto width {this->get_width(ctx)};
            if (width > 1)
            {
                formatter<std::basic_string_view<CharT>, CharT> string_fmt;
                string_fmt.set_fill(this->get_fill());
                string_fmt.set_align(this->get_align());
                string_fmt.set_width(width);

                return string_fmt.format({&val, 1}, ctx);
            }
            
            ctx.stream().put(val);
        }
    };

    export
    template<>
    struct formatter<char, wchar_t> : public formatter<wchar_t, wchar_t>
    {
        void format(const char val, wformat_context& ctx) const
        {
            return formatter<wchar_t, wchar_t>::format(static_cast<wchar_t>(static_cast<unsigned char>(val)), ctx);
        }
    };

    export
    template<class CharT, class Traits, class Allocator>
    struct formatter<basic_lazy_string<CharT, Traits, Allocator>, CharT> : public formatter<std::basic_string_view<CharT, Traits>, CharT>
    {
        void format(const basic_lazy_string<CharT, Traits, Allocator>& val, basic_format_context<CharT>& ctx) const
        {
            return formatter<std::basic_string_view<CharT, Traits>, CharT>::format(val.view(), ctx);
        }
    };

    export
    template<class CharT, std::size_t N>
    struct formatter<lib2::basic_string_literal<CharT, N>, CharT> : public formatter<std::basic_string_view<CharT>, CharT>
    {
        void format(const lib2::basic_string_literal<CharT, N>& val, basic_format_context<CharT>& ctx) const
        {
            return formatter<std::basic_string_view<CharT>, CharT>::format(val.view(), ctx);
        }
    };
}