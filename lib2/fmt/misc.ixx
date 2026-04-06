export module lib2.fmt:misc;

import std;

import lib2.io;
import lib2.strings;

import :formatter;
import :context;
import :parsers;
import :format;
import :string;
import :arithmetic;

namespace lib2
{
    export
    template<>
    struct formatter<bool> : public width_parser
    {
        char fill {' '};
        fill_align_parser::align_type align {fill_align_parser::align_type::left};
        unsigned int base {0};
        bool upper {false};
        bool use_locale {false};

        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};

            ctx.begin = fill_align_parser::parse(ctx, fill, align);
            ctx.begin = width_parser::parse(ctx);
            ctx.begin = locale_parser::parse(ctx, use_locale);
            
            if (ctx.begin != ctx.end)
            {
                switch (*ctx.begin)
                {
                case 'B':
                    upper = true;
                case 'b':
                    base = 2;
                    ++ctx.begin;
                    break;
                case 'o':
                    base = 8;
                    ++ctx.begin;
                    break;
                case 'd':
                    base = 10;
                    ++ctx.begin;
                    break;
                case 'X':
                    upper = true;
                case 'x':
                    base = 16;
                    ++ctx.begin;
                    break;
                }
            }

            return ctx.begin;
        }

        void format(const bool value, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            if (base)
            {
                using un_t = std::make_unsigned_t<char>;

                formatter<unsigned char> int_fmt;
                int_fmt.fill  = fill;
                int_fmt.align = align;
                int_fmt.set_width(width);
                int_fmt.base  = base;
                int_fmt.upper = upper;

                int_fmt.format(static_cast<unsigned char>(value), ctx);
            }
            else
            {
                std::string_view svalue;
                std::string loc_value;

                if (use_locale)
                {
                    const auto loc {ctx.locale()};
                    const auto& facet {std::use_facet<std::numpunct<char>>(loc)};

                    if (value)
                    {
                        loc_value = facet.truename();
                    }
                    else
                    {
                        loc_value = facet.falsename();
                    }
                    svalue = loc_value;
                }
                else if (value)
                {
                    svalue = "true";
                }
                else
                {
                    svalue = "false";
                }

                if (width)
                {
                    formatter<std::string_view> str_fmt;
                    str_fmt.fill  = fill;
                    str_fmt.align = align;
                    str_fmt.set_width(width);
                    str_fmt.format(svalue, ctx);
                }
                else
                {
                    ctx.stream.write(svalue);
                }
            }
        }

        static void default_format(const bool value, format_context& ctx)
        {
            if (value)
            {
                ctx.stream.write("true");
            }
            else
            {
                ctx.stream.write("false");
            }
        }
    };

    export
    template<std::derived_from<std::exception> E>
    struct formatter<E>
    {
        std::string_view delim {"\n "};

        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};

            const auto end {std::find(ctx.begin, ctx.end, '}')};
            if (ctx.begin != end)
            {
                delim = {ctx.begin, end};
            }

            return end;
        }

        void format(const std::exception& e, format_context& ctx) const
        {
            ctx.stream.write(e.what());

            try
            {
                std::rethrow_if_nested(e);
            }
            catch (const std::exception& e2)
            {
                ctx.stream.write(delim);
                format(e2, ctx);
            }
        }
    };

    export
    template<>
    struct formatter<std::filesystem::path> : public width_parser
    {
        char fill {' '};
        fill_align_parser::align_type align {fill_align_parser::align_type::left};
        bool debug {false};
        bool generic {false};

        constexpr auto parse(format_parse_context& ctx)
        {
            *this = {};

            ctx.begin = fill_align_parser::parse(ctx, fill, align);
            ctx.begin = width_parser::parse(ctx);

            debug = ctx.begin != ctx.end && *ctx.begin == '?';
            ctx.begin += debug;

            generic = ctx.begin != ctx.end && *ctx.begin == 'g';
            ctx.begin += generic;

            return ctx.begin;
        }

        void format(const std::filesystem::path& value, format_context& ctx) const
        {
            formatter<std::string> str_fmt;
            str_fmt.fill  = fill;
            str_fmt.align = align;
            str_fmt.set_width(this->get_width(ctx));
            str_fmt.debug = debug;

            if (generic)
            {
                str_fmt.format(value.generic_string(), ctx);
            }
            else
            {
                #if _WIN32
                str_fmt.format(value.string(), ctx);
                #else
                str_fmt.format(value.native(), ctx);
                #endif
            }
        }

        static void default_format(const std::filesystem::path& value, format_context& ctx)
        {
            #if _WIN32
            lib2::utf16_to_utf8(value.native(), ostream_iterator{ctx.stream});
            #else
            str_fmt.format(value.native(), ctx);
            #endif
        }
    };
}