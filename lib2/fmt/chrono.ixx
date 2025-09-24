module;

#include <time.h>
#include "lib2/strings/widen.h"

export module lib2.fmt:chrono;

import std;

import :format;

namespace lib2
{
    template<class R>
    struct chrono_suffix {};

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::atto>)
    {
        return os << STATICALLY_WIDEN(CharT, "as");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::femto>)
    {
        return os << STATICALLY_WIDEN(CharT, "fs");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::pico>)
    {
        return os << STATICALLY_WIDEN(CharT, "ps");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::nano>)
    {
        return os << STATICALLY_WIDEN(CharT, "ns");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::micro>)
    {
        return os << STATICALLY_WIDEN(CharT, "us");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::milli>)
    {
        return os << STATICALLY_WIDEN(CharT, "ms");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::centi>)
    {
        return os << STATICALLY_WIDEN(CharT, "cs");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::deci>)
    {
        return os << STATICALLY_WIDEN(CharT, "ds");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::ratio<1>>)
    {
        os.put('s');
        return os;
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::deca>)
    {
        return os << STATICALLY_WIDEN(CharT, "das");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::hecto>)
    {
        return os << STATICALLY_WIDEN(CharT, "hs");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::kilo>)
    {
        return os << STATICALLY_WIDEN(CharT, "ks");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::mega>)
    {
        return os << STATICALLY_WIDEN(CharT, "Ms");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::giga>)
    {
        return os << STATICALLY_WIDEN(CharT, "Gs");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::tera>)
    {
        return os << STATICALLY_WIDEN(CharT, "Ts");
    }
    
    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::peta>)
    {
        return os << STATICALLY_WIDEN(CharT, "Ps");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::exa>)
    {
        return os << STATICALLY_WIDEN(CharT, "Es");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::ratio<60>>)
    {
        return os << STATICALLY_WIDEN(CharT, "min");
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::ratio<3600>>)
    {
        os.put('h');
        return os;
    }

    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::ratio<86400>>)
    {
        os.put('d');
        return os;
    }

    template<class CharT, std::intmax_t Num>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::ratio<Num, 1>>)
    {
        os.put('[');
        return os << Num << STATICALLY_WIDEN(CharT, "]s");
    }

    template<class CharT, std::intmax_t Num, std::intmax_t Den>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const chrono_suffix<std::ratio<Num, Den>>)
    {
        os.put('[');
        os << Num;
        os.put('/');
        return os << Den << STATICALLY_WIDEN(CharT, "]s");
    }

    template<class CRTP, class CharT>
    class chrono_formatter : public fill_align_parser<CharT>, public width_parser<CharT>, public precision_parser<CharT>, public locale_parser<CharT>
    {
    public:
        [[nodiscard]] constexpr std::basic_string_view<CharT> get_spec() const noexcept
        {
            return spec;
        }

        constexpr void set_spec(std::basic_string_view<CharT> spec)
        {
            check_spec(spec);
            this->spec = spec;
        }

        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            auto iter {fill_align_parser<CharT>::parse(ctx)};
            ctx.advance_to(iter);
            iter = width_parser<CharT>::parse(ctx);
            ctx.advance_to(iter);
            iter = precision_parser<CharT>::parse(ctx);
            ctx.advance_to(iter);
            iter = locale_parser<CharT>::parse(ctx);
            ctx.advance_to(iter);

            const auto spec_end {std::find(iter, ctx.end(), '}')};
            set_spec({iter, spec_end});
            
            return spec_end;
        }

        static constexpr bool check_spec(CharT spec, CharT mod) noexcept
        {
            if (mod)
            {
                return false;
            }

            switch (spec)
            {
            case '%':
            case 'n':
            case 't':
                return true;
            }

            return false;
        }
    protected:
        using Out = lib2::ostream_iterator<CharT>;

        template<class Rep, class Period>
        void format(const std::chrono::duration<Rep, Period> dur, basic_format_context<CharT>& ctx, const std::basic_string_view<CharT> abbrev = {}, const std::chrono::seconds* offset = nullptr)
        {
            const auto precision {this->get_precision(ctx)};
            const auto width {this->get_width(ctx)};

            
            const auto loc {this->is_locale_formatting() ? ctx.locale() : std::locale::classic()};
            std::basic_ios<CharT> str {nullptr};
            str.imbue(loc);

            if (precision)
            {
                str.precision(*precision);
            }

            const auto& time_base {std::use_facet<std::time_put<CharT, Out>>(loc)};
            const auto& char_base {std::use_facet<std::ctype<CharT>>(loc)};
            const auto& num_base {std::use_facet<std::num_put<CharT, Out>>(loc)};

            std::tm main_tm;
            std::tm offset_tm;
            {
                const std::time_t t = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
                _gmtime64_s(&main_tm, &t);
            }
            if (offset)
            {
                std::time_t t;
                if (offset->count() < 0)
                {
                    t = -offset->count();
                }
                else
                {
                    t = offset->count();
                }
                _gmtime64_s(&offset_tm, &t);
            }

            basic_text_ostream<CharT>* stream;
            lib2::basic_ostringstream<CharT> ss;
            if (width == 0)
            {
                stream = std::addressof(ctx.stream());
            }
            else
            {
                stream = std::addressof(ss);
            }
            Out out {*stream};

            auto start {spec.begin()};
            const auto end {spec.end()};

            do
            {
                auto it {std::find(start, end, char_base.widen('%'))};
                *stream << std::basic_string_view<CharT>{start, it};
                if (it != end)
                {
                    if (++it != end)
                    {
                        char specifier {char_base.narrow(*it)};
                        char modifier {0};

                        if (++it != end)
                        {
                            switch (*it)
                            {
                            case 'O':
                            case 'E':
                                modifier  = specifier;
                                specifier = char_base.narrow(*it);
                            }
                        }

                        switch (specifier)
                        {
                        case 'z':
                        {
                            if (!offset)
                            {
                                throw std::format_error{"No time zone offset"};
                            }
                            
                            if (offset->count() < 0)
                            {
                                stream->put(char_base.widen('-'));
                            }

                            time_base.put(out, str, this->get_fill(), &offset_tm, 'H');
                            if (modifier)
                            {
                                stream->put(char_base.widen(':'));
                            }
                            time_base.put(out, str, this->get_fill(), &offset_tm, 'M');
                            break;
                        }
                        case 'Z':
                            if (!abbrev.data())
                            {
                                throw std::format_error{"No time zone abbreviation"};
                            }
                            *stream << abbrev;
                            break;
                        case 'q':
                            *stream << chrono_suffix<Period>{};
                            break;
                        case 'Q':
                        {
                            num_base.put(out, str, this->get_fill(), dur.count());
                            break;
                        }
                        default:
                            time_base.put(out, str, this->get_fill(), &main_tm, specifier, modifier);
                        }
                    }
                }

                start = it;
            } while (start != end);

            if (stream == std::addressof(ss))
            {
                lib2::formatter<std::basic_string_view<CharT>, CharT> str_fmt;
                str_fmt.set_fill(this->get_fill());
                str_fmt.set_align(this->get_align());
                str_fmt.set_width(width);

                str_fmt.format(ss.view(), ctx);
            }
        }
    private:
        std::basic_string_view<CharT> spec;

        static constexpr void check_spec(std::basic_string_view<CharT> spec)
        {
            for (auto it {spec.begin()}; it != spec.end(); ++it)
            {
                if (*it == '%')
                {
                    if (++it == spec.end())
                    {
                        throw std::format_error{"Unexpected end to chrono spec"};
                    }
                    CharT specifier {*it};
                    CharT modifier  {0};

                    if (specifier == 'E' || specifier == 'O')
                    {
                        if (++it == spec.end())
                        {
                            throw std::format_error{"Unexpected end to chrono spec"};
                        }

                        modifier  = specifier;
                        specifier = *it;
                    }

                    if (!CRTP::check_spec(specifier, modifier))
                    {
                        if (modifier)
                        {
                            throw std::format_error{lib2::format("Invalid chrono spec: '%{}{}'", static_cast<char>(modifier), static_cast<char>(specifier))};
                        }
                        throw std::format_error{lib2::format("Invalid chrono spec: '%{}'", static_cast<char>(specifier))};
                    }
                }
            }
        }
    };

    export
    template<class Rep, class Period, class CharT>
    struct formatter<std::chrono::duration<Rep, Period>, CharT> : public chrono_formatter<formatter<std::chrono::duration<Rep, Period>, CharT>, CharT>
    {
        using base = chrono_formatter<formatter<std::chrono::duration<Rep, Period>, CharT>, CharT>;

        constexpr formatter() noexcept
        {
            this->set_spec("%Q%q");
        }

        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            const auto iter {base::parse(ctx)};

            if (this->get_spec().empty())
            {
                this->set_spec("%Q%q");
            }

            return iter;
        }

        void format(const std::chrono::duration<Rep, Period> dur, basic_format_context<CharT>& ctx)
        {
            base::format(dur, ctx);
        }

        static constexpr bool check_spec(CharT spec, CharT mod) noexcept
        {
            switch (spec)
            {
            case 'H':
            case 'I':
            case 'M':
            case 'S':
                return mod == 0 || mod ==  'O';
            case 'p':
            case 'R':
            case 'T':
            case 'r':
            case 'Q':
            case 'q':
                return mod == 0;
            case 'X':
                return mod == 0 || mod == 'E';
            }

            return base::check_spec(spec, mod);
        }
    };
}