export module lib2.fmt:chrono;

import std;

import lib2.concepts;

import :utility;
import :context;
import :parsers;
import :format;

namespace lib2
{
    // MSVC Bug: Not all can be static.
    template<class R>
    struct chrono_suffix {};

    static inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::atto>)
    {
        return os << "as";
    }

    static inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::femto>)
    {
        return os << "fs";
    }

    static inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::pico>)
    {
        return os << "ps";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::nano>)
    {
        return os << "ns";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::micro>)
    {
        return os << "us";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::milli>)
    {
        return os << "ms";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::centi>)
    {
        return os << "cs";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::deci>)
    {
        return os << "ds";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::ratio<1>>)
    {
        os.put('s');
        return os;
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::deca>)
    {
        return os << "das";
    }

    static inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::hecto>)
    {
        return os << "hs";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::kilo>)
    {
        return os << "ks";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::mega>)
    {
        return os << "Ms";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::giga>)
    {
        return os << "Gs";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::tera>)
    {
        return os << "Ts";
    }
    
    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::peta>)
    {
        return os << "Ps";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::exa>)
    {
        return os << "Es";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::ratio<60>>)
    {
        return os << "min";
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::ratio<3600>>)
    {
        os.put('h');
        return os;
    }

    inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::ratio<86400>>)
    {
        os.put('d');
        return os;
    }

    template<std::intmax_t Num>
    static inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::ratio<Num, 1>>)
    {
        return os << '[' << Num << "]s";
    }

    template<std::intmax_t Num, std::intmax_t Den>
    static inline text_ostream& operator<<(text_ostream& os, const chrono_suffix<std::ratio<Num, Den>>)
    {
        return os << '[' << Num << '/' << Den << "]s";
    }

    template<class Fmt>
    class chrono_spec
    {
    public:
        constexpr chrono_spec() noexcept = default;

        constexpr chrono_spec(const std::string_view fmt)
            : fmt_{fmt}
        {
            validate(fmt);
        }

        constexpr chrono_spec(const chrono_spec&) noexcept = default;
        constexpr chrono_spec& operator=(const chrono_spec&) noexcept = default;

        constexpr std::string_view fmt() const noexcept
        {
            return fmt_;
        }
    private:
        std::string_view fmt_;

        static constexpr void validate(const std::string_view fmt)
        {
            bool in_format {false};
            char mod {};

            for (const auto ch : fmt)
            {
                if (in_format)
                {
                    switch (ch)
                    {
                    case 'O':
                    case 'E':
                        if (mod)
                        {
                            // throw std::format_error{lib2::format("Invalid chrono spec: %{}{}", mod, ch)};
                        }
                        mod = ch;
                        continue;
                    case '%':
                    case 'n':
                    case 't':
                        if (mod)
                        {
                            // throw std::format_error{lib2::format("Invalid chrono spec: %{}{}", mod, ch)};
                        }
                        break;
                    default:
                        if (!Fmt::validate_spec(ch, mod))
                        {
                            // if (mod)
                            // {
                            //     throw std::format_error{lib2::format("Invalid chrono spec: %{}{}", mod, ch)};
                            // }
                            // throw std::format_error{lib2::format("Invalid chrono spec: %{}", ch)};
                        }
                    }
                    in_format = false;
                }
                else
                {
                    in_format = ch == '%';
                }
            }

            if (in_format)
            {
                throw std::format_error{"Invalid chrono spec: %"};
            }
        }
    };

    template<class CRTP>
    struct chrono_fmt
    {
        std::locale get_locale(this const auto& self) noexcept
        {
            if (self.use_locale)
            {
                if (self.locale)
                {
                    return *self.locale;
                }
                return {};
            }

            return std::locale::classic();
        }

        static constexpr CRTP parse(format_context& ctx)
        {
            CRTP fmt;
            auto it {parse_fill_align(ctx, fmt.fill, fmt.align)};
            ctx.advance_to(it);
            it = parse_width(ctx, fmt.width);
            ctx.advance_to(it);
            if constexpr (requires(const CRTP& fmt) { fmt.precision; })
            {
                it = parse_precision(ctx, fmt.precision);
                ctx.advance_to(it);
            }
            it = parse_locale(ctx, fmt.use_locale);
            fmt.locale = ctx.locale();

            const auto end {std::find(it, ctx.end(), '}')};
            fmt.spec = std::string_view{it, end};
            ctx.advance_to(end);
            return fmt;
        }

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            switch (spec)
            {
            case '%':
            case 'n':
            case 't':
                return mod == 0;
            default:
                return false;
            }
        }
    };

    struct tm_fmt
    {
        char fill {' '};
        std::string_view spec;
        const std::locale& locale;
    };

    template<class R>
    struct denom_10th;

    template<std::intmax_t N>
    struct denom_10th<std::ratio<N, 1>> : std::integral_constant<std::intmax_t, 1> {};

    template<std::intmax_t N, std::intmax_t D>
    struct denom_10th<std::ratio<N, D>> : std::integral_constant<std::intmax_t, 1 + denom_10th<std::ratio<N, D / 10>>::value> {};

    template<class Rep = int, class Period = std::ratio<1>>
    text_ostream& to_stream(text_ostream& os, const std::tm& value, const tm_fmt& fmt, const std::chrono::duration<Rep, Period> d = {})
    {
        std::ios ios{nullptr};
        ios.imbue(fmt.locale);
        const auto& time_facet {std::use_facet<std::time_put<char, ostream_iterator<char>>>(fmt.locale)};

        bool in_format {false};
        char mod {};

        for (const auto ch : fmt.spec)
        {
            if (in_format)
            {
                if (mod)
                {
                    time_facet.put(ostream_iterator{os}, ios, fmt.fill, &value, ch, mod);
                }
                else
                {
                    switch (ch)
                    {
                    case 'O':
                    case 'E':
                        mod = ch;
                        continue;
                    case '%':
                        os.put('%');
                        break;
                    case 'n':
                        os.put('\n');
                        break;
                    case 't':
                        os.put('\t');
                        break;
                    case 'Q':
                        os << d.count();
                        break;
                    case 'q':
                        os << chrono_suffix<Period>{};
                        break;
                    case 'd':
                        to_stream(os, value.tm_mday, {.fill='0', .width=2});
                        break;
                    case 'e':
                        to_stream(os, value.tm_mday, {.fill=' ', .width=2});
                        break;
                    case 'm':
                        to_stream(os, value.tm_mon + 1, {.fill='0', .width=2});
                        break;
                    case 'C':
                        to_stream(os, (value.tm_year + 1900) / 100, {.fill='0', .width=2});
                        break;
                    case 'Y':
                        to_stream(os, value.tm_year + 1900, {.fill='0', .width=4});
                        break;
                    case 'y':
                        to_stream(os, (value.tm_year + 1900) % 100, {.fill='0', .width=2});
                        break;
                    case 'u':
                        os << (value.tm_wday + 1);
                        break;
                    case 'w':
                        os << value.tm_wday;
                        break;
                    case 'H':
                        to_stream(os, value.tm_hour, {.fill='0', .width=2});
                        break;
                    case 'M':
                        to_stream(os, value.tm_min, {.fill='0', .width=2});
                        break;
                    case 'S':
                        if constexpr (std::chrono::treat_as_floating_point_v<Rep>)
                        {
                            const std::chrono::hh_mm_ss hhmmss {d};
                            to_stream(os, (d - hhmmss.hours() - hhmmss.minutes()).count(), {.precision=std::size_t(std::min(std::intmax_t{6}, denom_10th<Period>::value))});
                        }
                        else
                        {
                            to_stream(os, value.tm_sec, {.fill='0', .width=2});
                        }
                        break;
                    default:
                        time_facet.put(ostream_iterator{os}, ios, fmt.fill, &value, ch, mod);
                        break;
                    }
                }

                in_format = false;
            }
            else
            {
                switch (ch)
                {
                case '%':
                    in_format = true;
                    break;
                default:
                    os.put(ch);
                }
            }
        }

        return os;
    }

    export
    struct day_fmt : public chrono_fmt<day_fmt>
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        chrono_spec<day_fmt> spec {};

        bool use_locale {false};
        const std::locale* locale;

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            switch (spec)
            {
            case 'd':
            case 'e':
                return mod == 0 || mod == 'O';
            default:
                return chrono_fmt<day_fmt>::validate_spec(spec, mod);
            }
        }
    };

    export
    text_ostream& operator<<(text_ostream& os, const std::chrono::day d)
    {
        to_stream(os, unsigned{d}, {.fill='0', .width=2});
        if (!d.ok())
        {
            os << " is not a valid day";
        }
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::day d, const day_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            ref_os << d;
        }
        else
        {
            const std::tm time {.tm_mday=int(unsigned{d})};
            to_stream(ref_os, time, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .locale=fmt.get_locale()});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    export
    struct month_fmt : public chrono_fmt<month_fmt>
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        chrono_spec<month_fmt> spec {};

        bool use_locale {false};
        const std::locale* locale;

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            switch (spec)
            {
            case 'b':
            case 'h':
            case 'B':
                return mod == 0;
            case 'm':
                return mod == 0 || mod == 'O';
            default:
                return chrono_fmt<month_fmt>::validate_spec(spec, mod);
            }
        }
    };

    static void empty_spec(text_ostream& os, const std::chrono::month m, const std::locale& loc = {});

    export
    text_ostream& operator<<(text_ostream& os, const std::chrono::month m)
    {
        if (m.ok())
        {
            empty_spec(os, m);
        }
        else
        {
            os << unsigned{m} << " is not a valid month";
        }
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::month m, const month_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(os, m, fmt.get_locale());
        }
        else
        {
            const std::tm time {.tm_mon=int(unsigned{m}-1)};
            to_stream(ref_os, time, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .locale=fmt.get_locale()});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    static void empty_spec(text_ostream& os, const std::chrono::month m, const std::locale& loc)
    {
        to_stream(os, m, {.spec="%b", .use_locale=true, .locale = &loc});
    }

    export
    struct year_fmt : public chrono_fmt<year_fmt>
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        chrono_spec<year_fmt> spec {};

        bool use_locale {false};
        const std::locale* locale;

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            switch (spec)
            {
            case 'C':
            case 'Y':
                return mod == 0 || mod == 'E';
            case 'y':
                return true;
            default:
                return false;
            }
        }
    };

    export
    text_ostream& operator<<(text_ostream& os, const std::chrono::year y)
    {
        to_stream(os, int{y}, {.fill='0', .width=4});
        if (!y.ok())
        {
            os << " is not a valid year";
        }
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::year y, const year_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            ref_os << y;
        }
        else
        {
            const std::tm time {.tm_year=int{y} - 1900};
            to_stream(ref_os, time, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .locale=fmt.get_locale()});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    export
    struct weekday_fmt : public chrono_fmt<weekday_fmt>
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        chrono_spec<weekday_fmt> spec {};

        bool use_locale {false};
        const std::locale* locale;

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            switch (spec)
            {
            case 'a':
            case 'A':
                return mod == 0;
            case 'u':
            case 'w':
                return mod == 0 || mod == 'O';
            default:
                return chrono_fmt<weekday_fmt>::validate_spec(spec, mod);
            }
        }
    };

    static inline void empty_spec(text_ostream& os, const std::chrono::weekday w, const std::locale& loc = {});

    export
    inline text_ostream& operator<<(text_ostream& os, const std::chrono::weekday w)
    {
        empty_spec(os, w);
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::weekday w, const weekday_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(ref_os, w, fmt.get_locale());
        }
        else
        {
            const std::tm time {.tm_wday=int(w.c_encoding())};
            to_stream(ref_os, time, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .locale=fmt.get_locale()});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    static inline void empty_spec(text_ostream& os, const std::chrono::weekday w, const std::locale& loc)
    {
        if (w.ok())
        {
            to_stream(os, w, {.spec="%a", .use_locale=true, .locale=&loc});
        }
        else
        {
            os << w.c_encoding() << " is not a valid weekday";
        }
    }

    static inline void empty_spec(text_ostream& os, const std::chrono::weekday_indexed wi, const std::locale& loc = {})
    {
        to_stream(os, wi.weekday(), {.use_locale=true, .locale=&loc});
        os.put('[');
        os << wi.index();
        if (wi.index() < 1 || wi.index() > 5)
        {
            os << " is not a valid index";
        }
        os.put(']');
    }

    export
    inline text_ostream& operator<<(text_ostream& os, const std::chrono::weekday_indexed wi)
    {
        empty_spec(os, wi);
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::weekday_indexed wi, const weekday_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(ref_os, wi, fmt.get_locale());
        }
        else
        {
            const auto loc {fmt.get_locale()};
            to_stream(ref_os, wi.weekday(), {.fill=fmt.fill, .spec=fmt.spec, .locale=&loc});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    static inline void empty_spec(text_ostream& os, const std::chrono::weekday_last wl, const std::locale& loc = {})
    {
        to_stream(os, wl.weekday(), {.use_locale=true, .locale=&loc});
        os << "[last]";
    }

    export
    inline text_ostream& operator<<(text_ostream& os, const std::chrono::weekday_last wl)
    {
        empty_spec(os, wl);
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::weekday_last wl, const weekday_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(ref_os, wl, fmt.get_locale());
        }
        else
        {
            const auto loc {fmt.get_locale()};
            to_stream(ref_os, wl.weekday(), {.fill=fmt.fill, .spec=fmt.spec.fmt(), .use_locale=true, .locale=&loc});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    export
    struct month_day_fmt : public chrono_fmt<month_day_fmt>
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        chrono_spec<month_day_fmt> spec {};

        bool use_locale {false};
        const std::locale* locale;

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            switch (spec)
            {
            case 'b':
            case 'h':
            case 'B':
                return mod == 0;
            case 'm':
                return mod == 0 || mod == 'O';
            default:
                return day_fmt::validate_spec(spec, mod);
            }
        }
    };

    static inline void empty_spec(text_ostream& os, const std::chrono::month_day md, const std::locale& loc = {})
    {
        empty_spec(os, md.month(), loc);
        os.put('/');
        os << md.day();
    }

    export
    inline text_ostream& operator<<(text_ostream& os, const std::chrono::month_day md)
    {
        empty_spec(os, md);
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::month_day md, const month_day_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(ref_os, md, fmt.get_locale());
        }
        else
        {
            const std::tm time {.tm_mday=int(unsigned{md.day()}), .tm_mon=int(unsigned{md.month()}-1)};
            to_stream(ref_os, time, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .locale=fmt.get_locale()});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    static inline void empty_spec(text_ostream& os, const std::chrono::month_day_last mdl, const std::locale& loc = {})
    {
        empty_spec(os, mdl.month(), loc);
        os << "/last";
    }

    export
    inline text_ostream& operator<<(text_ostream& os, const std::chrono::month_day_last mdl)
    {
        empty_spec(os, mdl);
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::month_day_last mdl, const month_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(ref_os, mdl, fmt.get_locale());
        }
        else
        {
            const auto loc {fmt.get_locale()};
            to_stream(ref_os, mdl.month(), {.fill=fmt.fill, .spec=fmt.spec.fmt(), .use_locale=true, .locale=&loc});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    export
    struct month_weekday_fmt : public chrono_fmt<month_weekday_fmt>
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        chrono_spec<month_weekday_fmt> spec {};

        bool use_locale {false};
        const std::locale* locale;

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            return month_fmt::validate_spec(spec, mod) ||
                   weekday_fmt::validate_spec(spec, mod);
        }
    };

    static inline void empty_spec(text_ostream& os, const std::chrono::month_weekday mw, const std::locale& loc = {})
    {
        empty_spec(os, mw.month(), loc);
        empty_spec(os, mw.weekday_indexed(), loc);
    }

    export
    text_ostream& operator<<(text_ostream& os, const std::chrono::month_weekday mw)
    {
        empty_spec(os, mw);
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::month_weekday mw, const month_weekday_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(ref_os, mw, fmt.get_locale());
        }
        else
        {
            const std::tm time {.tm_mon=int(unsigned{mw.month()}-1), .tm_wday=int(mw.weekday_indexed().weekday().c_encoding())};
            to_stream(ref_os, time, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .locale=fmt.get_locale()});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    static inline void empty_spec(text_ostream& os, const std::chrono::month_weekday_last mwl, const std::locale& loc = {})
    {
        empty_spec(os, mwl.month(), loc);
        os.put('/');
        empty_spec(os, mwl.weekday_last(), loc);
    }

    export
    inline text_ostream& operator<<(text_ostream& os, const std::chrono::month_weekday_last mwl)
    {
        empty_spec(os, mwl);
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::month_weekday_last mwl, const month_weekday_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(ref_os, mwl, fmt.get_locale());
        }
        else
        {
            const std::tm time {.tm_mon=int(unsigned{mwl.month()}-1), .tm_wday=int(mwl.weekday_last().weekday().c_encoding())};
            to_stream(ref_os, time, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .locale=fmt.get_locale()});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    export
    struct year_month_fmt : public chrono_fmt<year_month_fmt>
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        chrono_spec<year_month_fmt> spec {};

        bool use_locale {false};
        const std::locale* locale;

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            return month_fmt::validate_spec(spec, mod) ||
                   year_fmt::validate_spec(spec, mod);
        }
    };

    static inline void empty_spec(text_ostream& os, const std::chrono::year_month ym, const std::locale& loc = {})
    {
        os << ym.year();
        os.put('/');
        empty_spec(os, ym.month(), loc);
    }

    export
    inline text_ostream& operator<<(text_ostream& os, const std::chrono::year_month ym)
    {
        empty_spec(os, ym);
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::year_month ym, const year_month_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(ref_os, ym, fmt.get_locale());
        }
        else
        {
            const std::tm time {.tm_mon=int(unsigned{ym.month()}-1), .tm_year=int{ym.year()}-1900};
            to_stream(ref_os, time, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .locale=fmt.get_locale()});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    export
    struct year_month_day_fmt : public chrono_fmt<year_month_day_fmt>
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        chrono_spec<year_month_day_fmt> spec {};

        bool use_locale {false};
        const std::locale* locale;

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            return month_fmt::validate_spec(spec, mod) ||
                   year_fmt::validate_spec(spec, mod) ||
                   day_fmt::validate_spec(spec, mod);
        }
    };

    export
    text_ostream& operator<<(text_ostream& os, const std::chrono::year_month_day ymd)
    {
        to_stream(os, int{ymd.year()} % 100, {.fill='0', .width=2});
        to_stream(os, unsigned{ymd.month()}, {.fill='0', .width=2});
        to_stream(os, unsigned{ymd.day()}, {.fill='0', .width=2});

        if (!ymd.ok())
        {
            os << " is not a valid date";
        }

        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::year_month_day ymd, const year_month_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            os << ymd;
        }
        else
        {
            const std::tm time {.tm_mday=int(unsigned{ymd.day()}), .tm_mon=int(unsigned{ymd.month()}-1), .tm_year=int{ymd.year()}-1900};
            to_stream(ref_os, time, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .locale=fmt.get_locale()});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    static inline void empty_spec(text_ostream& os, const std::chrono::year_month_day_last ymdl, const std::locale& loc = {})
    {
        os << ymdl.year();
        os.put('/');
        empty_spec(os, ymdl.month_day_last(), loc);
    }

    export
    inline text_ostream& operator<<(text_ostream& os, const std::chrono::year_month_day_last ymdl)
    {
        empty_spec(os, ymdl);
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::year_month_day_last ymdl, const year_month_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(os, ymdl, fmt.get_locale());
        }
        else
        {
            const auto loc {fmt.get_locale()};
            to_stream(ref_os, ymdl.year()/ymdl.month()/ymdl.day(), {.fill=fmt.fill, .spec=fmt.spec.fmt(), .use_locale=true, .locale=&loc});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    export
    struct year_month_weekday_fmt : public chrono_fmt<year_month_weekday_fmt>
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        chrono_spec<year_month_weekday_fmt> spec {};

        bool use_locale {false};
        const std::locale* locale;

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            return month_fmt::validate_spec(spec, mod) ||
                   year_fmt::validate_spec(spec, mod) ||
                   weekday_fmt::validate_spec(spec, mod);
        }
    };

    static inline void empty_spec(text_ostream& os, const std::chrono::year_month_weekday ymw, const std::locale& loc = {})
    {
        os << ymw.year();
        os.put('/');
        empty_spec(os, ymw.month(), loc);
        os.put('/');
        empty_spec(os, ymw.weekday_indexed(), loc);
    }

    export
    inline text_ostream& operator<<(text_ostream& os, const std::chrono::year_month_weekday ymw)
    {
        empty_spec(os, ymw);
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::year_month_weekday ymw, const year_month_weekday_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(os, ymw, fmt.get_locale());
        }
        else
        {
            const std::tm time {.tm_mon=int(unsigned{ymw.month()}-1), .tm_year=int{ymw.year()}-1900, .tm_wday=int(ymw.weekday().c_encoding())};
            to_stream(ref_os, time, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .locale=fmt.get_locale()});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    static inline void empty_spec(text_ostream& os, const std::chrono::year_month_weekday_last ymwl, const std::locale& loc = {})
    {
        os << ymwl.year();
        os.put('/');
        empty_spec(os, ymwl.month(), loc);
        os.put('/');
        empty_spec(os, ymwl.weekday_last(), loc);
    }

    export
    inline text_ostream& operator<<(text_ostream& os, const std::chrono::year_month_weekday_last ymwl)
    {
        empty_spec(os, ymwl);
        return os;
    }

    export
    text_ostream& to_stream(text_ostream& os, const std::chrono::year_month_weekday_last ymwl, const year_month_weekday_fmt& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            empty_spec(os, ymwl, fmt.get_locale());
        }
        else
        {
            const auto loc {fmt.get_locale()};
            to_stream(ref_os, std::chrono::year_month_weekday{static_cast<std::chrono::sys_days>(ymwl)}, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .use_locale=true, .locale=&loc});
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }

    export
    template<class Rep, class Period>
    struct duration_fmt : public chrono_fmt<duration_fmt<Rep, Period>>
    {
        char fill {' '};
        align_type align {align_type::left};
        std::size_t width {0};
        chrono_spec<duration_fmt<Rep, Period>> spec {};

        bool use_locale {false};
        const std::locale* locale;

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            switch (spec)
            {
            case 'H':
            case 'I':
            case 'M':
            case 'S':
                return mod == 0 || mod == 'O';
            case 'p':
            case 'R':
            case 'T':
            case 'r':
            case 'Q':
            case 'q':
                return mod == 0;
            case 'X':
                return mod == 0 || mod == 'E';
            default:
                return chrono_fmt<duration_fmt<Rep, Period>>::validate_spec(spec, mod);
            }
        }
    };

    export
    template<class Rep, class Period>
    inline text_ostream& operator<<(text_ostream& os, const std::chrono::duration<Rep, Period> d)
    {
        return os << d.count() << chrono_suffix<Period>{};
    }

    export
    template<class Rep, class Period>
    text_ostream& to_stream(text_ostream& os, const std::chrono::duration<Rep, Period> d, const duration_fmt<Rep, Period>& fmt)
    {
        ostringstream temp_os;
        text_ostream& ref_os {fmt.width ? temp_os : os};

        if (fmt.spec.fmt().empty())
        {
            os << d;
        }
        else
        {
            const std::chrono::hh_mm_ss<std::chrono::duration<Rep, Period>> hhmmss{d};
            const std::tm time {
                .tm_sec  = int(hhmmss.seconds().count())
            ,   .tm_min  = int(hhmmss.minutes().count())
            ,   .tm_hour = int(hhmmss.hours().count())
            };
            to_stream(ref_os, time, {.fill=fmt.fill, .spec=fmt.spec.fmt(), .locale=fmt.get_locale()}, d);
        }

        if (fmt.width)
        {
            const str_fmt s_fmt {
                .fill  = fmt.fill,
                .align = fmt.align,
                .width = fmt.width
            };

            to_stream(os, temp_os, s_fmt);
        }

        return os;
    }
}

