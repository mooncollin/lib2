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
            lib2::format_to<"[{}/{}]s">(ctx.stream, Num, Den);
        }
    };

    template<std::intmax_t Num>
    struct formatter<chrono_suffix<std::ratio<Num, 1>>> : public format_parser
    {
        static void format(const chrono_suffix<std::ratio<Num, 1>>, format_context& ctx)
        {
            lib2::format_to<"[{}]s">(ctx.stream, Num);
        }
    };

    template<>
    struct formatter<chrono_suffix<std::atto>> : public format_parser
    {
        static void format(const chrono_suffix<std::atto>, format_context& ctx)
        {
            ctx.stream.write("as");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::femto>> : public format_parser
    {
        static void format(const chrono_suffix<std::femto>, format_context& ctx)
        {
            ctx.stream.write("fs");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::pico>> : public format_parser
    {
        static void format(const chrono_suffix<std::pico>, format_context& ctx)
        {
            ctx.stream.write("ps");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::nano>> : public format_parser
    {
        static void format(const chrono_suffix<std::nano>, format_context& ctx)
        {
            ctx.stream.write("ns");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::micro>> : public format_parser
    {
        static void format(const chrono_suffix<std::micro>, format_context& ctx)
        {
            ctx.stream.write("us");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::milli>> : public format_parser
    {
        static void format(const chrono_suffix<std::milli>, format_context& ctx)
        {
            ctx.stream.write("ms");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::centi>> : public format_parser
    {
        static void format(const chrono_suffix<std::centi>, format_context& ctx)
        {
            ctx.stream.write("cs");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::deci>> : public format_parser
    {
        static void format(const chrono_suffix<std::deci>, format_context& ctx)
        {
            ctx.stream.write("ds");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::ratio<1>>> : public format_parser
    {
        static void format(const chrono_suffix<std::ratio<1>>, format_context& ctx)
        {
            ctx.stream.put('s');
        }
    };

    template<>
    struct formatter<chrono_suffix<std::deca>> : public format_parser
    {
        static void format(const chrono_suffix<std::deca>, format_context& ctx)
        {
            ctx.stream.write("das");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::hecto>> : public format_parser
    {
        static void format(const chrono_suffix<std::hecto>, format_context& ctx)
        {
            ctx.stream.write("hs");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::kilo>> : public format_parser
    {
        static void format(const chrono_suffix<std::kilo>, format_context& ctx)
        {
            ctx.stream.write("ks");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::mega>> : public format_parser
    {
        static void format(const chrono_suffix<std::mega>, format_context& ctx)
        {
            ctx.stream.write("Ms");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::giga>> : public format_parser
    {
        static void format(const chrono_suffix<std::giga>, format_context& ctx)
        {
            ctx.stream.write("Gs");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::tera>> : public format_parser
    {
        static void format(const chrono_suffix<std::tera>, format_context& ctx)
        {
            ctx.stream.write("Ts");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::peta>> : public format_parser
    {
        static void format(const chrono_suffix<std::peta>, format_context& ctx)
        {
            ctx.stream.write("Ps");
        }
    };
    
    template<>
    struct formatter<chrono_suffix<std::exa>> : public format_parser
    {
        static void format(const chrono_suffix<std::exa>, format_context& ctx)
        {
            ctx.stream.write("Es");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::ratio<60>>> : public format_parser
    {
        static void format(const chrono_suffix<std::ratio<60>>, format_context& ctx)
        {
            ctx.stream.write("min");
        }
    };

    template<>
    struct formatter<chrono_suffix<std::ratio<3600>>> : public format_parser
    {
        static void format(const chrono_suffix<std::ratio<3600>>, format_context& ctx)
        {
            ctx.stream.put('h');
        }
    };

    template<>
    struct formatter<chrono_suffix<std::ratio<86400>>> : public format_parser
    {
        static void format(const chrono_suffix<std::ratio<86400>>, format_context& ctx)
        {
            ctx.stream.put('d');
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
            std::ios ios{nullptr};
            ios.imbue(get_locale(ctx));
            const auto& time_facet {std::use_facet<std::time_put<char, ostream_iterator<char>>>(ios.getloc())};

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
                    ctx.stream.write(std::string_view{begin, iter});
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
                        time_facet.put(ostream_iterator{ctx.stream}, ios, fill, &value, *begin, spec);
                        continue;
                    }
                }

                switch (spec)
                {
                case '%':
                    ctx.stream.put('%');
                    break;
                case 'n':
                    ctx.stream.put('\n');
                    break;
                case 't':
                    ctx.stream.put('\t');
                    break;
                case 'Q':
                    formatter<Rep>::default_format(d.count(), ctx);
                    break;
                case 'q':
                    formatter<chrono_suffix<Period>>::format({}, ctx);
                    break;
                case 'd':
                    lib2::format_to<"{:02}">(ctx.stream, value.tm_mday);
                    break;
                case 'e':
                    lib2::format_to<"{: >2}">(ctx.stream, value.tm_mday);
                    break;
                case 'm':
                    lib2::format_to<"{:02}">(ctx.stream, value.tm_mon + 1);
                    break;
                case 'C':
                    lib2::format_to<"{:02}">(ctx.stream, value.tm_year + 1900);
                    break;
                case 'Y':
                    lib2::format_to<"{:04}">(ctx.stream, value.tm_year + 1900);
                    break;
                case 'y':
                    lib2::format_to<"{:02}">(ctx.stream, (value.tm_year + 1900) % 100);
                    break;
                case 'u':
                    formatter<int>::default_format(value.tm_wday + 1, ctx);
                    break;
                case 'w':
                    formatter<int>::default_format(value.tm_wday, ctx);
                    break;
                case 'H':
                    lib2::format_to<"{:02}">(ctx.stream, value.tm_hour);
                    break;
                case 'M':
                    lib2::format_to<"{:02}">(ctx.stream, value.tm_min);
                    break;
                case 'S':
                    if constexpr (std::chrono::treat_as_floating_point_v<Rep>)
                    {
                        const std::chrono::hh_mm_ss hhmmss {d};
                        lib2::format_to<"{:.{}}">(ctx.stream, (d - hhmmss.hours() - hhmmss.minutes()).count(), std::size_t(std::min(std::intmax_t{6}, denom_10th<Period>::value)));
                    }
                    else
                    {
                        lib2::format_to<"{:02}">(ctx.stream, value.tm_sec);
                    }
                    break;
                default:
                    time_facet.put(ostream_iterator{ctx.stream}, ios, fill, &value, spec);
                    break;
                }
            }
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

    export
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

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::day d, format_context& ctx)
        {
            lib2::format_to<"{:02}">(ctx.stream, unsigned{d});
            if (!d.ok())
            {
                ctx.stream.write(" is not a valid day");
            }
        }
    };

    export
    template<>
    struct formatter<std::chrono::month> : public chrono_formatter
    {
        void format(const std::chrono::month m, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::month m, format_context& ctx)
        {
            if (m.ok())
            {
                lib2::format_to<"{:L%b}">(ctx.locale(), ctx.stream, m);
            }
            else
            {
                lib2::format_to<"{} is not a valid month">(ctx.stream, unsigned{m});
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

    export
    template<>
    struct formatter<std::chrono::year> : public chrono_formatter
    {
        void format(const std::chrono::year y, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::year y, format_context& ctx)
        {
            lib2::format_to<"{:04}">(ctx.stream, int{y});
            if (!y.ok())
            {
                ctx.stream.write(" is not a valid year");
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

    export
    template<>
    struct formatter<std::chrono::weekday> : public chrono_formatter
    {
        void format(const std::chrono::weekday wd, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::weekday wd, format_context& ctx)
        {
            if (wd.ok())
            {
                lib2::format_to<"{:L%a}">(ctx.locale(), ctx.stream, wd);
            }
            else
            {
                lib2::format_to<"{} is not a valid weekday">(ctx.stream, wd.c_encoding());
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

    export
    template<>
    struct formatter<std::chrono::weekday_indexed> : public formatter<std::chrono::weekday>
    {
        void format(const std::chrono::weekday_indexed wdi, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::weekday_indexed wdi, format_context& ctx)
        {
            if (wdi.index() < 1 || wdi.index() > 5)
            {
                lib2::format_to<"{:L}[{} is not a valid index]">(ctx.locale(), ctx.stream, wdi.weekday(), wdi.index());
            }
            else
            {
                lib2::format_to<"{:L}[{}]">(ctx.locale(), ctx.stream, wdi.weekday(), wdi.index());
            }
        }
    };

    export
    template<>
    struct formatter<std::chrono::weekday_last> : public formatter<std::chrono::weekday>
    {
        void format(const std::chrono::weekday_last wdl, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::weekday_last wdl, format_context& ctx)
        {
            lib2::format_to<"{:L}[last]">(ctx.locale(), ctx.stream, wdl.weekday());
        }
    };

    export
    template<>
    struct formatter<std::chrono::month_day> : public chrono_formatter
    {
        void format(const std::chrono::month_day md, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::month_day md, format_context& ctx)
        {
            lib2::format_to<"{}/{}">(ctx.locale(), ctx.stream, md.month(), md.day());
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

    export
    template<>
    struct formatter<std::chrono::month_day_last> : public formatter<std::chrono::month>
    {
        void format(const std::chrono::month_day_last mdl, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::month_day_last mdl, format_context& ctx)
        {
            lib2::format_to<"{}/last">(ctx.locale(), ctx.stream, mdl.month());
        }
    };

    export
    template<>
    struct formatter<std::chrono::month_weekday> : public chrono_formatter
    {
        void format(const std::chrono::month_weekday mwd, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::month_weekday mwd, format_context& ctx)
        {
            lib2::format_to<"{:L}{:L}">(ctx.locale(), ctx.stream, mwd.month(), mwd.weekday_indexed());
        }

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            return formatter<std::chrono::month>::validate_spec(spec, mod) ||
                   formatter<std::chrono::weekday>::validate_spec(spec, mod);
        }
    };

    export
    template<>
    struct formatter<std::chrono::month_weekday_last> : public formatter<std::chrono::month_weekday>
    {
        void format(const std::chrono::month_weekday_last mwdl, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::month_weekday_last mwdl, format_context& ctx)
        {
            lib2::format_to<"{:L}{:L}">(ctx.locale(), ctx.stream, mwdl.month(), mwdl.weekday_last());
        }
    };

    export
    template<>
    struct formatter<std::chrono::year_month> : public chrono_formatter
    {
        void format(const std::chrono::year_month ym, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::year_month ym, format_context& ctx)
        {
            lib2::format_to<"{}/{:L}">(ctx.locale(), ctx.stream, ym.year(), ym.month());
        }

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            return formatter<std::chrono::month>::validate_spec(spec, mod) ||
                   formatter<std::chrono::year>::validate_spec(spec, mod);
        }
    };

    export
    template<>
    struct formatter<std::chrono::year_month_day> : public chrono_formatter
    {
        void format(const std::chrono::year_month_day ymd, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::year_month_day ymd, format_context& ctx)
        {
            lib2::format_to<"{:%F}">(ctx.locale(), ctx.stream, ymd);
            if (!ymd.ok())
            {
                ctx.stream.write(" is not a valid date");
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

    export
    template<>
    struct formatter<std::chrono::year_month_day_last> : public formatter<std::chrono::year_month_day>
    {
        void format(const std::chrono::year_month_day_last ymdl, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::year_month_day_last ymdl, format_context& ctx)
        {
            lib2::format_to<"{}/{:L}">(ctx.locale(), ctx.stream, ymdl.year(), ymdl.month_day_last());
        }
    };

    export
    template<>
    struct formatter<std::chrono::year_month_weekday> : public chrono_formatter
    {
        void format(const std::chrono::year_month_weekday ymwd, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::year_month_weekday ymwd, format_context& ctx)
        {
            lib2::format_to<"{}/{:L}/{:L}">(ctx.locale(), ctx.stream, ymwd.year(), ymwd.month(), ymwd.weekday_indexed());
        }

        static constexpr bool validate_spec(const char spec, const char mod) noexcept
        {
            return formatter<std::chrono::month>::validate_spec(spec, mod) ||
                   formatter<std::chrono::year>::validate_spec(spec, mod) ||
                   formatter<std::chrono::weekday>::validate_spec(spec, mod);
        }
    };

    export
    template<>
    struct formatter<std::chrono::year_month_weekday_last> : public formatter<std::chrono::year_month_weekday>
    {
        void format(const std::chrono::year_month_weekday_last ymwdl, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::year_month_weekday_last ymwdl, format_context& ctx)
        {
            lib2::format_to<"{}/{:L}/{:L}">(ctx.locale(), ctx.stream, ymwdl.year(), ymwdl.month(), ymwdl.weekday_last());
        }
    };

    export
    template<class Rep, class Period>
    struct formatter<std::chrono::duration<Rep, Period>> : public chrono_formatter, public precision_parser
    {
        using chrono_formatter::parse;

        void format(const std::chrono::duration<Rep, Period> d, format_context& ctx) const
        {
            const auto width {this->get_width(ctx)};

            ostringstream temp_os;
            text_ostream& ref_os {width ? temp_os : ctx.stream};
            format_context temp_context {this->get_locale(ctx), ref_os};

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
                str_fmt.format(temp_os.view(), ctx);
            }
        }

        static void default_format(const std::chrono::duration<Rep, Period> d, format_context& ctx)
        {
            lib2::format_to<"{}{}">(ctx.locale(), ctx.stream, d.count(), chrono_suffix<Period>{});
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
