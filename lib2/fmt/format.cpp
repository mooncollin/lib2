module lib2.fmt;

import std;

import lib2.utility;

namespace lib2
{
    struct runtime_collector
    {
        format_context fmt_ctx;

        void string_literal(const std::string_view str)
        {
            fmt_ctx.stream.write(str);
        }

        void arg(const std::size_t idx, format_parse_context& parse_ctx)
        {
            fmt_ctx.args.get(idx).visit(overloaded {
                [&](const format_arg::handle& h) {
                    h.format(parse_ctx, fmt_ctx);
                },
                [&]<class T>(const T& value) {
                    if (parse_ctx.begin == parse_ctx.end || *parse_ctx.begin == '}')
                    {
                        formatter<T>::default_format(value, fmt_ctx);
                    }
                    else
                    {
                        formatter<T> fmtr;
                        parse_ctx.begin = fmtr.parse(parse_ctx);
                        fmtr.format(value, fmt_ctx);
                    }
                }
            });
        }
    };

    text_ostream vformat_to(text_ostream os, const std::string_view fmt, const format_args args)
    {
        runtime_collector collector {.fmt_ctx={os, args}};
        format_parse_context parse_ctx {fmt, args.size()};
        do_format(parse_ctx, collector);
        return os;
    }

    text_ostream vformat_to(const std::locale& loc, text_ostream os, const std::string_view fmt, const format_args args)
    {
        runtime_collector collector {.fmt_ctx={loc, os, args}};
        format_parse_context parse_ctx {fmt, args.size()};
        do_format(parse_ctx, collector);
        return os;
    }
}