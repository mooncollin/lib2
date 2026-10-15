export module lib2.fmt:chrono;

import std;

import lib2.concepts;

import :formatter;
import :context;
import :parsers;
import :format;
import :string;
import :arithmetic;
import :misc;

namespace lib2
{
    template<class R>
    struct chrono_suffix {};

    template<std::intmax_t Num, std::intmax_t Den>
    struct formatter<chrono_suffix<std::ratio<Num, Den>>> : public format_parser
    {
        static void format(const chrono_suffix<std::ratio<Num, Den>>, format_context& ctx)
        {
            lib2::format_to<"[{}/{}]s">(ctx.out, Num, Den);
        }
    };

    template<std::intmax_t Num>
    struct formatter<chrono_suffix<std::ratio<Num, 1>>> : public format_parser
    {
        static void format(const chrono_suffix<std::ratio<Num, 1>>, format_context& ctx)
        {
            lib2::format_to<"[{}]s">(ctx.out, Num);
        }
    };

    template<>
    struct formatter<chrono_suffix<std::atto>> : public format_parser
    {
        static void format(const chrono_suffix<std::atto>, format_context& ctx)
        {
            ctx.out.write("as");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::femto>> : public format_parser
    {
        static void format(const chrono_suffix<std::femto>, format_context& ctx)
        {
            ctx.out.write("fs");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::pico>> : public format_parser
    {
        static void format(const chrono_suffix<std::pico>, format_context& ctx)
        {
            ctx.out.write("ps");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::nano>> : public format_parser
    {
        static void format(const chrono_suffix<std::nano>, format_context& ctx)
        {
            ctx.out.write("ns");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::micro>> : public format_parser
    {
        static void format(const chrono_suffix<std::micro>, format_context& ctx)
        {
            ctx.out.write("us");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::milli>> : public format_parser
    {
        static void format(const chrono_suffix<std::milli>, format_context& ctx)
        {
            ctx.out.write("ms");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::centi>> : public format_parser
    {
        static void format(const chrono_suffix<std::centi>, format_context& ctx)
        {
            ctx.out.write("cs");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::deci>> : public format_parser
    {
        static void format(const chrono_suffix<std::deci>, format_context& ctx)
        {
            ctx.out.write("ds");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::ratio<1>>> : public format_parser
    {
        static void format(const chrono_suffix<std::ratio<1>>, format_context& ctx)
        {
            ctx.out.put('s');
        }
    };

    template<>
    struct formatter<chrono_suffix<std::deca>> : public format_parser
    {
        static void format(const chrono_suffix<std::deca>, format_context& ctx)
        {
            ctx.out.write("das");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::hecto>> : public format_parser
    {
        static void format(const chrono_suffix<std::hecto>, format_context& ctx)
        {
            ctx.out.write("hs");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::kilo>> : public format_parser
    {
        static void format(const chrono_suffix<std::kilo>, format_context& ctx)
        {
            ctx.out.write("ks");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::mega>> : public format_parser
    {
        static void format(const chrono_suffix<std::mega>, format_context& ctx)
        {
            ctx.out.write("Ms");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::giga>> : public format_parser
    {
        static void format(const chrono_suffix<std::giga>, format_context& ctx)
        {
            ctx.out.write("Gs");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::tera>> : public format_parser
    {
        static void format(const chrono_suffix<std::tera>, format_context& ctx)
        {
            ctx.out.write("Ts");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::peta>> : public format_parser
    {
        static void format(const chrono_suffix<std::peta>, format_context& ctx)
        {
            ctx.out.write("Ps");
        }
    };
    
    template<>
    struct formatter<chrono_suffix<std::exa>> : public format_parser
    {
        static void format(const chrono_suffix<std::exa>, format_context& ctx)
        {
            ctx.out.write("Es");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::ratio<60>>> : public format_parser
    {
        static void format(const chrono_suffix<std::ratio<60>>, format_context& ctx)
        {
            ctx.out.write("min");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::ratio<3600>>> : public format_parser
    {
        static void format(const chrono_suffix<std::ratio<3600>>, format_context& ctx)
        {
            ctx.out.put('h');
        }
    };

    template<>
    struct formatter<chrono_suffix<std::ratio<86400>>> : public format_parser
    {
        static void format(const chrono_suffix<std::ratio<86400>>, format_context& ctx)
        {
            ctx.out.put('d');
        }
    };

    struct chrono_formatter : public width_parser
    {
    public:
        char fill {' '};
        fill_align_parser::align_type align {fill_align_parser::align_type::left};
        bool use_locale {false};

        template<class Self>
        constexpr auto parse(this Self& self, format_parse_context& ctx)
        {
            self = {};

            ctx.begin = fill_align_parser::parse(ctx, self.fill, self.align);
            ctx.begin = self.width_parser::parse(ctx);
            if constexpr (std::derived_from<Self, precision_parser>)
            {
                ctx.begin = self.precision_parser::parse(ctx);
            }
            ctx.begin = locale_parser::parse(ctx, self.use_locale);

            const auto end {std::find(ctx.begin, ctx.end, '}')};
            self.fmt(std::string_view{ctx.begin, end});
            return end;
        }

        [[nodiscard]] constexpr std::string_view fmt() const noexcept
        {
            return fmt_;
        }

        constexpr void fmt(this auto& self, const std::string_view fmt)
        {
            self.validate(fmt);
            self.fmt_ = fmt;
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
    protected:
        std::locale get_locale(const format_context& ctx) const noexcept
        {
            if (use_locale)
            {
                return ctx.locale();
            }

            return std::locale::classic();
        }

        template<class R>
        struct denom_10th;

        template<std::intmax_t N>
        struct denom_10th<std::ratio<N, 1>> : std::integral_constant<std::intmax_t, 1> {};

        template<std::intmax_t N, std::intmax_t D>
        struct denom_10th<std::ratio<N, D>> : std::integral_constant<std::intmax_t, 1 + denom_10th<std::ratio<N, D / 10>>::value> {};

        template<class Rep = int, class Period = std::ratio<1>>
        void format_tm(const std::tm& value, format_context& ctx, const std::chrono::duration<Rep, Period> d = {}) const
        {
            std::stringbuf sbuf; // For the love of God, why? :(

            std::ios ios{nullptr};
            const auto loc {get_locale(ctx)};
            ios.imbue(loc);
            const auto& time_facet {std::use_facet<std::time_put<char>>(loc)};

            bool in_format {false};
            char mod {};

            const auto f {fmt()};
            const auto end {f.end()};
            auto begin {f.begin()};
            
            while (begin != end)
            {
                const auto iter {std::find(begin, end, '%')};
                if (begin != iter)
                {
                    ctx.out.write(std::string_view{begin, iter});
                }

                if (iter == end)
                {
                    break;
                }

                begin = iter;

                ++begin;
                const auto spec {*begin};
                ++begin;

                if (begin != end)
                {
                    if (spec == 'O' || spec == 'E')
                    {
                        time_facet.put({&sbuf}, ios, fill, &value, *begin, spec);
                        continue;
                    }
                }

                switch (spec)
                {
                case '%':
                    ctx.out.put('%');
                    break;
                case 'n':
                    ctx.out.put('\n');
                    break;
                case 't':
                    ctx.out.put('\t');
                    break;
                case 'Q':
                    formatter<Rep>::default_format(d.count(), ctx);
                    break;
                case 'q':
                    formatter<chrono_suffix<Period>>::format({}, ctx);
                    break;
                case 'd':
                    lib2::format_to<"{:02}">(ctx.out, value.tm_mday);
                    break;
                case 'e':
                    lib2::format_to<"{: >2}">(ctx.out, value.tm_mday);
                    break;
                case 'm':
                    lib2::format_to<"{:02}">(ctx.out, value.tm_mon + 1);
                    break;
                case 'C':
                    lib2::format_to<"{:02}">(ctx.out, value.tm_year + 1900);
                    break;
                case 'Y':
                    lib2::format_to<"{:04}">(ctx.out, value.tm_year + 1900);
                    break;
                case 'y':
                    lib2::format_to<"{:02}">(ctx.out, (value.tm_year + 1900) % 100);
                    break;
                case 'u':
                    formatter<int>::default_format(value.tm_wday + 1, ctx);
                    break;
                case 'w':
                    formatter<int>::default_format(value.tm_wday, ctx);
                    break;
                case 'H':
                    lib2::format_to<"{:02}">(ctx.out, value.tm_hour);
                    break;
                case 'M':
                    lib2::format_to<"{:02}">(ctx.out, value.tm_min);
                    break;
                case 'S':
                    if constexpr (std::chrono::treat_as_floating_point_v<Rep>)
                    {
                        const std::chrono::hh_mm_ss hhmmss {d};
                        lib2::format_to<"{:.{}}">(ctx.out, (d - hhmmss.hours() - hhmmss.minutes()).count(), std::size_t(std::min(std::intmax_t{6}, denom_10th<Period>::value)));
                    }
                    else
                    {
                        lib2::format_to<"{:02}">(ctx.out, value.tm_sec);
                    }
                    break;
                default:
                    time_facet.put({&sbuf}, ios, fill, &value, spec);
                    break;
                }
            }

            ctx.out.write(sbuf.view());
        }
    private:
        std::string_view fmt_;

        constexpr void validate(this const auto& self, const std::string_view fmt)
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
                            throw format_error{format<"Invalid chrono spec: %{}{}">(mod, ch)};
                        }
                        mod = ch;
                        continue;
                    case '%':
                    case 'n':
                    case 't':
                        if (mod)
                        {
                            throw format_error{format<"Invalid chrono spec: %{}{}">(mod, ch)};
                        }
                        break;
                    default:
                        if (!self.validate_spec(ch, mod))
                        {
                            if (mod)
                            {
                                throw format_error{format<"Invalid chrono spec: %{}{}">(mod, ch)};
                            }
                            throw format_error{format<"Invalid chrono spec: %{}">(ch)};
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
                throw format_error{"Invalid chrono spec: %"};
            }
        }
    };

    template<>
    struct formatter<std::chrono::day> : public chrono_formatter
    {
        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            switch (spec)
            {
            case 'd':
            case 'e':
                return mod == 0 || mod == 'O';
            default:
                return chrono_formatter::validate_spec(spec, mod);
            }
        }

        void format(const std::chrono::day d, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(d, temp_context);
            }
            else
            {
                const std::tm time {.tm_mday=int(unsigned{d})};
                this->format_tm(time, temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::day d, format_context& ctx)
        {
            lib2::format_to<"{:02}">(ctx.out, unsigned{d});
            if (!d.ok())
            {
                ctx.out.write(" is not a valid day");
            }
        }
    };

    template<>
    struct formatter<std::chrono::month> : public chrono_formatter
    {
        void format(const std::chrono::month m, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(m, temp_context);
            }
            else
            {
                const std::tm time {.tm_mon=int(unsigned{m}-1)};
                this->format_tm(time, temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::month m, format_context& ctx)
        {
            if (m.ok())
            {
                lib2::format_to<"{:L%b}">(ctx.locale(), ctx.out, m);
            }
            else
            {
                lib2::format_to<"{} is not a valid month">(ctx.out, unsigned{m});
            }
        }
        
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
                return chrono_formatter::validate_spec(spec, mod);
            }
        }
    };

    template<>
    struct formatter<std::chrono::year> : public chrono_formatter
    {
        void format(const std::chrono::year y, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(y, temp_context);
            }
            else
            {
                const std::tm time {.tm_year=int{y} - 1900};
                this->format_tm(time, temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::year y, format_context& ctx)
        {
            lib2::format_to<"{:04}">(ctx.out, int{y});
            if (!y.ok())
            {
                ctx.out.write(" is not a valid year");
            }
        }

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
                return chrono_formatter::validate_spec(spec, mod);
            }
        }
    };

    template<>
    struct formatter<std::chrono::weekday> : public chrono_formatter
    {
        void format(const std::chrono::weekday wd, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(wd, temp_context);
            }
            else
            {
                const std::tm time {.tm_wday=int(wd.c_encoding())};
                this->format_tm(time, temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::weekday wd, format_context& ctx)
        {
            if (wd.ok())
            {
                lib2::format_to<"{:L%a}">(ctx.locale(), ctx.out, wd);
            }
            else
            {
                lib2::format_to<"{} is not a valid weekday">(ctx.out, wd.c_encoding());
            }
        }

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
                return chrono_formatter::validate_spec(spec, mod);
            }
        }
    };

    template<>
    struct formatter<std::chrono::weekday_indexed> : public formatter<std::chrono::weekday>
    {
        void format(const std::chrono::weekday_indexed wdi, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(wdi, temp_context);
            }
            else
            {
                formatter<std::chrono::weekday>::format(wdi.weekday(), temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::weekday_indexed wdi, format_context& ctx)
        {
            if (wdi.index() < 1 || wdi.index() > 5)
            {
                lib2::format_to<"{:L}[{} is not a valid index]">(ctx.locale(), ctx.out, wdi.weekday(), wdi.index());
            }
            else
            {
                lib2::format_to<"{:L}[{}]">(ctx.locale(), ctx.out, wdi.weekday(), wdi.index());
            }
        }
    };

    template<>
    struct formatter<std::chrono::weekday_last> : public formatter<std::chrono::weekday>
    {
        void format(const std::chrono::weekday_last wdl, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(wdl, temp_context);
            }
            else
            {
                formatter<std::chrono::weekday>::format(wdl.weekday(), temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::weekday_last wdl, format_context& ctx)
        {
            lib2::format_to<"{:L}[last]">(ctx.locale(), ctx.out, wdl.weekday());
        }
    };

    template<>
    struct formatter<std::chrono::month_day> : public chrono_formatter
    {
        void format(const std::chrono::month_day md, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(md, temp_context);
            }
            else
            {
                const std::tm time {.tm_mday=int(unsigned{md.day()}), .tm_mon=int(unsigned{md.month()}-1)};
                this->format_tm(time, temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::month_day md, format_context& ctx)
        {
            lib2::format_to<"{}/{}">(ctx.locale(), ctx.out, md.month(), md.day());
        }

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
                return formatter<std::chrono::day>::validate_spec(spec, mod);
            }
        }
    };

    template<>
    struct formatter<std::chrono::month_day_last> : public formatter<std::chrono::month>
    {
        void format(const std::chrono::month_day_last mdl, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(mdl, temp_context);
            }
            else
            {
                formatter<std::chrono::month>::format(mdl.month(), temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::month_day_last mdl, format_context& ctx)
        {
            lib2::format_to<"{}/last">(ctx.locale(), ctx.out, mdl.month());
        }
    };

    template<>
    struct formatter<std::chrono::month_weekday> : public chrono_formatter
    {
        void format(const std::chrono::month_weekday mwd, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(mwd, temp_context);
            }
            else
            {
                const std::tm time {.tm_mon=int(unsigned{mwd.month()}-1), .tm_wday=int(mwd.weekday_indexed().weekday().c_encoding())};
                this->format_tm(time, temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::month_weekday mwd, format_context& ctx)
        {
            lib2::format_to<"{:L}{:L}">(ctx.locale(), ctx.out, mwd.month(), mwd.weekday_indexed());
        }

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            return formatter<std::chrono::month>::validate_spec(spec, mod) ||
                   formatter<std::chrono::weekday>::validate_spec(spec, mod);
        }
    };

    template<>
    struct formatter<std::chrono::month_weekday_last> : public formatter<std::chrono::month_weekday>
    {
        void format(const std::chrono::month_weekday_last mwdl, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(mwdl, temp_context);
            }
            else
            {
                const std::tm time {.tm_mon=int(unsigned{mwdl.month()}-1), .tm_wday=int(mwdl.weekday_last().weekday().c_encoding())};
                this->format_tm(time, temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::month_weekday_last mwdl, format_context& ctx)
        {
            lib2::format_to<"{:L}{:L}">(ctx.locale(), ctx.out, mwdl.month(), mwdl.weekday_last());
        }
    };

    template<>
    struct formatter<std::chrono::year_month> : public chrono_formatter
    {
        void format(const std::chrono::year_month ym, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(ym, temp_context);
            }
            else
            {
                const std::tm time {.tm_mon=int(unsigned{ym.month()}-1), .tm_year=int{ym.year()}-1900};
                this->format_tm(time, temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::year_month ym, format_context& ctx)
        {
            lib2::format_to<"{}/{:L}">(ctx.locale(), ctx.out, ym.year(), ym.month());
        }

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            return formatter<std::chrono::month>::validate_spec(spec, mod) ||
                   formatter<std::chrono::year>::validate_spec(spec, mod);
        }
    };

    template<>
    struct formatter<std::chrono::year_month_day> : public chrono_formatter
    {
        void format(const std::chrono::year_month_day ymd, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(ymd, temp_context);
            }
            else
            {
                const std::tm time {.tm_mday=int(unsigned{ymd.day()}), .tm_mon=int(unsigned{ymd.month()}-1), .tm_year=int{ymd.year()}-1900};
                this->format_tm(time, temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::year_month_day ymd, format_context& ctx)
        {
            lib2::format_to<"{:%F}">(ctx.locale(), ctx.out, ymd);
            if (!ymd.ok())
            {
                ctx.out.write(" is not a valid date");
            }
        }

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            switch (spec)
            {
            case 'D':
            case 'F':
                return mod == 0;
            case 'x':
                return mod == 0 || mod == 'E';
            }

            return formatter<std::chrono::month>::validate_spec(spec, mod) ||
                   formatter<std::chrono::year>::validate_spec(spec, mod) ||
                   formatter<std::chrono::day>::validate_spec(spec, mod);
        }
    };

    template<>
    struct formatter<std::chrono::year_month_day_last> : public formatter<std::chrono::year_month_day>
    {
        void format(const std::chrono::year_month_day_last ymdl, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(ymdl, temp_context);
            }
            else
            {
                formatter<std::chrono::year_month_day>::format(static_cast<std::chrono::sys_days>(ymdl), temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::year_month_day_last ymdl, format_context& ctx)
        {
            lib2::format_to<"{}/{:L}">(ctx.locale(), ctx.out, ymdl.year(), ymdl.month_day_last());
        }
    };

    template<>
    struct formatter<std::chrono::year_month_weekday> : public chrono_formatter
    {
        void format(const std::chrono::year_month_weekday ymwd, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(ymwd, temp_context);
            }
            else
            {
                const std::tm time {.tm_mon=int(unsigned{ymwd.month()}-1), .tm_year=int{ymwd.year()}-1900, .tm_wday=int(ymwd.weekday().c_encoding())};
                this->format_tm(time, temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::year_month_weekday ymwd, format_context& ctx)
        {
            lib2::format_to<"{}/{:L}/{:L}">(ctx.locale(), ctx.out, ymwd.year(), ymwd.month(), ymwd.weekday_indexed());
        }

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            return formatter<std::chrono::month>::validate_spec(spec, mod) ||
                   formatter<std::chrono::year>::validate_spec(spec, mod) ||
                   formatter<std::chrono::weekday>::validate_spec(spec, mod);
        }
    };

    template<>
    struct formatter<std::chrono::year_month_weekday_last> : public formatter<std::chrono::year_month_weekday>
    {
        void format(const std::chrono::year_month_weekday_last ymwdl, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(ymwdl, temp_context);
            }
            else
            {
                formatter<std::chrono::year_month_weekday>::format(static_cast<std::chrono::sys_days>(ymwdl), temp_context);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::year_month_weekday_last ymwdl, format_context& ctx)
        {
            lib2::format_to<"{}/{:L}/{:L}">(ctx.locale(), ctx.out, ymwdl.year(), ymwdl.month(), ymwdl.weekday_last());
        }
    };

    template<class Rep, class Period>
    struct formatter<std::chrono::duration<Rep, Period>> : public chrono_formatter, public precision_parser
    {
        using chrono_formatter::parse;

        void format(const std::chrono::duration<Rep, Period> d, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            io::string_sink ss;
            format_context temp_context {this->get_locale(ctx), width ? ss : ctx.out};

            if (this->fmt().empty())
            {
                default_format(d, temp_context);
            }
            else
            {
                const std::chrono::hh_mm_ss<std::chrono::duration<Rep, Period>> hhmmss{d};
                const std::tm time {
                    .tm_sec  = int(hhmmss.seconds().count())
                ,   .tm_min  = int(hhmmss.minutes().count())
                ,   .tm_hour = int(hhmmss.hours().count())
                };
                this->format_tm(time, temp_context, d);
            }

            if (width)
            {
                formatter<std::string_view> str_fmt;
                str_fmt.fill  = fill;
                str_fmt.align = align;
                str_fmt.set_width(width);
                str_fmt.format(ss.str, ctx);
            }
        }

        static void default_format(const std::chrono::duration<Rep, Period> d, format_context& ctx)
        {
            lib2::format_to<"{}{}">(ctx.locale(), ctx.out, d.count(), chrono_suffix<Period>{});
        }

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
                return chrono_formatter::validate_spec(spec, mod);
            }
        }
    };
}
