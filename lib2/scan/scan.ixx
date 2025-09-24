export module lib2.scan;
export import :scan_error;
export import :scanner;
export import :basic_scan_parse_context;
export import :basic_scan_arg;
export import :basic_scan_context;

import std;

namespace lib2
{
    template<std::input_iterator I, std::sentinel_for<I> S, class CharT, class... Args>
    I do_scan(const std::locale& loc, I begin, const S end, const std::basic_string_view<CharT> fmt, Args&... args)
    {
        const auto& facet {std::use_facet<std::ctype<CharT>>(loc)};

        using ctx_t = basic_scan_context<I, S, CharT>;

        auto arg_store {make_scan_args<ctx_t>(args...)};
        basic_scan_args<ctx_t> scan_args {arg_store};
        basic_scan_parse_context<CharT> parse_ctx {fmt};

        auto fmt_it {fmt.begin()};

        while (fmt_it != fmt.end() && begin != end)
        {
            if (*fmt_it != '{')
            {
                if (facet.is(std::ctype_base::space, *begin) && facet.is(std::ctype_base::space, *fmt_it))
                {
                    ++begin;
                    while (begin != end)
                    {
                        if (!facet.is(std::ctype_base::space, *begin))
                        {
                            break;
                        }
                        ++begin;
                    }
                    while (fmt_it != fmt.end())
                    {
                        if (!facet.is(std::ctype_base::space, *fmt_it))
                        {
                            break;
                        }
                        ++fmt_it;
                    }
                    continue;
                }

                if (*begin != *fmt_it)
                {
                    throw scan_pattern_not_matched{std::format("Pattern not matched: '{}' != '{}'", *begin, *fmt_it)};
                }

                ++begin;
                ++fmt_it;
                continue;
            }

            ++fmt_it;
            if (*fmt_it == '{')
            {
                if (*begin != '{')
                {
                    throw scan_pattern_not_matched{std::format("Pattern not matched: '{}' != '{{'", *begin)};
                }

                ++begin;
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
                    const std::string_view sint {fmt_it, fmt.end()};
                    const auto conv_result {std::from_chars(sint.data(), sint.data() + sint.size(), arg_idx)};
                    if (conv_result.ec != std::errc{})
                    {
                        throw scan_parse_error{"Invalid format string"};
                    }
                    parse_ctx.check_arg_id(arg_idx);
                    fmt_it += conv_result.ptr - sint.data();

                    if (fmt_it == fmt.end())
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
                    arg_idx = parse_ctx.next_arg_id();
                    break;
                default:
                    throw scan_parse_error{"Invalid format string"};
            }

            const auto end_curly {std::find(fmt_it, fmt.end(), '}')};
            if (end_curly == fmt.end())
            {
                throw scan_parse_error{"Unmatched '{' in format string"};
            }

            parse_ctx.advance_to(fmt_it);

            basic_scan_context<I, S, CharT> ctx {begin, end, scan_args, loc};
            auto arg {ctx.arg(arg_idx)};
            begin = arg.scan(parse_ctx, ctx);
            fmt_it = end_curly;
            ++fmt_it;
        }

        if (fmt_it != fmt.end() && begin == end)
        {
            throw scan_pattern_not_matched{"Unexpected end of file"};
        }

        return begin;
    }

    export
    template<std::input_iterator I, std::sentinel_for<I> S, class... Args>
    auto scan(const std::locale& loc, I begin, S end, const std::string_view fmt, Args&... args)
    {
        return do_scan(loc, std::move(begin), std::move(end), fmt, args...);
    }

    export
    template<std::input_iterator I, std::sentinel_for<I> S, class... Args>
    auto scan(const std::locale& loc, I begin, S end, const std::wstring_view fmt, Args&... args)
    {
        return do_scan(loc, std::move(begin), std::move(end), fmt, args...);
    }

    export
    template<std::ranges::input_range R, class... Args>
    auto scan(const std::locale& loc, R&& r, const std::string_view fmt, Args&... args)
    {
        return scan(loc, std::ranges::begin(r), std::ranges::end(r), fmt, args...);
    }

    export
    template<std::ranges::input_range R, class... Args>
    auto scan(const std::locale& loc, R&& r, const std::wstring_view fmt, Args&... args)
    {
        return scan(loc, std::ranges::begin(r), std::ranges::end(r), fmt, args...);
    }

    export
    template<std::input_iterator I, std::sentinel_for<I> S, class... Args>
    auto scan(I begin, S end, const std::string_view fmt, Args&... args)
    {
        return scan({}, std::move(begin), std::move(end), fmt, args...);
    }

    export
    template<std::input_iterator I, std::sentinel_for<I> S, class... Args>
    auto scan(I begin, S end, const std::wstring_view fmt, Args&... args)
    {
        return scan({}, std::move(begin), std::move(end), fmt, args...);
    }

    export
    template<std::ranges::input_range R, class... Args>
    auto scan(R&& r, const std::string_view fmt, Args&... args)
    {
        return scan(std::ranges::begin(r), std::ranges::end(r), fmt, args...);
    }

    export
    template<std::ranges::input_range R, class... Args>
    auto scan(R&& r, const std::wstring_view fmt, Args&... args)
    {
        return scan(std::ranges::begin(r), std::ranges::end(r), fmt, args...);
    }

