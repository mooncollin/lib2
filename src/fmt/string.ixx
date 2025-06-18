#pragma once

namespace lib2
{
    export
    template<class CharT>
    lib2::basic_text_ostream<CharT>& escape_codepoint(lib2::basic_text_ostream<CharT>& os, std::uint32_t codepoint)
    {
        if (codepoint <= 0x7F)
        {
            os.put(static_cast<CharT>(codepoint));
        }
        else
        {
            format_to(os, STATICALLY_WIDEN(CharT, "\\u{:x}"), codepoint);
        }

        return os;
    }

    export
    template<class CharT, class Traits>
    struct formatter<std::basic_string_view<CharT, Traits>, CharT> : public fill_align_parser<CharT>, public width_parser<CharT>, public precision_parser<CharT>
    {
    public:
        [[nodiscard]] constexpr bool is_debug_formatting() const noexcept
        {
            return debug;
        }

        constexpr void set_debug_formatting(const bool d = true) noexcept
        {
            debug = d;
        }

        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
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
            do_format(str, ctx, std::allocator<CharT>{});
        }
    protected:
        template<class Allocator>
        void do_format(std::basic_string_view<CharT, Traits> str, basic_format_context<CharT>& ctx, const Allocator a) const
        {
            const utf8_codepoint_view utf8_view {str};
            const auto precision {this->get_precision(ctx)};

            if (precision)
            {
                auto utf8_it {utf8_view.begin()};
                std::ranges::advance(utf8_it, *precision, utf8_view.end());
                const auto leftover {utf8_it.leftover()};
                str.remove_prefix(str.size() - std::basic_string_view<CharT, Traits>{leftover.begin(), leftover.end()}.size());
            }

            lib2::basic_ostringstream<CharT, Traits> debug_str;
            if (debug)
            {
                debug_str.put('"');
                for (auto it {utf8_view.begin()}; it != utf8_view.end(); ++it)
                {
                    switch (*it)
                    {
                    case '\t':
                        debug_str << STATICALLY_WIDEN(CharT, "\\t");
                        break;
                    case '\n':
                        debug_str << STATICALLY_WIDEN(CharT, "\\n");
                        break;
                    case '\r':
                        debug_str << STATICALLY_WIDEN(CharT, "\\r");
                        break;
                    case '"':
                        debug_str << STATICALLY_WIDEN(CharT, "\\\"");
                        break;
                    case '\\':
                        debug_str << STATICALLY_WIDEN(CharT, "\\\\");
                        break;
                    default:
                        if (it.is_invalid())
                        {
                            format_to(debug_str, STATICALLY_WIDEN(CharT, "\\x{:>02x}"), *it);
                        }
                        else
                        {
                            escape_codepoint(debug_str, *it);
                        }
                    }
                }
                debug_str.put('"');
                str = debug_str.view();
            }

            const auto size {std::ranges::distance(utf8_view)};
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
}