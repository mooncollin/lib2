export module lib2.fmt:format;

import std;

import lib2.type_traits;
import lib2.concepts;
import lib2.utility;
import lib2.compact_optional;
import lib2.strings;
import lib2.io;

import :formatter;
import :context;
import :parsers;

namespace lib2
{
// ==============================================================================================================================
// Format strings
// ==============================================================================================================================
    export
    template<class Collector>
    constexpr void do_format(format_parse_context& ctx, Collector& collector)
    {
        constexpr std::string_view braces {"{}"};
        
        while (ctx.begin != ctx.end)
        {
            auto iter {std::find_first_of(ctx.begin, ctx.end, braces.begin(), braces.end())};
            
            if (iter == ctx.end)
            {
                if (ctx.begin != ctx.end)
                {
                    collector.string_literal(std::string_view{ctx.begin, ctx.end});
                }
                break;
            }
            
            if (*iter == '}')
            {
                ++iter;
                if (iter == ctx.end || *iter != '}')
                {
                    throw format_error{"Unmatched '}' in format string."}; 
                }

                collector.string_literal(std::string_view{ctx.begin, iter});
                ctx.begin = iter + 1;
                continue;
            }

            ++iter;
            if (iter == ctx.end)
            {
                throw format_error{"Unmatched '{' in format string."}; 
            }

            if (*iter == '{')
            {
                collector.string_literal(std::string_view{ctx.begin, iter});
                ctx.begin = iter + 1;
                continue;
            }
            
            if (ctx.begin != (iter - 1))
            {
                collector.string_literal(std::string_view{ctx.begin, iter - 1});
            }

            std::size_t arg_id;

            if (isdigit_ascii(*iter))
            {
                arg_id = format_parser::parse_unsigned(iter, ctx.end);
                if (iter == ctx.end)
                {
                    throw format_error{"Unexpected end in format string"};
                }
                ctx.check_arg_id(arg_id);

                if (*iter == ':')
                {
                    ++iter;
                    if (iter == ctx.end)
                    {
                        throw format_error{"Unexpected end in format string"};
                    }
                }
                else if (*iter != '}')
                {
                    throw format_error{"Unexpected character in format string"};
                }
            }
            else if (*iter == ':')
            {
                ++iter;
                if (iter == ctx.end)
                {
                    throw format_error{"Unexpected end in format string"};
                }
                arg_id = ctx.next_arg_id();
            }
            else if (*iter == '}')
            {
                arg_id = ctx.next_arg_id();
            }
            else
            {
                throw format_error{"Unexpected character in format string"};
            }

            ctx.begin = iter;
            collector.arg(arg_id, ctx);

            if (ctx.begin == ctx.end || *ctx.begin != '}')
            {
                throw format_error{"Incorrectly parsed format string. Expecting '}'"};
            }
            ++ctx.begin;
        }
    }

    template<class... Args>
    struct compile_time_checker
    {
        std::tuple<formatter<Args>...> formatters {};
        std::size_t estimated_str_size {0};
        std::size_t leading_str_size {0};
        std::size_t trailing_str_size {0};
        std::size_t num_literals {0};
        std::size_t num_args {0};

        constexpr void string_literal(const std::string_view str) noexcept
        {
            ++num_literals;
            estimated_str_size += str.size();
            const auto pure {str.find_first_of("{}") == std::string_view::npos};
            if (pure)
            {
                if (num_literals == 1 && num_args == 0)
                {
                    leading_str_size = str.size();
                }
                else
                {
                    trailing_str_size = str.size();
                }
            }
        }

        template<std::size_t I>
        constexpr void arg(const std::size_t idx, format_parse_context& ctx)
        {
            if constexpr (I == sizeof...(Args))
            {
                throw format_error{"Argument out of range"};
            }
            else
            {
                if (I == idx)
                {
                    ctx.begin = std::get<I>(formatters).parse(ctx);
                }
                else
                {
                    arg<I+1>(idx, ctx);
                }
            }
        }

