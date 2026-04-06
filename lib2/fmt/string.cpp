module lib2.fmt;

import std;

import :format;

namespace lib2
{
    void formatter<std::string_view>::format(std::string_view str, format_context& ctx) const
    {
        const auto width {this->get_width(ctx)};
        const auto precision {this->get_precision(ctx)};

        auto utf_view {views::utf_codepoints(str)};

        if (precision)
        {
            auto utf_it {utf_view.begin()};
            std::ranges::advance(utf_it, *precision, utf_view.end());
            const auto leftover {utf_it.leftover()};
            str.remove_prefix(str.size() - std::string_view{leftover.begin(), leftover.end()}.size());
        }

        std::string debug_str;
        if (debug)
        {
            debug_str = lib2::format<"{}">(escaped(str));
            str = debug_str;
        }

        utf_view = views::utf_codepoints(str);

        const auto size {std::ranges::distance(utf_view)};

        if (width > size)
        {
            const auto padding {width - size};
            switch (align)
            {
            case fill_align_parser::align_type::left:
                ctx.stream.write(str);
                ctx.stream.fill(fill, padding);
                break;
            case fill_align_parser::align_type::right:
                ctx.stream.fill(fill, padding);
                ctx.stream.write(str);
                break;
            case fill_align_parser::align_type::center:
                {
                    const auto left_pad {padding / 2};
                    const auto right_pad {padding - left_pad};

                    ctx.stream.fill(fill, left_pad);
                    ctx.stream.write(str);
                    ctx.stream.fill(fill, right_pad);
                }
                break;
            }
        }
        else
        {
            ctx.stream.write(str);
        }
    }
}