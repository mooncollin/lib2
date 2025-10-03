module;

#include "lib2/strings/widen.h"

export module lib2.fmt:format;

import std;

import lib2.type_traits;
import lib2.concepts;
import lib2.utility;
import lib2.strings;
import lib2.io;

import :unicode;

namespace lib2
{
    export
    template<class CharT>
    class basic_format_parse_context;

    export
    template<class CharT>
    class basic_format_context;

    export
    template<class T, class CharT = char>
    struct formatter
    {
        template<class ParseCtx>
            requires(stream_writable<T, CharT>)
        constexpr auto parse(ParseCtx& ctx) const noexcept
        {
            return ctx.begin();
        }

        template<class FormatCtx>
            requires(stream_writable<T, CharT>)
        void format(const T& val, FormatCtx& ctx) const
        {
            ctx.stream() << val;
        }
    };

    export
    template<class CharT>
    class basic_format_arg
    {
    public:
        enum class fmt_type
        {
            bool_t,
            char_t,
            int_t,
            uint_t,
            lint_t,
            ulint_t,
            float_t,
            double_t,
            ldouble_t,
            cstring_t,
            string_t,
            ptr_t,
            handle_t
        };
    private:
        template<class T, class CharT2>
        struct transform_arg_t;

        template<class T, class CharT2>
        struct transform_arg_t<const T, CharT2> : public transform_arg_t<T, CharT2> {};

        template<class T, class CharT2>
        struct transform_arg_t<T&, CharT2> : public transform_arg_t<T, CharT2> {};

        template<class T, class CharT2>
        struct transform_arg_t<const T&, CharT2> : public transform_arg_t<T, CharT2> {};

        template<class CharT2>
        struct transform_arg_t<bool, CharT2>
        {
            static constexpr fmt_type value {fmt_type::bool_t};

            using type = bool;

            [[nodiscard]] constexpr type operator()(const bool val) const noexcept
            {
                return val;
            }
        };

        template<class CharT2>
        struct transform_arg_t<CharT2, CharT2>
        {
            static constexpr fmt_type value {fmt_type::char_t};

            using type = CharT2;

            [[nodiscard]] constexpr type operator()(const CharT2 val) const noexcept
            {
                return val;
            }
        };

        template<>
        struct transform_arg_t<char, wchar_t>
        {
            static constexpr fmt_type value {fmt_type::char_t};

            using type = wchar_t;

            [[nodiscard]] constexpr type operator()(const char val) const noexcept
            {
                return static_cast<type>(static_cast<unsigned char>(val));
            }
        };

        template<std::signed_integral T, class CharT2>
            requires(sizeof(T) <= sizeof(int))
        struct transform_arg_t<T, CharT2>
        {
            static constexpr fmt_type value {fmt_type::int_t};

            using type = int;

            [[nodiscard]] constexpr type operator()(const T val) const noexcept
            {
                return static_cast<type>(val);
            }
        };

        template<std::signed_integral T, class CharT2>
            requires(sizeof(T) > sizeof(int))
        struct transform_arg_t<T, CharT2>
        {
            static constexpr fmt_type value {fmt_type::lint_t};

            using type = long long;

            [[nodiscard]] constexpr type operator()(const T val) const noexcept
            {
                return static_cast<type>(val);
            }
        };

        template<std::unsigned_integral T, class CharT2>
            requires(sizeof(T) <= sizeof(unsigned int))
        struct transform_arg_t<T, CharT2>
        {
            static constexpr fmt_type value {fmt_type::uint_t};

            using type = unsigned int;

            [[nodiscard]] constexpr type operator()(const T val) const noexcept
            {
                return static_cast<type>(val);
            }
        };

        template<std::unsigned_integral T, class CharT2>
            requires(sizeof(T) > sizeof(unsigned int))
        struct transform_arg_t<T, CharT2>
        {
            static constexpr fmt_type value {fmt_type::ulint_t};

            using type = unsigned long long;

            [[nodiscard]] constexpr type operator()(const T val) const noexcept
            {
                return static_cast<type>(val);
            }
        };

        template<class CharT2>
        struct transform_arg_t<float, CharT2>
        {
            static constexpr fmt_type value {fmt_type::float_t};

            using type = float;

            [[nodiscard]] constexpr type operator()(const float val) const noexcept
            {
                return val;
            }
        };

