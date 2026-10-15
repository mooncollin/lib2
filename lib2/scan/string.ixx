export module lib2.scan:string;

import std;

import lib2.strings;
import lib2.io;

import :scan_error;
import :context;
import :parsers;

namespace lib2
{
    export
    std::expected<void, scan_error> from_stream(text_istream& is, char& value)
    {
        if (is.get())
        {
            value = *is.bump();
            return {};
        }

        return std::unexpected<scan_error>{std::in_place, scan_error::end_of_input, "Unexpected end of input"};
    }

    export
    struct str_scan_fmt
    {
        std::size_t width {0};
        bool skipws {true};

        constexpr static str_scan_fmt parse(scan_context& ctx)
        {
            str_scan_fmt fmt;
            auto it {parse_width(ctx, fmt.width)};

            if (it != ctx.end())
            {
                if (*it == 'c')
                {
                    fmt.skipws = false;
                    ++it;
                }
            }

            ctx.advance_to(it);
            return fmt;
        }
    };

    export
    template<std::size_t N>
    struct str_array_scan_fmt
    {
        std::size_t width {0};
        bool skipws {true};

        constexpr static str_array_scan_fmt parse(scan_context& ctx)
        {
            str_array_scan_fmt fmt;
            auto it {parse_width(ctx, fmt.width)};
            if (fmt.width > N)
            {
                throw scan_parse_error{"width too large"};
            }

            if (it != ctx.end())
            {
                if (*it == 'c')
                {
                    fmt.skipws = false;
                    ++it;
                }
            }

            ctx.advance_to(it);
            return fmt;
        }
    };

    export
    template<std::size_t N>
    str_array_scan_fmt<N> scan_format_of(const char(&)[N]);

    export
    template<std::size_t N>
    std::expected<void, scan_error> from_stream(text_istream& is, char(&value)[N], const str_array_scan_fmt<N>& fmt = {})
    {
        if (fmt.skipws)
        {
            is >> ws;
        }

        const auto width {fmt.width ? fmt.width : N};
        if (is.read(value, width) != width)
        {
            return std::unexpected<scan_error>{std::in_place, scan_error::length_too_short, "Unable to fill char array to desired width"};
        }

        return {};
    }

    export
    template<class Allocator>
    std::expected<void, scan_error> from_stream(text_istream& is, std::basic_string<char, std::char_traits<char>, Allocator>& value, const str_scan_fmt& fmt = {})
    {
        if (fmt.skipws)
        {
            is >> ws;
        }

        value.clear();

        if (fmt.width)
        {
            value.resize(fmt.width);
            if (is.read(value, fmt.width) != fmt.width)
            {
                return std::unexpected<scan_error>{std::in_place, scan_error::length_too_short, "Unable to fill string to desired width"};
            }
        }
        else
        {
            while (is.get())
            {
                is.consume([&](const auto begin, auto end) {
                    end = std::find_if_not(begin, end, is_ascii_space);
                    value.append(begin, end);
                    return end;
                });

                if (is.read_available())
                {
                    break;
                }
            }
        }

        if (value.empty())
        {
            return std::unexpected<scan_error>{std::in_place, scan_error::invalid_scanned_value, "No string"};
        }

        return {};
    }

    export
    std::expected<void, scan_error> from_stream(text_istream& is, std::string_view& value, const str_scan_fmt& fmt = {})
    {
        if (fmt.skipws)
        {
            is >> ws;
        }
        
        value = {};

        if (fmt.width)
        {
            if (is.get())
            {
                is.consume([&](const auto begin, auto end) {
                    end = std::min(end, begin + fmt.width);
                    value = {begin, end};
                    return end;
                });
            }

            if (value.size() != fmt.width)
            {
                return std::unexpected<scan_error>{std::in_place, scan_error::length_too_short, "Unable to get a string_view of desired width"};
            }
        }
        else
        {
            if (is.get())
            {
                is.consume([&](const auto begin, auto end) {
                    end = std::find_if_not(begin, end, is_ascii_space);
                    value = {begin, end};
                    return end;
                });
            }
        }

        if (value.empty())
        {
            return std::unexpected<scan_error>{std::in_place, scan_error::end_of_input, "No string"};
        }

        return {};
    }
}

namespace std
{
    export
    template<class Allocator>
    lib2::str_scan_fmt scan_format_of(const std::basic_string<char, std::char_traits<char>, Allocator>&);

    export
    lib2::str_scan_fmt scan_format_of(std::string_view);
}