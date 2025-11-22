export module lib2.fmt:parsers;

import lib2.utility;
import lib2.compact_optional;

import :context;
import :utility;

namespace lib2
{
    export
    enum class align_type : char
    {
        left   = '<',
        right  = '>',
        center = '^'
    };

    export
    constexpr auto parse_fill_align(format_context& ctx, char& fill, align_type& align) noexcept
    {
        auto it {ctx.begin()};

        if (it != ctx.end() && *it != '}')
        {
            switch (*it)
            {
            case '<':
            case '>':
            case '^':
                align = static_cast<align_type>(*it);
                ++it;
                break;
            default:
                fill = *it;
                ++it;
                if (it == ctx.end())
                {
                    --it;
                }
                else
                {
                    switch (*it)
                    {
                    case '<':
                    case '>':
                    case '^':
                        align = static_cast<align_type>(*it);
                        ++it;
                        break;
                    default:
                        --it;
                        fill = ' ';
                        break;
                    }
                }
                break;
            }
        }

        return it;
    }

    export
    constexpr auto parse_range_fill_align(format_context& ctx, char& fill, align_type& align)
    {
        const auto it {parse_fill_align(ctx, fill, align)};

        switch (fill)
        {
        case '{':
        case '}':
        case ':':
            throw std::format_error{"Invalid fill character"};
        }

        return it;
    }

    export
    constexpr auto parse_width(format_context& ctx, std::size_t& width)
    {
        width = 0;

        auto it {ctx.begin()};
        const auto end {ctx.end()};

        if (it != end)
        {
            switch (*it)
            {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                width = parse_arg_id(it, end);
                break;
            case '{':
                ++it;
                if (it == end)
                {
                    throw std::format_error{"Unmatched '{' in format string"};
                }
                switch (*it)
                {
                case '}':
                    width = ctx.next_arg_id();
                    ++it;
                    break;
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
                    width = parse_arg_id(it, end);
                    ctx.check_arg_id(width);
                    break;
                default:
                    throw std::format_error{"Unmatched '{' in format string."};
                }
                width = ctx.arg(width).visit(lib2::overloaded {
                    [](const std::intmax_t value) -> std::size_t {
                        if (value < 0)
                        {
                            throw std::format_error{"Width cannot be negative."};
                        }
                        
                        return value;
                    }
                ,   [](const std::uintmax_t value) -> std::size_t {
                        return value;
                    }
                ,   [](const auto&) -> std::size_t {
                        throw std::format_error{"Width argument is not integral."};
                    }
                });
                break;
            }
        }

        return it;
    }

    export
    constexpr auto parse_precision(format_context& ctx, optional_size& precision)
    {
        auto it {ctx.begin()};
        const auto end {ctx.end()};

        if (it != end)
        {
            if (*it == '.')
            {
                ++it;
                if (it == end)
                {
                    throw std::format_error{"Invalid precision format"};
                }
                
                switch (*it)
                {
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    precision = parse_arg_id(it, end);
                    break;
                case '{':
                    ++it;
                    if (it == end)
                    {
                        throw std::format_error{"Unmatched '{' in format string"};
                    }

                    switch (*it)
                    {
                    case '}':
                        precision = ctx.next_arg_id();
                        ++it;
                        break;
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
                        precision = parse_arg_id(it, end);
                        ctx.check_arg_id(*precision);
                        break;
                    default:
                        throw std::format_error{"Unmatched '{' in format string."};
                    }
                    precision = ctx.arg(*precision).visit(lib2::overloaded {
                        [](const std::intmax_t value) -> std::size_t {
                            if (value < 0)
                            {
                                throw std::format_error{"Precision cannot be negative."};
                            }
                            return value;
                        }
                    ,   [](const std::uintmax_t value) -> std::size_t {
                            return value;
                        }
                    ,   [](const auto&) -> std::size_t {
                            throw std::format_error{"Precision argument is not integral."};
                        }
                    });
                    break;
                }
            }
        }

        return it;
    }

    export
    enum class sign_type : char
    {
        plus  = '+',
        minus = '-',
        space = ' '
    };

    export
    constexpr auto parse_sign(format_context& ctx, sign_type& sign) noexcept
    {
        auto it {ctx.begin()};
        const auto end {ctx.end()};

        if (it != end)
        {
            switch (*it)
            {
            case '+':
            case '-':
            case ' ':
                sign = static_cast<sign_type>(static_cast<char>(*it));
                ++it;
                break;
            }
        }

        return it;
    }

    export
    constexpr auto parse_alt(format_context& ctx, bool& alt, bool& zero) noexcept
    {
        auto it {ctx.begin()};
        const auto end {ctx.end()};

        if (it != end)
        {
            switch (*it)
            {
            case '#':
                alt = true;
                ++it;
                if (it == end || *it != '0')
                {
                    break;
                }
            case '0':
                zero = true;
                ++it;
            }
        }

        return it;
    }

    export
    constexpr auto parse_locale(format_context& ctx, bool& use_locale) noexcept
    {
        auto it {ctx.begin()};

        if (it != ctx.end())
        {
            if (*it == 'L')
            {
                use_locale = true;
                ++it;
            }
        }

        return it;
    }

    export
    constexpr auto parse_string(format_context& ctx, std::string_view& str)
    {
        auto it {ctx.begin()};
        const auto end {ctx.end()};

        if (it != end)
        {
            switch (*it)
            {
            case '{':
                {
                    std::size_t arg;
                    ++it;
                    if (it == end)
                    {
                        throw std::format_error{"Unmatched '{' in format string"};
                    }
                    switch (*it)
                    {
                    case '}':
                        arg = ctx.next_arg_id();
                        ++it;
                        break;
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
                        arg = parse_arg_id(it, end);
                        ctx.check_arg_id(arg);
                        break;
                    default:
                        throw std::format_error{"Unmatched '{' in format string."};
                    }
                    str = ctx.arg(arg).visit(lib2::overloaded {
                        [](const std::string_view value) -> std::string_view {
                            return value;
                        }
                    ,   [](const auto&) -> std::string_view {
                            throw std::format_error{"Argument is not string."};
                        }
                    });
                    break;
                }
            default:
                {
                    const auto end2 {std::find(it, end, '}')};
                    str = {it, end2};
                    it = end2;
                }
            }
        }

        return it;
    }
}