    template<class CharT>
    struct base_parser
    {
    public:
        template<class ParseCtx>
        constexpr auto parse(ParseCtx& ctx) const noexcept
        {
            return ctx.begin();
        }
    protected:
        template<class ScanCtx>
        auto skipws(ScanCtx& ctx) const
        {
            const auto& facet {std::use_facet<std::ctype<CharT>>(ctx.locale())};

            auto it {ctx.begin()};
            while (it != ctx.end())
            {
                if (!facet.is(std::ctype_base::space, *it))
                {
                    break;
                }
                ++it;
            }

            return it;
        }
    };

    template<class CharT>
    struct locale_parser : base_parser<CharT>
    {
    public:
        template<class ParseCtx>
        constexpr auto parse(ParseCtx& ctx) noexcept
        {
            auto it {ctx.begin()};
            if (it != ctx.end())
            {
                if (*it == 'L')
                {
                    ++it;
                    use_local = true;
                }
            }

            return it;
        }
    protected:
        bool use_local {false};
    };

    template<class CharT>
    struct width_parser : base_parser<CharT>
    {
    public:
        template<class ParseCtx>
        constexpr auto parse(ParseCtx& ctx) noexcept
        {
            auto it {ctx.begin()};
            if (it != ctx.end())
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
                        width = *it - '0';
                        ++it;
                        while (it != ctx.end())
                        {
                            bool stop {false};
                            switch (*it)
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
                                    width *= 10;
                                    width += *it - '0';
                                    break;
                                default:
                                    stop = true;
                            }
                            if (stop)
                            {
                                break;
                            }
                            ++it;
                        }
                }
            }
            
            return it;
        }
    protected:
        std::size_t width {0};
    };

    template<class CharT>
    struct string_parser : width_parser<CharT>
    {
    public:
        template<class ParseCtx>
        auto parse(ParseCtx& ctx)
        {
            auto it {width_parser<CharT>::parse(ctx)};

            if (it != ctx.end())
            {
                switch (*it)
                {
                    case 'c':
                        skipws_ = false;
                        ++it;
                }
            }

            if (!skipws_ && this->width == 0)
            {
                throw scan_parse_error{"Must specify a maximum width when not skipping space"};
            }
            return it;
        }
    protected:
        bool skipws_ {true};
    };

    export
    template<class CharT>
    struct scanner<CharT, CharT> : base_parser<CharT>
    {
        template<class ScanCtx>
        auto scan(CharT& val, ScanCtx& ctx) const
        {
            auto iter {ctx.begin()};

            if (iter == ctx.end())
            {
                throw std::invalid_argument{"EOF"};
            }

            val = *iter++;
            return iter;
        }
    };

    export
    template<class CharT, std::size_t N>
    struct scanner<CharT[N], CharT> : public string_parser<CharT>
    {
        template<class ScanCtx>
        auto scan(CharT(&val)[N], ScanCtx& ctx) const
        {
            const auto& facet {std::use_facet<std::ctype<CharT>>(ctx.locale())};

            auto it {ctx.begin()};
            if (this->skipws_)
            {
                it = this->skipws(ctx);
            }

            std::size_t current_width {0};
            auto ptr {val};

            while (it != ctx.end() && ptr != (val + N - 1) && (this->width == 0 || current_width < this->width))
            {
                if (this->skipws_ && facet.is(std::ctype_base::space, *it))
                {
                    break;
                }

                *ptr = *it;
                ++ptr;
                ++it;
                ++current_width;
            }

            *ptr = CharT{};

            return it;
        }
    };

    export
    template<class CharT, class Traits, class Allocator>
    struct scanner<std::basic_string<CharT, Traits, Allocator>, CharT> : public string_parser<CharT>
    {
        template<class ScanCtx>
        auto scan(std::basic_string<CharT, Traits, Allocator>& val, ScanCtx& ctx) const
        {
            const auto& facet {std::use_facet<std::ctype<CharT>>(ctx.locale())};
            val.clear();

            auto it {ctx.begin()};
            if (this->skipws_)
            {
                it = this->skipws(ctx);
            }

            std::size_t current_width {0};

            while (it != ctx.end() && (this->width == 0 || current_width < this->width))
            {
                if (this->skipws_ && facet.is(std::ctype_base::space, *it))
                {
                    break;
                }

                val.push_back(*it);
                ++it;
                ++current_width;
            }

            return it;
        }
    };

    export
    template<class CharT, class Traits>
    struct scanner<std::basic_string_view<CharT, Traits>, CharT> : public string_parser<CharT>
    {
        template<class ScanCtx>
        auto scan(std::basic_string_view<CharT, Traits>& val, ScanCtx& ctx) const
        {
            static_assert(std::contiguous_iterator<typename ScanCtx::iterator>, "Scan iterator must be contiguous to scan std::string_view");

            const auto& facet {std::use_facet<std::ctype<CharT>>(ctx.locale())};
            val = {};

            auto it {ctx.begin()};
            if (this->skipws_)
            {
                it = this->skipws(ctx);
            }
            const auto begin {it};

            std::size_t current_width {0};

            while (it != ctx.end() && (this->width == 0 || current_width < this->width))
            {
                if (this->skipws_ && facet.is(std::ctype_base::space, *it))
                {
                    break;
                }

                ++it;
                ++current_width;
            }

            val = {begin, it};

            return it;
        }
    };
}