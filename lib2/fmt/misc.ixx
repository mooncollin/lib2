export module lib2.fmt:misc;

import std;

import lib2.io;
import lib2.strings;

import :utility;
import :context;
import :parsers;
import :format;

namespace lib2
{
    export
    struct exception_fmt
    {
        std::string_view delim {"\n "};
    };

    export
    exception_fmt format_of(const std::exception&);

    export
    text_ostream& to_stream(text_ostream& os, const std::exception& value, const exception_fmt& fmt)
    {
        os << value.what();

        try
        {
            std::rethrow_if_nested(value);
        }
        catch (const std::exception& e2)
        {
            os << fmt.delim;
            to_stream(os, e2, fmt);
        }

        return os;
    }

    export
    inline text_ostream& operator<<(text_ostream& os, const std::exception& e)
    {
        return to_stream(os, e, {});
    }

    export
    text_ostream& operator<<(text_ostream& os, const std::filesystem::path& p)
    {
        if constexpr (std::same_as<std::filesystem::path::value_type, char>)
        {
            os << p.native();
        }
        else
        {
            lib2::utf16_to_utf8(p.native(), ostream_iterator{os});
        }

        return os;
    }

    export
    struct path_fmt
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        bool debug {false};
        bool generic {false};

        constexpr static path_fmt parse(format_context& ctx)
        {
            path_fmt fmt;

            auto it {parse_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);
            
            if (it != ctx.end())
            {
                if (*it == '?')
                {
                    fmt.debug = true;
                    ++it;
                }
            }

            if (it != ctx.end())
            {
                if (*it == 'g')
                {
                    fmt.generic = true;
                    ++it;
                }
            }

            ctx.advance_to(it);
            return fmt;
        }
    };

    export
    path_fmt format_of(const std::filesystem::path&);

    export
    text_ostream& to_stream(text_ostream& os, const std::filesystem::path& value, const path_fmt& fmt)
    {
        const str_fmt str_fmtting {
            .fill  = fmt.fill,
            .align = fmt.align,
            .width = fmt.width,
            .debug = fmt.debug
        };

        if (fmt.generic)
        {
            to_stream(os, value.generic_string(), str_fmtting);
        }
        else
        {
            if constexpr (std::same_as<std::filesystem::path::value_type, char>)
            {
                // to_stream(os, value.native(), str_fmtting);
            }
            else
            {
                to_stream(os, value.string(), str_fmtting);
            }
        }

        return os;
    }
}