export module lib2.scan:parsers;

import std;

import lib2.strings;

import :scan_error;
import :context;

namespace lib2
{
    export
    constexpr auto parse_locale(scan_context& ctx, bool& use_locale) noexcept
    {
        auto it {ctx.begin()};

        if (it != ctx.end())
        {
            if (*it == 'L')
            {
                ++it;
                use_locale = true;
            }
        }

        return it;
    }

    export
    constexpr auto parse_width(scan_context& ctx, std::size_t& width)
    {
        width = 0;

        auto it {ctx.begin()};
        while (it != ctx.end())
        {
            if (is_ascii_digit(*it))
            {
                if ((width * 10) < width)
                {
                    throw scan_parse_error{"width too large"};
                }
                width *= 10;

                if ((width + (*it - '0')) < width)
                {
                    throw scan_parse_error{"width too large"};
                }
                width += *it - '0';
            }
            else
            {
                break;
            }
        }
        
        return it;
    }
}