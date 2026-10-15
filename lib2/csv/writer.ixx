export module lib2.csv:writer;

import std;

import lib2.io;
import lib2.fmt;
import lib2.strings;

import :dialect;

namespace lib2::csv
{
    export
    class writer
    {
    public:
        writer(lib2::io::dyn_sink output, dialect d = {}) noexcept
            : output_{output}, dialect_{std::move(d)} {}

        const lib2::csv::dialect& dialect() const noexcept
        {
            return dialect_;
        }

        template<class Arg, class... Args>
        inline void write_row(const Arg& arg, const Args&... args)
        {
            write_value(arg);
            if constexpr (sizeof...(args) > 0)
            {
                output_.put(dialect_.delimiter);
                write_row(args...);
            }
            else
            {
                output_.put('\n');
            }
        }
    private:
        lib2::io::dyn_sink output_;
        lib2::csv::dialect dialect_;

        inline void write_value(const std::string_view str)
        {
            switch (dialect_.quoting)
            {
            case quoting_style::minimal:
                {
                    const char search[] {dialect_.quotechar, dialect_.escapechar};
                    const std::string_view search_view {search, sizeof(search)};
                    auto idx {str.find_first_of(search_view)};
                    if (idx == std::string_view::npos)
                    {
                        output_.write(str);
                    }
                    else
                    {
                        output_.put(dialect_.quotechar);
                        output_.write(str.substr(0, idx));
                        for (const char c : str.substr(idx))
                        {
                            if (c == dialect_.quotechar || c == dialect_.escapechar)
                            {
                                output_.put(dialect_.escapechar);
                            }
                            output_.put(c);
                        }
                        output_.put(dialect_.quotechar);
                    }
                }
                break;
            case quoting_style::nonnumeric:
                {
                    const auto number {std::ranges::find_if(str, lib2::isdigit_ascii)};
                    if (number == str.end())
                    {
                        output_.write(str);
                        break;
                    }
                }
            case quoting_style::all:
                output_.put(dialect_.quotechar);
                for (const char c : str)
                {
                    if (c == dialect_.quotechar || c == dialect_.escapechar)
                    {
                        output_.put(dialect_.escapechar);
                    }
                    output_.put(c);
                }
                output_.put(dialect_.quotechar);
                break;
            default:
                output_.write(str);
                break;
            }
        }

        template<lib2::formattable Arg>
            requires(!std::convertible_to<Arg, std::string_view>)
        inline void write_value(const Arg& arg)
        {
            if constexpr (std::integral<Arg> || std::floating_point<Arg>)
            {
                switch (dialect_.quoting)
                {
                case quoting_style::all:
                    output_.put(dialect_.quotechar);
                    lib2::format_to<"{}">(output_, arg);
                    output_.put(dialect_.quotechar);
                    break;
                case quoting_style::minimal:
                case quoting_style::nonnumeric:
                default:
                    lib2::format_to<"{}">(output_, arg);
                    break;
                }
            }
            else
            {
                switch (dialect_.quoting)
                {
                case quoting_style::minimal:
                case quoting_style::all:
                case quoting_style::nonnumeric:
                    write_value(lib2::format<"{}">(arg));
                    break;
                default:
                    lib2::format_to<"{}">(output_, arg);
                    break;
                }
            }
        }
    };
}