namespace std
{
    export
    lib2::day_fmt format_of(std::chrono::day);

    export
    lib2::month_fmt format_of(std::chrono::month);
    
    export
    lib2::year_fmt format_of(std::chrono::year);

    export
    lib2::weekday_fmt format_of(std::chrono::weekday);

    export
    lib2::weekday_fmt format_of(std::chrono::weekday_indexed);

    export
    lib2::weekday_fmt format_of(std::chrono::weekday_last);

    export
    lib2::month_day_fmt format_of(std::chrono::month_day);

    export
    lib2::month_fmt format_of(std::chrono::month_day_last);

    export
    lib2::month_weekday_fmt format_of(std::chrono::month_weekday);

    export
    lib2::month_weekday_fmt format_of(std::chrono::month_weekday_last);

    export
    lib2::year_month_fmt format_of(std::chrono::year_month);

    export
    lib2::year_month_day_fmt format_of(std::chrono::year_month_day);

    export
    lib2::year_month_day_fmt format_of(std::chrono::year_month_day_last);

    export
    lib2::year_month_weekday_fmt format_of(std::chrono::year_month_weekday);

    export
    lib2::year_month_weekday_fmt format_of(std::chrono::year_month_weekday_last);

    export
    template<class Rep, class Period>
    lib2::duration_fmt<Rep, Period> format_of(std::chrono::duration<Rep, Period>);
}