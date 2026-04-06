export module lib2.fmt:ranges;

import std;

import lib2.io;
import lib2.concepts;

import :formatter;
import :context;
import :parsers;
import :format;
import :string;
import :arithmetic;
import :misc;

namespace lib2
{
    export
    struct range_fill_align_parser : public format_parser
    {
        static constexpr auto parse(format_parse_context& ctx, char& fill, fill_align_parser::align_type& align)
        {
            if (ctx.begin != ctx.end && *ctx.begin != '}')
            {
                if ((ctx.begin + 1) < ctx.end)
                {
                    const auto a {*(ctx.begin + 1)};
                    if (a == '<' || a == '>' || a == '^')
                    {
                        if (*ctx.begin != '{' && *ctx.begin != '}' && *ctx.begin != ':')
                        {
                            fill = *ctx.begin;
                        }
                        align = static_cast<fill_align_parser::align_type>(a);
                        return ctx.begin + 2;
                    }
                }

                const auto a {*ctx.begin};
                if (a == '<' || a == '>' || a == '^')
                {
                    align = static_cast<fill_align_parser::align_type>(a);
                    return ctx.begin + 1;
                }
            }

            return ctx.begin;
        }
    };

    template<class T>
    struct pair_like : std::false_type {};

    template<class T1, class T2>
    struct pair_like<std::pair<T1, T2>> : std::true_type {};

    template<class T1, class T2>
    struct pair_like<std::tuple<T1, T2>> : std::true_type {};

    export
    enum class range_format
    {
        disabled,
        map,
        set,
        sequence,
        string,
        debug_string
    };

    export
    template<formattable T>
    class range_formatter : public width_parser
    {
    public:
        char fill {' '};
        fill_align_parser::align_type align {fill_align_parser::align_type::right};
        std::string_view separator {", "};
        std::string_view opening_bracket {"["};
        std::string_view closing_bracket {"]"};
        formatter<T> underlying {};

        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};

            ctx.begin = range_fill_align_parser::parse(ctx, fill, align);
            ctx.begin = width_parser::parse(ctx);

            if (ctx.begin != ctx.end)
            {
                switch (*ctx.begin)
                {
                case 'n':
                    set_type(range_format::disabled);
                    ++ctx.begin;
                    break;
                case 'm':
                    if constexpr (pair_like<T>::value)
                    {
                        ++ctx.begin;

                        ctx.begin = underlying.parse(ctx);
                        set_type(range_format::map);
                        return ctx.begin;
                    }
                    else
                    {
                        throw format_error{"Memberwise formatting not supported"};
                    }
                    break;
                case 's':
                    ++ctx.begin;
                    if (ctx.begin == ctx.end)
                    {
                        throw format_error{"Unexpected end of format string"};
                    }

                    if (*ctx.begin == '?')
                    {
                        ++ctx.begin;
                        set_type(range_format::debug_string);
                    }
                    else
                    {
                        set_type(range_format::string);
                    }
                    return ctx.begin;
                }

                ctx.begin = underlying.parse(ctx);
            }

