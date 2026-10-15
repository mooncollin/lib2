export module lib2.scan;
export import :scan_error;
export import :utility;
export import :context;
export import :parsers;
export import :string;

import std;

import lib2.strings;
import lib2.io;

namespace lib2
{
    std::expected<void, scan_error> do_scan(text_istream& is, scan_context& ctx)
    {
        auto fmt_it {ctx.begin()};

        while (is.get() && fmt_it != ctx.end())
        {
            if (*fmt_it != '{')
            {
                if (is_ascii_space(*is.get()) && is_ascii_space(*fmt_it))
                {
                    is >> ws;
                    fmt_it = std::find_if_not(fmt_it, ctx.end(), is_ascii_space);
                    continue;
                }

                if (*is.get() != *fmt_it)
                {
                    return std::unexpected<scan_error>{std::in_place, scan_error::invalid_literal, "Pattern not matched"};
                }

                is.bump();
                ++fmt_it;
                continue;
            }

            ++fmt_it;
            if (*fmt_it == '{')
            {
                if (is.get() != '{')
                {
                    return std::unexpected<scan_error>{std::in_place, scan_error::invalid_literal, "Pattern not matched"};
                }

                is.bump();
                ++fmt_it;
                continue;
            }

            std::size_t arg_idx;

            switch (*fmt_it)
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
                {
                    const std::string_view sint {fmt_it, ctx.end()};
                    const auto conv_result {std::from_chars(sint.data(), sint.data() + sint.size(), arg_idx)};
                    if (conv_result.ec != std::errc{})
                    {
                        throw scan_parse_error{"Invalid format string"};
                    }
                    ctx.check_arg_id(arg_idx);
                    fmt_it += conv_result.ptr - sint.data();

                    if (fmt_it == ctx.end())
                    {
                        throw scan_parse_error{"Invalid format string"};
                    }

                    if (*conv_result.ptr != ':')
                    {
                        throw scan_parse_error{"Invalid format string"};
                    }

                    ++fmt_it;
                }
                    break;
                case ':':
                    ++fmt_it;
                case '}':
                    arg_idx = ctx.next_arg_id();
                    break;
                default:
                    throw scan_parse_error{"Invalid format string"};
            }

            ctx.advance_to(fmt_it);
            auto arg {ctx.arg(arg_idx)};
            const auto result {arg.scan(is, ctx)};
            if (!result)
            {
                return std::unexpected{result.error()};
            }

            if (ctx.begin() == ctx.end() || *ctx.begin() != '}')
            {
                throw scan_parse_error{"Unparsed format"};
            }

            fmt_it = ctx.begin();
            ++fmt_it;
        }

        if (fmt_it != ctx.end() && is.get())
        {
            return std::unexpected<scan_error>{std::in_place, scan_error::end_of_input, "Unexpected end of input"};
        }

        return {};
    }

    export
    template<class... Args>
    std::expected<void, scan_error> scan_to(text_istream& is, const std::string_view fmt, Args&... args)
    {
        auto arg_store {make_scan_args(args...)};
        scan_context ctx {fmt, arg_store};
        return do_scan(is, ctx);
    }

    export
    template<class... Args>
    std::expected<void, scan_error> scan_to(text_istream& is, const std::locale& loc, const std::string_view fmt, Args&... args)
    {
        auto arg_store {make_scan_args(args...)};
        scan_context ctx {fmt, arg_store, loc};
        return do_scan(is, ctx);
    }

    export
    template<class... Args>
    std::expected<std::tuple<Args...>, scan_error> scan(text_istream& is, const std::string_view fmt)
    {
        std::tuple<Args...> args;

        const auto result {std::apply([&](auto&... as) {
            return scan_to(is, fmt, as...);
        }, args)};
        
        if (result)
        {
            return args;
        }

        return std::unexpected{result.error()};
    }

    export
    template<class... Args>
    std::expected<std::tuple<Args...>, scan_error> scan(text_istream& is, const std::locale& loc, const std::string_view fmt)
    {
        std::tuple<Args...> args;

        const auto result {std::apply([&](auto&... as) {
            return scan_to(is, loc, fmt, as...);
        }, args)};
        
        if (result)
        {
            return args;
        }

        return std::unexpected{result.error()};
    }
}