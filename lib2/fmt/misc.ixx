#pragma once

namespace lib2
{
    export
    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const bool b)
    {
        if (b)
        {
            return os << STATICALLY_WIDEN(CharT, "true");
        }

        return os << STATICALLY_WIDEN(CharT, "false");
    }
    
    export
    template<class CharT>
    struct formatter<bool, CharT> : public fill_align_parser<CharT>, public width_parser<CharT>, public locale_parser<CharT>
    {
    public:
        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            auto it {fill_align_parser<CharT>::parse(ctx)};
            ctx.advance_to(it);
            it = width_parser<CharT>::parse(ctx);
            ctx.advance_to(it);
            it = locale_parser<CharT>::parse(ctx);

            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'B':
                case 'b':
                case 'd':
                case 'o':
                case 'X':
                case 'x':
                    type = *it;
                    ++it;
                    break;
                }
            }

            return it;
        }

        void format(const bool val, basic_format_context<CharT>& ctx) const
        {
            const auto width {this->get_width(ctx)};

            if (type)
            {
                using un_t = std::make_unsigned_t<CharT>;
                formatter<un_t, CharT> int_fmt;
                int_fmt.set_fill(this->get_fill());
                int_fmt.set_align(this->get_align());
                int_fmt.set_width(width);

                switch (type)
                {
                case 'B':
                    int_fmt.set_upper_format();
                case 'b':
                    int_fmt.set_base(2);
                    break;
                case 'd':
                    int_fmt.set_base(10);
                    break;
                case 'o':
                    int_fmt.set_base(8);
                    break;
                case 'X':
                    int_fmt.set_upper_format();
                case 'x':
                    int_fmt.set_base(16);
                    break;
                }

                return int_fmt.format(static_cast<un_t>(val), ctx);
            }

            formatter<std::basic_string_view<CharT>, CharT> str_fmt;
            str_fmt.set_fill(this->get_fill());
            str_fmt.set_align(this->get_align());
            str_fmt.set_width(width);

            if (this->is_locale_formatting())
            {
                const auto& facet {std::use_facet<std::numpunct<CharT>>(ctx.locale())};
                if (val)
                {
                    return str_fmt.format(facet.truename(), ctx);
                }

                return str_fmt.format(facet.falsename(), ctx);
            }

            if (val)
            {
                return str_fmt.format(STATICALLY_WIDEN(CharT, "true"), ctx);
            }

            return str_fmt.format(STATICALLY_WIDEN(CharT, "false"), ctx);
        }
    private:
        CharT type {};
    };
    
    export
    template<std::derived_from<std::exception> E>
    struct formatter<E>
    {
    public:
        [[nodiscard]] constexpr std::string_view get_delimiter() const noexcept
        {
            return delim;
        }

        constexpr void set_delimiter(std::string_view d) noexcept
        {
            delim = d;
        }

        constexpr auto parse(format_parse_context& ctx) noexcept
        {
            const auto it {std::find(ctx.begin(), ctx.end(), '}')};
            delim = {ctx.begin(), it};
            return it;
        }
    
        void format(const E& e, format_context& ctx) const
        {
            do_format(e, ctx);
        }
    private:
        void do_format(const E& e, format_context& ctx) const
        {
            ctx.stream() << e.what();

            try
            {
                std::rethrow_if_nested(e);
            }
            catch (const std::exception& e2)
            {
                ctx.stream() << delim;
                do_format(e2, ctx);
            }
        }

        std::string_view delim;
    };

    export
    inline text_ostream& operator<<(text_ostream& os, const std::exception& e)
    {
        return format_to(os, "{:\n }", e);
    }
}