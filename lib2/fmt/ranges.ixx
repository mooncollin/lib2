export module lib2.fmt:ranges;

import std;

import lib2.io;

import :utility;
import :context;
import :parsers;
import :format;

namespace lib2
{
    export
    template<class... Ts>
    struct tuple_fmt
    {
        char fill {' '};
        align_type align {align_type::right};
        std::size_t width {0};
        std::string_view separator {", "};
        std::string_view opening_bracket {"("};
        std::string_view closing_bracket {")"};

        constexpr tuple_fmt parse(format_context& ctx)
        {
            tuple_fmt fmt;
            auto it {parse_range_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);

            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'n':
                    fmt.separator = {};
                    fmt.opening_bracket = {};
                    fmt.closing_bracket = {};
                    ++it;
                    break;
                case 'm':
                    throw std::format_error{"Memberwise formatting not supported for non 2-sized tuples"};
                }
            }

            ctx.advance_to(it);
            return it;
        }
    };

    export
    template<class T1, class T2>
    struct tuple_fmt<T1, T2>
    {
        char fill {' '};
        align_type align {align_type::right};
        std::size_t width {0};
        std::string_view separator {", "};
        std::string_view opening_bracket {"("};
        std::string_view closing_bracket {")"};

        constexpr tuple_fmt parse(format_context& ctx)
        {
            tuple_fmt fmt;
            auto it {parse_range_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);

            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'n':
                    fmt.separator = {};
                    fmt.opening_bracket = {};
                    fmt.closing_bracket = {};
                    ++it;
                    break;
                case 'm':
                    fmt.separator = ": ";
                    fmt.opening_bracket = {};
                    fmt.closing_bracket = {};
                    ++it;
                    break;
                }
            }

            ctx.advance_to(it);
            return it;
        }
    };

    export
    template<class... Ts>
    inline text_ostream& to_stream(text_ostream& os, const std::tuple<Ts...>& tup, const tuple_fmt<Ts...>& fmt)
    {
        os << fmt.opening_bracket;

        if constexpr (sizeof...(Ts) > 0)
        {
            std::apply(
                [&](const auto& v1, const auto&... vs) {
                    os << v1;
                    ((os << fmt.separator << vs), ...);
                }
            , tup);
        }

        os << fmt.closing_bracket;
        return os;
    }

    export
    template<class... Ts>
    inline text_ostream& operator<<(text_ostream& os, const std::tuple<Ts...>& tup)
    {
        return to_stream(os, tup, {});
    }

    export
    template<class T1, class T2>
    inline text_ostream& to_stream(text_ostream& os, const std::pair<T1, T2>& p, const tuple_fmt<T1, T2>& fmt)
    {
        return os << fmt.opening_bracket
                  << p.first
                  << fmt.separator
                  << p.second
                  << fmt.closing_bracket;
    }

    export
    template<class T1, class T2>
    inline text_ostream& operator<<(text_ostream& os, const std::pair<T1, T2>& p)
    {
        return to_stream(os, p, {});
    }

    export
    template<class R>
    struct range_fmt;

    export
    template<class R>
        requires(std::format_kind<R> == std::format_kind::map || std::format_kind<R> == std::format_kind::set)
    struct range_fmt<R>
    {
        char fill {' '};
        align_type align {align_type::right};
        std::size_t width {0};
        std::string_view separator {", "};
        std::string_view opening_bracket {"{"};
        std::string_view closing_bracket {"}"};
        format_of_t<typename std::ranges::range_value_t<R>> element_fmt {};

        constexpr range_fmt parse(format_context& ctx)
        {
            range_fmt fmt;
            auto it {parse_range_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);

            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'n':
                    fmt.opening_bracket = {};
                    fmt.closing_bracket = {};
                    ++it;
                    break;
                }
            }

            if constexpr (requires(format_context& ctx) { format_of_t<std::ranges::range_value_t<R>>::parse(ctx); })
            {
                ctx.advance_to(it);
                fmt.element_fmt = format_of_t<std::ranges::range_value_t<R>>::parse(ctx);
            }

            ctx.advance_to(it);
            return it;
        }
    };

    export
    template<class R>
        requires(std::format_kind<R> == std::format_kind::sequence)
    struct range_fmt<R>
    {
        char fill {' '};
        align_type align {align_type::right};
        std::size_t width {0};
        std::string_view separator {", "};
        std::string_view opening_bracket {"["};
        std::string_view closing_bracket {"]"};
        format_of_t<typename std::ranges::range_value_t<R>> element_fmt {};

        constexpr range_fmt parse(format_context& ctx)
        {
            range_fmt fmt;
            auto it {parse_range_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);

            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'n':
                    fmt.opening_bracket = {};
                    fmt.closing_bracket = {};
                    ++it;
                    break;
                }
            }

            if constexpr (requires(format_context& ctx) { format_of_t<std::ranges::range_value_t<R>>::parse(ctx); })
            {
                ctx.advance_to(it);
                fmt.element_fmt = format_of_t<std::ranges::range_value_t<R>>::parse(ctx);
            }

            ctx.advance_to(it);
            return it;
        }
    };
    
    export
    template<std::ranges::input_range R>
        requires(formattable<std::ranges::range_reference_t<R>> && !std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<R>>, R>)
    text_ostream& to_stream(text_ostream& os, R&& r, const range_fmt<R>& fmt)
    {
        os << fmt.opening_bracket;

        auto it {std::ranges::begin(r)};
        const auto end {std::ranges::end(r)};
        if (it != end)
        {
            to_stream(os, *it, fmt.element_fmt);
            ++it;
        }

        for (; it != end; ++it)
        {
            os << fmt.separator;
            to_stream(os, *it, fmt.element_fmt);
        }

        os << fmt.closing_bracket;
        return os;
    }

    export
    template<std::ranges::input_range R>
        requires(stream_writable<std::ranges::range_reference_t<R>> && !character<std::remove_cvref_t<std::ranges::range_reference_t<R>>>)
    text_ostream& operator<<(text_ostream& os, R&& r)
    {
        constexpr range_fmt<R> fmt {};

        os << fmt.opening_bracket;
        auto it {std::ranges::begin(r)};
        const auto end {std::ranges::end(r)};

        if (it != end)
        {
            os << *it;
            ++it;
        }

        for (; it != end; ++it)
        {
            os << fmt.separator << *it;
        }

        os << fmt.closing_bracket;
        return os;
    }
}

namespace std
{
    export
    template<class... Ts>
    lib2::tuple_fmt<Ts...> format_of(const std::tuple<Ts...>&);

    export
    template<class T1, class T2>
    lib2::tuple_fmt<T1, T2> format_of(const std::pair<T1, T2>&);
}