        constexpr void arg(const std::size_t idx, format_parse_context& ctx)
        {
            ++num_args;
            trailing_str_size = 0;
            arg<0>(idx, ctx);
        }
    };


    template<class... Args>
    struct instruction_counter : public compile_time_checker<Args...>
    {
        std::size_t num_instructions {0};

        constexpr void string_literal(const std::string_view str) noexcept
        {
            compile_time_checker<Args...>::string_literal(str);
            ++num_instructions;
        }

        constexpr void arg(const std::size_t idx, format_parse_context& ctx)
        {
            compile_time_checker<Args...>::arg(idx, ctx);
            ++num_instructions;
        }
    };

    struct instruction
    {
        enum class t_ : std::uint8_t
        {
            literal,
            arg
        } type;

        std::size_t idx;

        union
        {
            std::size_t size;
            bool default_fmt;
        };
    };

    export
    template<lib2::string_literal Fmt, class... Args>
    struct compile_time_collector : public instruction_counter<Args...>
    {
        static constexpr std::size_t max_instructions {[] {
            instruction_counter<Args...> counter;
            constexpr format_type_store<Args...> store;
            compile_format_parse_context parse_context {Fmt, store};
            do_format(parse_context, counter);
            return counter.num_instructions;
        }()};

        std::array<instruction, max_instructions> instructions {};

        constexpr void string_literal(const std::string_view str) noexcept
        {
            instructions[this->num_instructions] = {.type=instruction::t_::literal, .idx = static_cast<std::size_t>(str.data() - Fmt.data()), .size=str.size()};
            instruction_counter<Args...>::string_literal(str);
        }

        constexpr void arg(const std::size_t idx, format_parse_context& ctx)
        {
            if (idx >= sizeof...(Args))
            {
                throw format_error{"index out of range"};
            }

            instructions[this->num_instructions] = {.type=instruction::t_::arg, .idx=idx, .default_fmt=ctx.begin == ctx.end || *ctx.begin == '}'};
            instruction_counter<Args...>::arg(idx, ctx);
        }

        constexpr bool all_default_fmt() const noexcept
        {
            return std::ranges::all_of(instructions, [](const auto& instr) { return instr.type == instruction::t_::literal || instr.default_fmt; });
        }

        constexpr bool all_literals() const noexcept
        {
            return std::ranges::all_of(instructions, [](const auto& instr) { return instr.type == instruction::t_::literal; });
        }
    };

    export
    text_ostream vformat_to(text_ostream, std::string_view, format_args);

    export
    text_ostream vformat_to(const std::locale&, text_ostream, std::string_view, format_args);

    export
    template<class... Args>
    class format_string
    {
    public:
        consteval format_string(const std::string_view str)
            : str{str}
        {
            compile_time_checker<Args...> checker;
            constexpr format_type_store<Args...> store;
            compile_format_parse_context parse_context {str, store};
            do_format(parse_context, checker);
            estimated_str_size_ = checker.estimated_str_size;
            leading_str_size = checker.leading_str_size;
            trailing_str_size = checker.trailing_str_size;
        }

        template<std::size_t N>
        consteval format_string(const char(&str)[N])
            : format_string{std::string_view{str}} {}
        
        [[nodiscard]] constexpr std::string_view get() const noexcept
        {
            return str;
        }
        
        [[nodiscard]] constexpr std::size_t estimated_str_size() const noexcept
        {
            return estimated_str_size_;
        }

        inline void format(text_ostream os, const Args&... args) const
        {
            auto fmt {str};
            if (leading_str_size)
            {
                if (leading_str_size == 1)
                {
                    os.put(str.front());
                }
                else
                {
                    os.write(str.substr(0, leading_str_size));
                }
                fmt.remove_prefix(leading_str_size);
            }

            if (!fmt.empty())
            {
                if (trailing_str_size)
                {
                    fmt.remove_suffix(trailing_str_size);
                }

                vformat_to(os, fmt, make_format_args(args...));

                if (trailing_str_size)
                {
                    if (trailing_str_size == 1)
                    {
                        os.put(str.back());
                    }
                    else
                    {
                        os.write(str.substr(str.size() - trailing_str_size));
                    }
                }
            }
        }

