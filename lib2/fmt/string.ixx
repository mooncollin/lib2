export module lib2.fmt:string;

import std;

import lib2.strings;
import lib2.io;

import :context;
import :parsers;
import :formatter;
import :format;

namespace lib2
{
    export
    template<std::ranges::view V>
    struct escaped
    {
        template<std::ranges::input_range R>
        constexpr escaped(R&& r)
            : base{std::ranges::views::all(std::forward<R>(r))} {}

        V base;
    };

    export
    template<std::ranges::input_range R>
    escaped(R&&) -> escaped<std::ranges::views::all_t<R>>;

    export
    template<class V>
    struct formatter<escaped<V>> : public format_parser
    {
        void format(const escaped<V>& e, format_context& ctx) const
        {
            ctx.stream.put('"');

            const auto utf_view {views::utf_codepoints(e.base)};
            for (auto it {utf_view.begin()}; it != utf_view.end(); ++it)
            {
                switch (*it)
                {
                case '\t':
                    ctx.stream.write("\\t");
                    break;
                case '\n':
                    ctx.stream.write("\\n");
                    break;
                case '\r':
                    ctx.stream.write("\\r");
                    break;
                case '"':
                    ctx.stream.write("\\\"");
                    break;
                case '\\':
                    ctx.stream.write("\\\\");
                    break;
                default:
                    if (it.invalid())
                    {
                        format_to<"\\x{:0>2x}">(ctx.stream, *it);
                    }
                    else if (*it <= 0x7F)
                    {
                        ctx.stream.put(static_cast<char>(*it));
                    }
                    else
                    {
                        format_to<"\\u{:x}">(ctx.stream, *it);
                    }
                }
            }

            ctx.stream.put('"');
        }
    };

    export
    template<>
    struct formatter<std::string_view> : public width_parser, public precision_parser
    {
        char fill {' '};
        fill_align_parser::align_type align {fill_align_parser::align_type::left};
        bool debug {false};

        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};

            ctx.begin = fill_align_parser::parse(ctx, fill, align);
            ctx.begin = width_parser::parse(ctx);
            ctx.begin = precision_parser::parse(ctx);
            
            if (ctx.begin != ctx.end)
            {
                debug = *ctx.begin == '?';
                ctx.begin += debug;
            }

            return ctx.begin;
        }

        void format(std::string_view str, format_context& ctx) const;

        static void default_format(const std::string_view str, format_context& ctx)
        {
            ctx.stream.write(str);
        }
    };

    export
    template<class Allocator>
    struct formatter<std::basic_string<char, std::char_traits<char>, Allocator>> : public formatter<std::string_view>
    {
        void format(const std::basic_string<char, std::char_traits<char>, Allocator>& value, format_context& ctx) const
        {
            formatter<std::string_view>::format(std::string_view{value}, ctx);
        }

        static void default_format(const std::basic_string<char, std::char_traits<char>, Allocator>& value, format_context& ctx)
        {
            formatter<std::string_view>::default_format(std::string_view{value}, ctx);
        }
    };

    export
    template<>
    struct formatter<const char*> : public formatter<std::string_view> {};

    export
    template<>
    struct formatter<char*> : public formatter<const char*> {};

    export
    template<std::size_t N>
    struct formatter<const char(&)[N]> : public formatter<const char*> {};

    export
    template<std::size_t N>
    struct formatter<char(&)[N]> : public formatter<const char*> {};

    export
    template<std::size_t N>
    struct formatter<const char[N]> : public formatter<const char*> {};

    export
    template<std::size_t N>
    struct formatter<char[N]> : public formatter<const char*> {};
}