        template<class CharT2>
        struct transform_arg_t<double, CharT2>
        {
            static constexpr fmt_type value {fmt_type::double_t};

            using type = double;

            [[nodiscard]] constexpr type operator()(const double val) const noexcept
            {
                return val;
            }
        };

        template<class CharT2>
        struct transform_arg_t<long double, CharT2>
        {
            static constexpr fmt_type value {fmt_type::ldouble_t};

            using type = long double;

            [[nodiscard]] constexpr type operator()(const long double val) const noexcept
            {
                return val;
            }
        };

        template<class Traits, class CharT2>
        struct transform_arg_t<std::basic_string_view<CharT2, Traits>, CharT2>
        {
            static constexpr fmt_type value {fmt_type::string_t};

            using type = std::basic_string_view<CharT2>;

            [[nodiscard]] constexpr type operator()(const std::basic_string_view<CharT2, Traits> val) const noexcept
            {
                return {val.data(), val.size()};
            }
        };

        template<class Traits, class Allocator, class CharT2>
        struct transform_arg_t<std::basic_string<CharT2, Traits, Allocator>, CharT2>
        {
            static constexpr fmt_type value {fmt_type::string_t};

            using type = std::basic_string_view<CharT2>;

            [[nodiscard]] constexpr type operator()(const std::basic_string<CharT2, Traits, Allocator>& val) const noexcept
            {
                return {val.data(), val.size()};
            }
        };

        template<std::size_t N, class CharT2>
        struct transform_arg_t<CharT[N], CharT2>
        {
            static constexpr fmt_type value {fmt_type::cstring_t};

            using type = const CharT2*;

            template<class T>
            [[nodiscard]] constexpr type operator()(const T& val) const noexcept
            {
                return val;
            }
        };
        
        template<class CharT2>
        struct transform_arg_t<const void*, CharT2>
        {
            static constexpr fmt_type value {fmt_type::ptr_t};

            using type = const void*;

            [[nodiscard]] constexpr type operator()(const void* const val) const noexcept
            {
                return val;
            }
        };

        template<class CharT2>
        struct transform_arg_t<void*, CharT2> : public transform_arg_t<const void*, CharT2> {};

        template<class CharT2>
        struct transform_arg_t<std::nullptr_t, CharT2>
        {
            static constexpr fmt_type value {fmt_type::ptr_t};

            using type = const void*;

            [[nodiscard]] constexpr type operator()(const std::nullptr_t) const noexcept
            {
                return nullptr;
            }
        };

        using func_t = void(*)(basic_format_parse_context<CharT>&, basic_format_context<CharT>&, const void*);
    public:
        template<class T>
        using transform_arg = transform_arg_t<T, CharT>;

        class handle
        {
        public:
            void format(basic_format_parse_context<CharT>& parse_ctx, basic_format_context<CharT>& format_ctx) const
            {
                func(parse_ctx, format_ctx, obj);
            }
        private:
            handle(const void* const obj, func_t func) noexcept
                : obj{obj}
                , func{func} {}

            const void* obj;
            func_t func;

            friend class basic_format_arg;
        };

        basic_format_arg() noexcept = default;

        explicit operator bool() const noexcept
        {
            return !std::holds_alternative<std::monostate>(arg);
        }

        template<class Visitor>
        decltype(auto) visit(Visitor&& vis) const
        {
            return std::visit(std::forward<Visitor>(vis), arg);
        }

        template<class R, class Visitor>
        R visit(Visitor&& vis) const
        {
            return std::visit<R>(std::forward<Visitor>(vis), arg);
        }
    private:
        const std::variant<
            std::monostate
        ,   bool
        ,   CharT
        ,   int
        ,   unsigned int
        ,   long long int
        ,   unsigned long long int
        ,   float
        ,   double
        ,   long double
        ,   const CharT*
        ,   std::basic_string_view<CharT>
        ,   const void*
        ,   handle
        > arg;

        template<class T>
            requires(requires{ transform_arg<T>{}; })
        constexpr basic_format_arg(const T& val) noexcept
            : arg{transform_arg<T>{}(val)} {}

        template<class T>
        basic_format_arg(const T& val) noexcept
            : arg{handle{&val,
                [](basic_format_parse_context<CharT>& parse_ctx, basic_format_context<CharT>& format_ctx, const void* obj) {
                    using TD = std::remove_cvref_t<T>;
                    formatter<TD, CharT> f;
                    parse_ctx.advance_to(f.parse(parse_ctx));
                    f.format(*static_cast<const TD*>(obj), format_ctx);
                }}} {}