            return ctx.begin;
        }

        template<std::ranges::input_range R>
            requires(std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<R>>, T>)
        void format(R&& r, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};
            lib2::ostringstream ss;
            format_context other_context {ctx, width ? ss : ctx.stream};

            const auto format_range {[&] {
                auto it {std::ranges::begin(r)};
                const auto end {std::ranges::end(r)};
                if (it != end)
                {
                    underlying.format(*it, other_context);
                    ++it;

                    for (; it != end; ++it)
                    {
                        other_context.stream.write(separator);
                        underlying.format(*it, other_context);
                    }
                }
            }};
            
            switch (type)
            {
            case range_format::sequence:
                other_context.stream.write(opening_bracket);
                format_range();
                other_context.stream.write(closing_bracket);
                break;
            case range_format::disabled:
                format_range();
                break;
            case range_format::map:
            case range_format::set:
                other_context.stream.put('{');
                format_range();
                other_context.stream.put('}');
                break;
            case range_format::string:
            case range_format::escaped_string:
                if constexpr (std::same_as<T, std::string>)
                {
                    underlying.format(std::string(std::from_range, r), other_context);
                }
                break;
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);

                str_fmt.format(ss.view(), ctx);
            }
        }

        template<std::ranges::input_range R>
            requires(std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<R>>, T>)
        static void default_format(R&& r, format_context& ctx)
        {
            ctx.stream.put('[');

            auto it {std::ranges::begin(r)};
            const auto end {std::ranges::end(r)};
            if (it != end)
            {
                format_to<"{}">(ctx.locale(), ctx.stream, *it);
                ++it;

                for (; it != end; ++it)
                {
                    ctx.stream.write(", ");
                    format_to<"{}">(ctx.locale(), ctx.stream, *it);
                }
            }

            ctx.stream.put(']');
        }

        [[nodiscard]] constexpr range_format get_type() const noexcept
        {
            return type;
        }

        constexpr void set_type(const range_format type)
        {
            switch (type)
            {
            case range_format::map:
                if constexpr (pair_like<T>::value)
                {
                    opening_bracket = "{";
                    closing_bracket = "}";
                    underlying.separator = ": ";
                    underlying.opening_bracket = {};
                    underlying.closing_bracket = {};
                }
                else
                {
                    throw format_error{"Memberwise formatting not supported"};
                }
                break;
            case range_format::set:
                opening_bracket = "{";
                closing_bracket = "}";
                break;
            case range_format::debug_string:
                if constexpr (requires(formatter<T> under) { under.debug = true; })
                {
                    underlying.debug = true;
                }
                break;
            }

            this->type = type;
        }

    private:
        range_format type {range_format::sequence};
    };

    template<class R>
    constexpr range_format format_kind {range_format::disabled};
    
    template<std::ranges::input_range R>
        requires std::same_as<R, std::remove_cvref_t<R>>
    constexpr range_format format_kind<R> {[]
    {
        if constexpr (std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<R>>, R>)
        {
            return range_format::disabled;
        }
        else if constexpr (requires { typename R::key_type; })
        {
            if constexpr (requires { typename R::mapped_type; } &&
                        pair_like<std::ranges::range_reference_t<R>>::value)
            {
                return range_format::map;
            }
            else
            {
                return range_format::set;
            }
        }
        else
        {
            return range_format::sequence;
        }
    }()};

    export
    template<std::ranges::input_range R>
    struct formatter<R>;

    export
    template<std::ranges::input_range R>
        requires(format_kind<R> == range_format::sequence)
    struct formatter<R> : public range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>
    {
        void format(const R& r, format_context& ctx) const
        {
            range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::format(r, ctx);
        }

        static void default_format(const R& r, format_context& ctx)
        {
            range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::default_format(r, ctx);
        }
    };

    export
    template<std::ranges::input_range R>
        requires(format_kind<R> == range_format::map)
    struct formatter<R> : public range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>
    {
    public:
        constexpr formatter<R>()
        {
            this->set_type(range_format::map);
        }

        constexpr auto parse(format_parse_context& ctx)
        {
            ctx.begin = range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::parse(ctx);
            this->set_type(range_format::map);

            return ctx.begin;
        }

        void format(const R& r, format_context& ctx) const
        {
            range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::format(r, ctx);
        }

        static void default_format(const R& r, format_context& ctx)
        {
            range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::default_format(r, ctx);
        }
    private:
        using range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::separator;
        using range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::opening_bracket;
        using range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::closing_bracket;
        using range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::underlying;
    };

    export
    template<std::ranges::input_range R>
        requires(format_kind<R> == range_format::set)
    struct formatter<R> : public range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>
    {
    public:
        constexpr formatter<R>()
        {
            this->set_type(range_format::set);
        }

        constexpr auto parse(format_parse_context& ctx)
        {
            ctx.begin = range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::parse(ctx);
            this->set_type(range_format::set);

            return ctx.begin;
        }

        void format(const R& r, format_context& ctx) const
        {
            range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::format(r, ctx);
        }

        static void default_format(const R& r, format_context& ctx)
        {
            range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::default_format(r, ctx);
        }
    private:
        using range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::separator;
        using range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::opening_bracket;
        using range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::closing_bracket;
        using range_formatter<std::remove_cvref_t<std::ranges::range_reference_t<R>>>::underlying;
    };

    export
    template<std::ranges::input_range R>
        requires(format_kind<R> == range_format::string)
    struct formatter<R> : public range_formatter<std::string>
    {
    public:
        constexpr formatter<R>()
        {
            this->set_type(range_format::string);
        }

        constexpr auto parse(format_parse_context& ctx)
        {
            ctx.begin = this->underlying.parse(ctx);
            this->set_type(range_format::string);

            return ctx.begin;
        }

        void format(const R& r, format_context& ctx) const
        {
            range_formatter<std::string>::format(r, ctx);
        }

        static void default_format(const R& r, format_context& ctx)
        {
            range_formatter<std::string>::default_format(r, ctx);
        }
    private:
        using range_formatter<std::string>::separator;
        using range_formatter<std::string>::opening_bracket;
        using range_formatter<std::string>::closing_bracket;
        using range_formatter<std::string>::underlying;
    };

    export
    template<std::ranges::input_range R>
        requires(format_kind<R> == range_format::debug_string)
    struct formatter<R> : public range_formatter<std::string>
    {
    public:
        constexpr formatter<R>()
        {
            this->set_type(range_format::debug_string);
        }

        constexpr auto parse(format_parse_context& ctx)
        {
            ctx.begin = this->underlying.parse(ctx);
            this->set_type(range_format::debug_string);

            return ctx.begin;
        }

        void format(const R& r, format_context& ctx) const
        {
            range_formatter<std::string>::format(r, ctx);
        }

        static void default_format(const R& r, format_context& ctx)
        {
            range_formatter<std::string>::default_format(r, ctx);
        }
    private:
        using range_formatter<std::string>::separator;
        using range_formatter<std::string>::opening_bracket;
        using range_formatter<std::string>::closing_bracket;
        using range_formatter<std::string>::underlying;
    };

    export
    template<class... Ts>
    struct formatter<std::tuple<Ts...>> : public width_parser
    {
        char fill {' '};
        fill_align_parser::align_type align {fill_align_parser::align_type::right};
        std::string_view separator {", "};
        std::string_view opening_bracket {"("};
        std::string_view closing_bracket {")"};

        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};

            ctx.begin = range_fill_align_parser::parse(ctx, fill, align);
            ctx.begin = width_parser::parse(ctx);

            if (ctx.begin != ctx.end)
            {
                switch (*ctx.begin)
                {
                case 'n':
                    separator = {};
                    opening_bracket = {};
                    closing_bracket = {};
                    ++ctx.begin;
                    break;
                case 'm':
                    if constexpr (sizeof...(Ts) == 2)
                    {
                        separator = ": ";
                        opening_bracket = {};
                        closing_bracket = {};
                        ++ctx.begin;
                    }
                    else
                    {
                        throw format_error{"Memberwise formatting not supported for non 2-sized tuples"};
                    }
                    break;
                }
            }

            return ctx.begin;
        }

        void format(const std::tuple<Ts...>& value, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};
            lib2::ostringstream ss;
            format_context other_context {ctx, width ? ss : ctx.stream};

            other_context.stream.write(opening_bracket);

            if constexpr (sizeof...(Ts) > 0)
            {
                std::apply(
                    [&](const auto& v1, const auto&... vs) {
                        format_to<"{}">(other_context.locale(), other_context.stream, v1);
                        ((other_context.stream.write(separator), format_to<"{}">(other_context.locale(), other_context.stream, vs)), ...);
                    }
                , value);
            }

            other_context.stream.write(closing_bracket);

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);

                str_fmt.format(ss.view(), ctx);
            }
        }

        static void default_format(const std::tuple<Ts...>& value, format_context& ctx)
        {
            ctx.stream.put('(');

            if constexpr (sizeof...(Ts) > 0)
            {
                std::apply(
                    [&](const auto& v1, const auto&... vs) {
                        format_to<"{}">(ctx.locale(), ctx.stream, v1);
                        ((ctx.stream.write(", "), format_to<"{}">(ctx.locale(), ctx.stream, vs)), ...);
                    }
                , value);
            }

            ctx.stream.put(')');
        }
    };

    export
    template<class T1, class T2>
    struct formatter<std::pair<T1, T2>> : public formatter<std::tuple<T1, T2>>
    {
        void format(const std::pair<T1, T2>& value, format_context& ctx) const
        {
            formatter<std::tuple<T1, T2>>::format(std::forward_as_tuple(value.first, value.second), ctx);
        }

        static void default_format(const std::pair<T1, T2>& value, format_context& ctx)
        {
            formatter<std::tuple<T1, T2>>::default_format(std::forward_as_tuple(value.first, value.second), ctx);
        }
    };
}