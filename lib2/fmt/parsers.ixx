export module lib2.fmt:parsers;

import std;

import lib2.utility;
import lib2.compact_optional;
import lib2.strings;

import :context;
import :formatter;

namespace lib2
{
    export
    struct format_parser
    {
        constexpr auto parse(format_parse_context& ctx) noexcept
        {
            return ctx.begin;
        }

        static constexpr std::size_t parse_unsigned(const char*& begin, const char* const end)
        {
            std::size_t value {0};
            while (begin != end && isdigit_ascii(*begin))
            {
                const auto new_value {(value * 10) + (*begin - '0')};
                if (new_value < value)
                {
                    throw format_error{"Invalid format string: Number too large"};
                }
                value = new_value;
                ++begin;
            }

            return value;
        }
    };

    export
    struct fill_align_parser : format_parser
    {
        enum class align_type : char
        {
            left   = '<',
            right  = '>',
            center = '^'
        };

        static constexpr auto parse(format_parse_context& ctx, char& fill, align_type& align) noexcept
        {
            if (ctx.begin != ctx.end && *ctx.begin != '}')
            {
                if ((ctx.begin + 1) < ctx.end)
                {
                    const auto a {*(ctx.begin + 1)};
                    if (a == '<' || a == '>' || a == '^')
                    {
                        if (*ctx.begin != '{' && *ctx.begin != '}')
                        {
                            fill = *ctx.begin;
                        }
                        align = static_cast<align_type>(a);
                        return ctx.begin + 2;
                    }
                }

                const auto a {*ctx.begin};
                if (a == '<' || a == '>' || a == '^')
                {
                    align = static_cast<align_type>(a);
                    return ctx.begin + 1;
                }
            }

            return ctx.begin;
        }
    };

    export
    struct width_parser : format_parser
    {
    public:
        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};
            
            auto it {ctx.begin};

            if (it != ctx.end)
            {
                if (*it == '{')
                {
                    ++it;
                    if (it == ctx.end)
                    {
                        throw format_error{"Unmatched '{' in format string"};
                    }

                    if (*it == '}')
                    {
                        width = ctx.next_arg_id();
                        ctx.check_dynamic_spec_integral(width);
                        dynamic = true;
                        ++it;
                    }
                    else if (isdigit_ascii(*it))
                    {
                        width = parse_unsigned(it, ctx.end);
                        ctx.check_arg_id(width);
                        ctx.check_dynamic_spec_integral(width);
                        dynamic = true;

                        if (it == ctx.end || *it != '}')
                        {
                            throw format_error{"Unmatched '{' in format string."};
                        }
                        ++it;
                    }
                    else
                    {
                        throw format_error{"Unmatched '{' in format string."};
                    }
                }
                else if (*it >= '1' && *it <= '9')
                {
                    width = parse_unsigned(it, ctx.end);
                    dynamic = false;
                }
            }

            return it;
        }

        constexpr void set_width(const std::size_t w) noexcept
        {
            width = w;
            dynamic = false;
        }

        constexpr std::size_t get_width(format_context& ctx) const
        {
            if (dynamic)
            {
                return ctx.args.get(width).visit(overloaded {
                    [](const auto&) -> std::size_t {
                        throw format_error{"Format arg is not numeric"};
                    },
                    [](const std::intmax_t v) -> std::size_t {
                        if (v < 0)
                        {
                            throw format_error{"Format arg is negative"};
                        }
                        return v;
                    },
                    [](const std::uintmax_t v) -> std::size_t {
                        return v;
                    }
                });
            }

            return width;
        }
    private:
        std::size_t width {0};
        bool dynamic {false};
    };

    export
    struct precision_parser : format_parser
    {
    public:
        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};

            auto it {ctx.begin};

            if (it != ctx.end)
            {
                if (*it == '.')
                {
                    ++it;
                    if (it == ctx.end)
                    {
                        throw format_error{"Invalid precision format"};
                    }

                    if (*it == '{')
                    {
                        ++it;
                        if (it == ctx.end)
                        {
                            throw format_error{"Unmatched '{' in format string"};
                        }

                        if (*it == '}')
                        {
                            precision = ctx.next_arg_id();
                            ctx.check_dynamic_spec_integral(*precision);
                            dynamic = true;
                            ++it;
                        }
                        else if (isdigit_ascii(*it))
                        {
                            precision = parse_unsigned(it, ctx.end);
                            ctx.check_arg_id(*precision);
                            ctx.check_dynamic_spec_integral(*precision);
                            dynamic = true;

                            if (it == ctx.end || *it != '}')
                            {
                                throw format_error{"Unmatched '{' in format string."};
                            }
                            ++it;
                        }
                        else
                        {
                            throw format_error{"Unmatched '{' in format string."};
                        }
                    }
                    else if (isdigit_ascii(*it))
                    {
                        precision = parse_unsigned(it, ctx.end);
                        dynamic = false;
                    }
                    else
                    {
                        throw format_error{"Expected a number or replacement field in precision"};
                    }
                }
            }

            return it;
        }

        constexpr void set_precision(const std::size_t p) noexcept
        {
            precision = p;
            dynamic = false;
        }

        constexpr optional_size get_precision(format_context& ctx) const
        {
            if (dynamic)
            {
                return ctx.args.get(*precision).visit(overloaded {
                    [](const auto&) -> std::size_t {
                        throw format_error{"Format arg is not numeric"};
                    },
                    [](const std::intmax_t v) -> std::size_t {
                        if (v < 0)
                        {
                            throw format_error{"Format arg is negative"};
                        }
                        return v;
                    },
                    [](const std::uintmax_t v) -> std::size_t {
                        return v;
                    }
                });
            }

            return precision;
        }
    private:
        optional_size precision {};
        bool dynamic {false};
    };

    export
    struct sign_parser : format_parser
    {
        enum class sign_type : char
        {
            plus  = '+',
            minus = '-',
            space = ' '
        };

        static constexpr auto parse(format_parse_context& ctx, sign_type& sign) noexcept
        {
            if (ctx.begin != ctx.end)
            {
                const auto ch {*ctx.begin};
                if (ch == '+' || ch == '-' || ch == ' ')
                {
                    sign = static_cast<sign_type>(ch);
                    ++ctx.begin;
                }
            }

            return ctx.begin;
        }
    };

    export
    struct alt_parser : format_parser
    {
        static constexpr auto parse(format_parse_context& ctx, bool& alt, bool& zero) noexcept
        {
            alt = ctx.begin != ctx.end && *ctx.begin == '#';
            ctx.begin += alt;

            zero = ctx.begin != ctx.end && *ctx.begin == '0';
            ctx.begin += zero;

            return ctx.begin;
        }
    };

    export
    struct locale_parser : format_parser
    {
        static constexpr auto parse(format_parse_context& ctx, bool& use_locale) noexcept
        {
            use_locale = ctx.begin != ctx.end && *ctx.begin == 'L';
            ctx.begin += use_locale;
            
            return ctx.begin;
        }
    };
}