        inline void format(const std::locale& loc, text_ostream os, const Args&... args) const
        {
            auto fmt {str};
            if (leading_str_size)
            {
                if (leading_str_size == 1)
                {
                    os.put(str.front());
                }
                else
                {
                    os.write(str.substr(0, leading_str_size));
                }
                fmt.remove_prefix(leading_str_size);
            }

            if (!fmt.empty())
            {
                if (trailing_str_size)
                {
                    fmt.remove_suffix(trailing_str_size);
                }

                vformat_to(loc, os, fmt, make_format_args(args...));

                if (trailing_str_size)
                {
                    if (trailing_str_size == 1)
                    {
                        os.put(str.back());
                    }
                    else
                    {
                        os.write(str.substr(str.size() - trailing_str_size));
                    }
                }
            }
        }
    private:
        std::string_view str;
        std::size_t estimated_str_size_;
        std::size_t leading_str_size;
        std::size_t trailing_str_size;
    };

    export
    template<class... Args>
    format_string(std::string_view) -> format_string<Args...>;

    export
    template<std::size_t N, class... Args>
    format_string(const char(&)[N]) -> format_string<Args...>;

    export
    template<string_literal Fmt, class... Args>
    class cp_format_string
    {
        static constexpr auto collector {[] {
            compile_time_collector<Fmt, Args...> collector;
            constexpr format_type_store<Args...> store;
            compile_format_parse_context parse_context {Fmt, store};
            do_format(parse_context, collector);
            return collector;
        }()};
    public:
        consteval cp_format_string() noexcept {}

        static inline void format(text_ostream os, const Args&... args)
        {
            if constexpr (collector.all_literals())
            {
                do_format<0>(os);
            }
            else if constexpr (collector.all_default_fmt())
            {
                format_context ctx {os, {}};
                do_format<0>(ctx, std::forward_as_tuple(args...));
            }
            else
            {
                const auto store {make_format_args(args...)};
                format_context ctx {os, store};
                do_format<0>(ctx, std::forward_as_tuple(args...));
            }
        }

        static inline void format(const std::locale& loc, text_ostream os, const Args&... args)
        {
            if constexpr (collector.all_literals())
            {
                do_format<0>(os);
            }
            else if constexpr (collector.all_default_fmt())
            {
                format_context ctx {loc, os, {}};
                do_format<0>(ctx, std::forward_as_tuple(args...));
            }
            else
            {
                const auto store {make_format_args(args...)};
                format_context ctx {loc, os, store};
                do_format<0>(ctx, std::forward_as_tuple(args...));
            }
        }
    private:
        template<std::size_t I>
        static inline void do_format(text_ostream os)
        {
            if constexpr (I != collector.num_instructions)
            {
                constexpr auto& instr {std::get<I>(collector.instructions)};
                if constexpr (instr.size == 1)
                {
                    os.put(Fmt[instr.idx]);
                }
                else
                {
                    os.write(Fmt.data() + instr.idx, instr.size);
                }
                do_format<I + 1>(os);
            }
        }

        template<std::size_t I, class ArgTuple>
        static inline void do_format(format_context& ctx, const ArgTuple& args)
        {
            if constexpr (I != collector.num_instructions)
            {
                constexpr auto& instr {std::get<I>(collector.instructions)};
                if constexpr (instr.type == instruction::t_::literal)
                {
                    if constexpr (instr.size == 1)
                    {
                        ctx.stream.put(Fmt[instr.idx]);
                    }
                    else
                    {
                        ctx.stream.write(Fmt.data() + instr.idx, instr.size);
                    }
                }
                else
                {
                    using T = std::remove_cvref_t<std::tuple_element_t<instr.idx, ArgTuple>>;
                    if constexpr (default_formattable<T> && instr.default_fmt)
                    {
                        std::get<instr.idx>(collector.formatters).default_format(std::get<instr.idx>(args), ctx);
                    }
                    else
                    {
                        std::get<instr.idx>(collector.formatters).format(std::get<instr.idx>(args), ctx);
                    }
                }
                do_format<I + 1>(ctx, args);
            }
        }
    };