        template<class CharT2, class... Args>
        friend class format_arg_store;
    };

    template<class CharT, class... Args>
    struct format_arg_store
    {
        constexpr format_arg_store(const Args&... args) noexcept
            : args{args...} {}

        const std::array<basic_format_arg<CharT>, sizeof...(Args)> args;
    };

    export
    template<class CharT>
    class basic_format_args
    {
    public:
        constexpr basic_format_args() noexcept
            : num_args{0} {}

        template<class... Args>
        constexpr basic_format_args(const format_arg_store<CharT, Args...>& store) noexcept
            : args_{store.args.data()}
            , num_args{store.args.size()} {}

        constexpr basic_format_args(const basic_format_args&) noexcept = default;

        [[nodiscard]] constexpr basic_format_arg<CharT> get(const std::size_t i) const noexcept
        {
            if (i >= num_args)
            {
                return {};
            }

            return args_[i];
        }

        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return num_args;
        }
    private:
        const basic_format_arg<CharT>* const args_;
        const std::size_t num_args;
    };

    export
    template<class CharT, class... Args>
    basic_format_args(format_arg_store<CharT, Args...>) -> basic_format_args<CharT>;

    export
    template<class CharT>
    class basic_format_parse_context
    {
    public:
        using char_type       = CharT;
        using iterator        = typename std::basic_string_view<CharT>::const_iterator;
        using const_iterator  = iterator;
    private:
        enum class arg_mode : std::uint8_t
        {
            none,
            automatic,
            manual
        };

        template<class T>
        struct approved_type : std::false_type {};

        template<class T>
            requires(requires { typename basic_format_arg<CharT>::template transform_arg<T>; })
        struct approved_type<T> : std::true_type {};
    public:
        constexpr explicit basic_format_parse_context(const std::basic_string_view<CharT> fmt, const std::size_t num_args) noexcept
            : fmt{fmt}, num_args{num_args}, current_arg{0}, mode{arg_mode::none} {}

        basic_format_parse_context(const basic_format_parse_context&) = delete;
        basic_format_parse_context& operator=(const basic_format_parse_context&) = delete;

        constexpr const_iterator begin() const noexcept
        {
            return fmt.begin();
        }

        constexpr const_iterator end() const noexcept
        {
            return fmt.end();
        }

        constexpr void advance_to(const const_iterator it) noexcept
        {
            fmt = {it, fmt.end()};
        }

        constexpr std::size_t next_arg_id()
        {
            if (mode == arg_mode::manual)
            {
                throw std::format_error{"Cannot get next arg while in manual mode"};
            }
            
            if (current_arg == num_args)
            {
                throw std::format_error{"Argument out of bounds"};
            }

            mode = arg_mode::automatic;
            return current_arg++;
        }

        constexpr void check_arg_id(const std::size_t id)
        {
            if (mode == arg_mode::automatic)
            {
                throw std::format_error{"Cannot check arg while in automatic mode"};
            }

            if (id >= num_args)
            {
                throw std::format_error{"Argument out of bounds"};
            }

            mode = arg_mode::manual;
        }

        template<class... Ts>
        constexpr void check_dynamic_spec(const std::size_t id) const noexcept;

        constexpr void check_dynamic_spec_integral(const std::size_t id) noexcept
        {
            check_dynamic_spec<int, unsigned int, long long, unsigned long long>(id);
        }

        constexpr void check_dynamic_spec_string(const std::size_t id) const noexcept
        {
            check_dynamic_spec<const char_type*, std::basic_string_view<CharT>>(id);
        }
    protected:
        std::basic_string_view<CharT> fmt;
        const std::size_t num_args;
        std::size_t current_arg;
        arg_mode mode;
    };

    export
    using format_parse_context  = basic_format_parse_context<char>;

    export
    using wformat_parse_context = basic_format_parse_context<wchar_t>;

    template<class CharT>
    class compile_parse_context : public basic_format_parse_context<CharT>
    {
    public:
        using basic_format_parse_context<CharT>::basic_format_parse_context;

        template<class T>
        consteval compile_parse_context(const std::basic_string_view<CharT> str, const T& container) noexcept
            : basic_format_parse_context<CharT>{str, std::ranges::size(container)}
            , types_{container} {}

        template<class... Ts>
        consteval void check_dynamic_spec(const std::size_t id) const noexcept
        {
            static_assert(id < this->num_args, "Argument out of bounds");
            static_assert(((types_[id] == basic_format_arg<CharT>::template transform_arg<Ts>::value) || ...), "Dynamic spec is not one of the given types");
        }
    private:
        const std::span<const typename basic_format_arg<CharT>::fmt_type> types_;
    };

    template<class CharT>
    template<class... Ts>
    constexpr void basic_format_parse_context<CharT>::check_dynamic_spec(const std::size_t id) const noexcept
    {
        static_assert(sizeof...(Ts) >= 1, "Expected at least one type");
        static_assert(is_unique_v<Ts...>, "Types must be unique");
        static_assert(std::conjunction_v<approved_type<Ts>...>, "Type is not an expected fmt type");

        // if consteval
        // {
            // using compile_context = compile_parse_context<CharT>;
            // static_cast<const compile_context*>(this)->check_dynamic_spec(id);
        // }
    }

    export
    template<class CharT>
    class basic_format_context
    {
    public:
        using char_type = CharT;

        basic_format_context(basic_text_ostream<CharT>& os, basic_format_args<CharT> args) noexcept
            : os{os}, loc_{nullptr}, args{args} {}

        basic_format_context(basic_text_ostream<CharT>& os, const std::locale& loc, basic_format_args<CharT> args) noexcept
            : os{os}, loc_{&loc}, args{args} {}

        basic_format_context() = delete;
        basic_format_context(const basic_format_context&) = delete;
        basic_format_context& operator=(const basic_format_context&) = delete;

        std::locale locale() const noexcept
        {
            if (loc_)
            {
                return *loc_;
            }

            return {};
        }

        basic_text_ostream<CharT>& stream() noexcept
        {
            return os;
        }

        [[nodiscard]] basic_format_arg<CharT> arg(const std::size_t id) const noexcept
        {
            return args.get(id);
        }
    private:
        basic_text_ostream<CharT>& os;
        const std::locale* loc_;
        basic_format_args<CharT> args;
    };

    export
    using format_context  = basic_format_context<char>;

    export
    using wformat_context = basic_format_context<wchar_t>;

    export
    using format_args  = basic_format_args<char>;

    export
    using wformat_args = basic_format_args<wchar_t>;

    export
    template<class CharT = char, class... Args>
    [[nodiscard]] inline constexpr format_arg_store<CharT, Args...> make_format_args(const Args&... args) noexcept
    {
        return {args...};
    }

    export
    template<class... Args>
    [[nodiscard]] inline constexpr format_arg_store<wchar_t, Args...> make_wformat_args(const Args&... args) noexcept
    {
        return make_format_args<wchar_t>(args...);
    }

    export
    template<class T, class CharT = char>
    concept formattable = 
        requires(formatter<std::remove_cvref_t<T>, CharT>& f, const formatter<std::remove_cvref_t<T>, CharT>& cf, const T& t, basic_format_parse_context<CharT>& parse_ctx, basic_format_context<CharT>& format_ctx)
    {
        { f.parse(parse_ctx) } -> std::same_as<typename basic_format_parse_context<CharT>::iterator>;
        { cf.format(t, format_ctx) };
    };

    template<class It, std::integral T>
    constexpr It parse_num(It begin, const It end, T& arg_id)
    {
        arg_id = 0;
        while (begin != end)
        {
            switch (*begin)
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
                const auto new_arg_id {(arg_id * 10) + (*begin - '0')};
                if (new_arg_id < arg_id)
                {
                    throw std::format_error{"Invalid format string: Number too large"};
                }
                arg_id = new_arg_id;
                break;
            }
            default:
                return begin;
            }

            ++begin;
        }

        return begin;
    }

    template<class CharT>
    void do_format(basic_format_parse_context<CharT>& parse_ctx, basic_format_context<CharT> format_ctx)
    {
        constexpr auto braces {STATICALLY_WIDEN(CharT, "{}")};

        auto begin {parse_ctx.begin()};
        while (begin != parse_ctx.end())
        {
            auto iter {std::ranges::find_first_of(begin, parse_ctx.end(), braces.begin(), braces.end())};
            
            if (iter == parse_ctx.end())
            {
                format_ctx.stream() << std::basic_string_view<CharT>{begin, iter};
                break;
            }
            
            if (*iter == '}')
            {
                ++iter;
                if (iter == parse_ctx.end() || *iter != '}')
                {
                    throw std::format_error{"Unmatched '}' in format string."}; 
                }

                format_ctx.stream() << std::basic_string_view<CharT>{begin, iter};
                ++iter;
                begin = iter;
                continue;
            }

            ++iter;
            if (iter == parse_ctx.end())
            {
                throw std::format_error{"Unmatched '{' in format string."}; 
            }

            if (*iter == '{')
            {
                format_ctx.stream() << std::basic_string_view<CharT>{begin, iter};
                begin = iter;
                continue;
            }
            
            format_ctx.stream() << std::basic_string_view<CharT>{begin, iter - 1};

            std::size_t arg_id;
            
            switch (*iter)
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
                iter = parse_num(iter, parse_ctx.end(), arg_id);
                if (iter == parse_ctx.end())
                {
                    throw std::format_error{"Unexpected end in format string"};
                }
                parse_ctx.check_arg_id(arg_id);

                switch (*iter)
                {
                case ':':
                    ++iter;
                    if (iter == parse_ctx.end())
                    {
                        throw std::format_error{"Unexpected end in format string"};
                    }
                case '}':
                    break;
                default:
                    throw std::format_error{"Unexpected character in format string"};
                }
                break;
            case ':':
                ++iter;
                if (iter == parse_ctx.end())
                {
                    throw std::format_error{"Unexpected end in format string"};
                }
            case '}':
                arg_id = parse_ctx.next_arg_id();
                break;
            default:
                throw std::format_error{"Unexpected character in format string"};
            }

            parse_ctx.advance_to(iter);
            iter = parse_ctx.begin();

            const auto arg {format_ctx.arg(arg_id)};
            if (*iter == '}')
            {
                // Default formatting
                arg.visit(lib2::overloaded {
                    [](const std::monostate) {},
                    [&]<typename T>(const T& value) {
                        format_ctx.stream() << value;
                    },
                    [&](const typename basic_format_arg<CharT>::handle& h) {
                        h.format(parse_ctx, format_ctx);
                    }
                });
            }
            else
            {
                arg.visit(lib2::overloaded {
                    [](const std::monostate) {},
                    [&]<typename T>(const T& value) {
                        formatter<T, CharT> fmt;
                        parse_ctx.advance_to(fmt.parse(parse_ctx));
                        fmt.format(value, format_ctx);
                    },
                    [&](const typename basic_format_arg<CharT>::handle& h) {
                        h.format(parse_ctx, format_ctx);
                    }
                });
            }

            begin = parse_ctx.begin();

            if (begin != parse_ctx.end())
            {
                if (*begin != '}')
                {
                    throw std::format_error{std::format("Formatter at [{}] parsed incorrectly", arg_id)};
                }
                ++begin;
            }
        }
    }

    template<class... Args, class CharT>
        requires(sizeof...(Args) == 0)
    [[noreturn]] consteval typename basic_format_parse_context<CharT>::iterator
    parse_format_compiletime(basic_format_parse_context<CharT>&, const std::size_t)
    {
        throw std::format_error{"No format argument"};
    }

    template<class Arg, class... Args, class CharT>
    consteval typename basic_format_parse_context<CharT>::iterator
    parse_format_compiletime(basic_format_parse_context<CharT>& parse_ctx, const std::size_t arg_id)
    {
        if (arg_id)
        {
            return parse_format_compiletime<Args...>(parse_ctx, arg_id - 1);
        }

        formatter<Arg, CharT> fmt;
        return fmt.parse(parse_ctx);
    }

    template<class... Args, class CharT>
    consteval void do_format_compiletime(basic_format_parse_context<CharT>& parse_ctx)
    {
        constexpr auto braces {STATICALLY_WIDEN(CharT, "{}")};

        auto begin {parse_ctx.begin()};
        while (begin != parse_ctx.end())
        {
            auto iter {std::ranges::find_first_of(begin, parse_ctx.end(), braces.begin(), braces.end())};
            
            if (iter == parse_ctx.end())
            {
                break;
            }
            
            if (*iter == '}')
            {
                ++iter;
                if (iter == parse_ctx.end() || *iter != '}')
                {
                    throw std::format_error{"Unmatched '}' in format string."}; 
                }

                ++iter;
                begin = iter;
                continue;
            }

            ++iter;
            if (iter == parse_ctx.end())
            {
                throw std::format_error{"Unmatched '{' in format string."}; 
            }

            if (*iter == '{')
            {
                begin = iter;
                continue;
            }

            std::size_t arg_id;
            
            switch (*iter)
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
                iter = parse_num(iter, parse_ctx.end(), arg_id);
                if (iter == parse_ctx.end())
                {
                    throw std::format_error{"Unexpected end in format string"};
                }
                parse_ctx.check_arg_id(arg_id);

                switch (*iter)
                {
                case ':':
                    ++iter;
                    if (iter == parse_ctx.end())
                    {
                        throw std::format_error{"Unexpected end in format string"};
                    }
                case '}':
                    break;
                default:
                    throw std::format_error{"Unexpected character in format string"};
                }
                break;
            case ':':
                ++iter;
                if (iter == parse_ctx.end())
                {
                    throw std::format_error{"Unexpected end in format string"};
                }
            case '}':
                arg_id = parse_ctx.next_arg_id();
                break;
            default:
                throw std::format_error{"Unexpected character in format string"};
            }

            parse_ctx.advance_to(iter);
            begin = parse_format_compiletime<Args...>(parse_ctx, arg_id);

            if (begin != parse_ctx.end())
            {
                if (*begin != '}')
                {
                    throw std::format_error{std::format("Formatter at [{}] parsed incorrectly", arg_id)};
                }
                ++begin;
            }
        }
    }

    export
    template<class CharT, class... Args>
    struct basic_format_string;

    template<class CharT>
    struct runtime_format_string
    {
    public:
        runtime_format_string(const runtime_format_string&) = delete;
        runtime_format_string& operator=(const runtime_format_string&) = delete;

        friend runtime_format_string<char> runtime_format(std::string_view fmt) noexcept;
        friend runtime_format_string<wchar_t> runtime_format(std::wstring_view fmt) noexcept;
    private:
        std::basic_string_view<CharT> str;

        runtime_format_string(std::basic_string_view<CharT> s) noexcept
            : str{s} {};

        template<class CharT2, class... Args>
        friend struct basic_format_string;
    };

    export
    inline runtime_format_string<char> runtime_format(std::string_view fmt) noexcept
    {
        return {fmt};
    }

    export
    inline runtime_format_string<wchar_t> runtime_format(std::wstring_view fmt) noexcept
    {
        return {fmt};
    }

    export
    template<class CharT, class... Args>
    struct basic_format_string
    {
        template<class Arg>
        struct targ
        {
            using type = std::remove_cvref_t<Arg>;
            
            static constexpr auto value {basic_format_arg<CharT>::fmt_type::handle_t};
        };

        template<class Arg>
            requires(requires{typename basic_format_arg<CharT>::template transform_arg<Arg>::type;})
        struct targ<Arg> : public basic_format_arg<CharT>::template transform_arg<Arg> {};
    public:
        template<std::convertible_to<std::basic_string_view<CharT>> T>
        consteval basic_format_string(const T& s)
            : str{s}
        {
            constexpr typename basic_format_arg<CharT>::fmt_type types[] {targ<Args>::value...};
            compile_parse_context<CharT> parse_ctx{str, types};
            do_format_compiletime<typename targ<Args>::type...>(parse_ctx);
        }

        basic_format_string(const runtime_format_string<CharT> rstr) noexcept
            : str{rstr.str} {}

        constexpr std::basic_string_view<CharT> get() const noexcept
        {
            return str;
        }
    private:
        std::basic_string_view<CharT> str;
    };

    export
    template<class... Args>
    using format_string = basic_format_string<char, std::type_identity_t<Args>...>;

    export
    template<class... Args>
    using wformat_string = basic_format_string<wchar_t, std::type_identity_t<Args>...>;

    export
    inline text_ostream& vformat_to(text_ostream& os, const std::string_view fmt, const format_args args)
    {
        format_parse_context parse_ctx {fmt, args.size()};
        do_format(parse_ctx, format_context{os, args});
        return os;
    }

    export
    inline wtext_ostream& vformat_to(wtext_ostream& os, const std::wstring_view fmt, const wformat_args args)
    {
        wformat_parse_context parse_ctx {fmt, args.size()};
        do_format(parse_ctx, wformat_context{os, args});
        return os;
    }

    export
    inline text_ostream& vformat_to(text_ostream& os, const std::locale& loc, const std::string_view fmt, const format_args args)
    {
        format_parse_context parse_ctx {fmt, args.size()};
        do_format(parse_ctx, format_context{os, loc, args});
        return os;
    }

    export
    inline wtext_ostream& vformat_to(wtext_ostream& os, const std::locale& loc, const std::wstring_view fmt, const wformat_args args)
    {
        wformat_parse_context parse_ctx {fmt, args.size()};
        do_format(parse_ctx, wformat_context{os, loc, args});
        return os;
    }

    export
    inline std::string vformat(const std::string_view fmt, const format_args args)
    {
        ostringstream ss;
        vformat_to(ss, fmt, args);

        return std::move(ss).str();
    }

    export
    inline std::wstring vformat(const std::wstring_view fmt, const wformat_args args)
    {
        wostringstream ss;
        vformat_to(ss, fmt, args);

        return std::move(ss).str();
    }

    export
    inline std::string vformat(const std::locale& loc, const std::string_view fmt, const format_args args)
    {
        ostringstream ss;
        vformat_to(ss, loc, fmt, args);

        return std::move(ss).str();
    }

    export
    inline std::wstring vformat(const std::locale& loc, const std::wstring_view fmt, const wformat_args args)
    {
        wostringstream ss;
        vformat_to(ss, loc, fmt, args);

        return std::move(ss).str();
    }

    export
    template<class... Args>
    inline text_ostream& format_to(text_ostream& os, const format_string<Args...> fmt, const Args&... args)
    {
        return vformat_to(os, fmt.get(), make_format_args(args...));
    }

    export
    template<class... Args>
    inline wtext_ostream& format_to(wtext_ostream& os, const wformat_string<Args...> fmt, const Args&... args)
    {
        return vformat_to(os, fmt.get(), make_wformat_args(args...));
    }

    export
    template<class... Args>
    inline text_ostream& format_to(text_ostream& os, const std::locale& loc, const format_string<Args...> fmt, const Args&... args)
    {
        return vformat_to(os, loc, fmt.get(), make_format_args(args...));
    }

    export
    template<class... Args>
    inline wtext_ostream& format_to(wtext_ostream& os, const std::locale& loc, const wformat_string<Args...> fmt, const Args&... args)
    {
        return vformat_to(os, loc, fmt.get(), make_wformat_args(args...));
    }

    export
    template<class... Args>
    inline std::string format(const format_string<Args...> fmt, const Args&... args)
    {
        return vformat(fmt.get(), make_format_args(args...));
    }

    export
    template<class... Args>
    inline std::wstring format(const wformat_string<Args...> fmt, const Args&... args)
    {
        return vformat(fmt.get(), make_wformat_args(args...));
    }

    export
    template<class... Args>
    inline std::string format(const std::locale& loc, const format_string<Args...> fmt, const Args&... args)
    {
        return vformat(loc, fmt.get(), make_format_args(args...));
    }

    export
    template<class... Args>
    inline std::wstring format(const std::locale& loc, const wformat_string<Args...> fmt, const Args&... args)
    {
        return vformat(loc, fmt.get(), make_wformat_args(args...));
    }

    export
    template<class... Args>
    inline std::size_t formatted_size(const format_string<Args...> fmt, const Args&... args)
    {
        lib2::size_ostream<char> sos;
        format_to(sos, fmt, args...);
        return sos.size();
    }

    export
    template<class... Args>
    inline std::size_t formatted_size(const wformat_string<Args...> fmt, const Args&... args)
    {
        lib2::size_ostream<wchar_t> sos;
        format_to(sos, fmt, args...);
        return sos.size();
    }

    export
    template<class... Args>
    inline std::size_t formatted_size(const std::locale& loc, const format_string<Args...> fmt, const Args&... args)
    {
        lib2::size_ostream<char> sos;
        format_to(sos, loc, fmt, args...);
        return sos.size();
    }

    export
    template<class... Args>
    inline std::size_t formatted_size(const std::locale& loc, const wformat_string<Args...> fmt, const Args&... args)
    {
        lib2::size_ostream<wchar_t> sos;
        format_to(sos, loc, fmt, args...);
        return sos.size();
    }
}

#include "parsers.ixx"
#include "string.ixx"
#include "arithmetic.ixx"
#include "misc.ixx"
#include "ranges.ixx"