    export
    template<string_literal Fmt, class... Args>
    constexpr text_ostream format_to(text_ostream os, const Args&... args)
    {
        constexpr cp_format_string<Fmt, Args...> fmt_str;
        fmt_str.format(os, args...);
        return os;
    }

    export
    template<string_literal Fmt, class... Args>
    inline text_ostream format_to(const std::locale& loc, text_ostream os, const Args&... args)
    {
        constexpr cp_format_string<Fmt, Args...> fmt_str;
        fmt_str.format(loc, os, args...);
        return os;
    }

    export
    template<string_literal Fmt, class... Args>
    constexpr std::string format(const Args&... args)
    {
        ostringstream ss;
        format_to<Fmt>(ss, args...);
        return std::move(ss).str();
    }

    export
    template<string_literal Fmt, class... Args>
    inline std::string format(const std::locale& loc, const Args&... args)
    {
        ostringstream ss;
        format_to<Fmt>(loc, ss, args...);
        return std::move(ss).str();
    }

    export
    inline std::string vformat(const std::string_view fmt, const format_args args)
    {
        ostringstream ss;
        vformat_to(ss, fmt, args);
        return std::move(ss).str();
    }

    export
    inline std::string vformat(const std::locale& loc, const std::string_view fmt, const format_args args)
    {
        ostringstream ss;
        vformat_to(loc, ss, fmt, args);
        return std::move(ss).str();
    }

    export
    template<class... Args>
    inline text_ostream format_to(text_ostream os, const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        fmt.format(os, args...);
        return os;
    }

    export
    template<class... Args>
    inline text_ostream format_to(const std::locale& loc, text_ostream os, const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        fmt.format(loc, os, args...);
        return os;
    }

    export
    template<class... Args>
    inline std::string format(const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        std::string str;
        str.reserve(fmt.estimated_str_size());
        ostringstream ss {std::move(str)};
        format_to(ss, fmt, args...);
        return std::move(ss).str();
    }

    export
    template<class... Args>
    inline std::string format(const std::locale& loc, const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        std::string str;
        str.reserve(fmt.estimated_str_size());
        ostringstream ss {std::move(str)};
        format_to(loc, ss, fmt, args...);
        return std::move(ss).str();
    }

    export
    template<string_literal Fmt, class... Args>
    constexpr std::size_t formatted_size(const Args&... args)
    {
        lib2::size_ostream sos;
        format_to<Fmt>(sos, args...);
        return sos.size();
    }

    export
    template<string_literal Fmt, class... Args>
    inline std::size_t formatted_size(const std::locale& loc, const Args&... args)
    {
        lib2::size_ostream sos;
        format_to<Fmt>(loc, sos, args...);
        return sos.size();
    }

    export
    template<class... Args>
    inline std::size_t formatted_size(const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        lib2::size_ostream sos;
        format_to(sos, fmt, args...);
        return sos.size();
    }

    export
    template<class... Args>
    inline std::size_t formatted_size(const std::locale& loc, const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        lib2::size_ostream sos;
        format_to(loc, sos, fmt, args...);
        return sos.size();
    }

    export
    template<string_literal Fmt, class... Args>
    inline void print(const Args&... args)
    {
        format_to<Fmt>(lib2::cout, args...);
    }

    export
    template<string_literal Fmt, class... Args>
    inline void print(const std::locale& loc, const Args&... args)
    {
        format_to<Fmt>(loc, lib2::cout, args...);
    }

    export
    template<class... Args>
    inline void print(const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        format_to(lib2::cout, fmt, args...);
    }

    export
    template<class... Args>
    inline void print(const std::locale& loc, const format_string<std::type_identity_t<Args>...> fmt, const Args&... args)
    {
        format_to(loc, lib2::cout, fmt, args...);
    }
}