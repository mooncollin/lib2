export module lib2.fmt:context;

import std;

import lib2.io;
import lib2.type_traits;
import lib2.concepts;

import :formatter;

namespace lib2
{
    template<class... Args>
    struct format_arg_store;

    export
    class format_arg;

    export
    class format_args
    {
    public:
        constexpr format_args() noexcept
            : args_{nullptr}, num_args{0} {}

        template<class... Args>
        constexpr format_args(const format_arg_store<Args...>& store) noexcept
            : args_{store.args.data()}
            , num_args{store.args.size()} {}

        constexpr format_args(const format_args&) noexcept = default;

        [[nodiscard]] constexpr format_arg get(const std::size_t i) const noexcept;

        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return num_args;
        }
    private:
        const format_arg* const args_;
        const std::size_t num_args;
    };

    export
    class format_parse_context
    {
        static constexpr std::size_t manual_indexing {std::numeric_limits<std::size_t>::max()};
    public:
        constexpr format_parse_context(const std::string_view str, const std::size_t num_args) noexcept
            : begin{str.data()}, end{str.data() + str.size()}, num_args{num_args}, i{0} {}

        format_parse_context(const format_parse_context&) = delete;
        format_parse_context& operator=(const format_parse_context&) = delete;

        const char* begin;
        const char* const end;

        constexpr std::size_t next_arg_id()
        {
            if (i == manual_indexing)
            {
                throw format_error{"Cannot enter automatic mode while in manual mode"};
            }
            
            if (i == num_args)
            {
                throw format_error{"Argument out of bounds"};
            }

            return i++;
        }

        constexpr void check_arg_id(const std::size_t id)
        {
            if (i != 0 && i != manual_indexing)
            {
                throw format_error{"Cannot enter manual mode while in automatic mode"};
            }

            if (id >= num_args)
            {
                throw format_error{"Argument out of bounds"};
            }

            i = manual_indexing;
        }

        [[nodiscard]] constexpr format_arg arg(const std::size_t id) const noexcept;

        template<class... Ts>
        constexpr void check_dynamic_spec(const std::size_t id) const;

        constexpr void check_dynamic_spec_integral(const std::size_t id) const
        {
            check_dynamic_spec<std::intmax_t, std::uintmax_t>(id);
        }

        constexpr void check_dynamic_spec_string(const std::size_t id) const
        {
            check_dynamic_spec<std::string_view>(id);
        }
    private:
        std::size_t num_args;
        std::size_t i;
    };

    export
    class format_context
    {
    public:
        format_context(const format_context&) = delete;
        format_context& operator=(const format_context&) = delete;
        
        format_context(io::dyn_sink out, const format_args args = {}) noexcept
            : out{out}, args{args}, loc{nullptr} {}

        format_context(const std::locale& loc, io::dyn_sink out, const format_args args = {}) noexcept
            : out{out}, args{args}, loc{std::addressof(loc)} {}

        format_context(const format_context& other, io::dyn_sink out) noexcept
            : out{out}, args{other.args}, loc{other.loc} {}

        [[nodiscard]] std::locale locale() const noexcept
        {
            if (loc)
            {
                return *loc;
            }

            return {};
        }

        io::dyn_sink out;
        const format_args args;
    private:
        const std::locale* const loc;
    };

    export
    template<class T>
    concept formattable =
        requires(const T& value, format_parse_context& parse_ctx, format_context& format_ctx, formatter<T>& fmt, const formatter<T>& cfmt)
    {
        { fmt.parse(parse_ctx) } -> std::same_as<const char*>;
        { cfmt.format(value, format_ctx) };
    };

    export
    template<class T>
    concept default_formattable = formattable<T> &&
        requires(const T& value, format_context& format_ctx)
    {
        { formatter<T>::default_format(value, format_ctx) };
    };

    // For making nice errors
    template<class T>
    struct assert_formattable
    {
        static_assert(formattable<T>, "Type is not formattable");
    };

    export
    class format_arg
    {
    public:
        class handle
        {
        public:
            void format(format_parse_context& parse_ctx, format_context& format_ctx) const
            {
                func(parse_ctx, format_ctx, obj);
            }
            
            template<class T>
            handle(const T& obj) noexcept
                : obj{std::addressof(obj)}
                , func{[](format_parse_context& parse_ctx, format_context& format_ctx, const void* const obj) {

                    if constexpr (default_formattable<T>)
                    {
                        if (parse_ctx.begin == parse_ctx.end || *parse_ctx.begin == '}')
                        {
                            formatter<T>::default_format(*static_cast<const T*>(obj), format_ctx);
                        }
                        else
                        {
                            formatter<T> fmtr;
                            parse_ctx.begin = fmtr.parse(parse_ctx);
                            fmtr.format(*static_cast<const T*>(obj), format_ctx);
                        }
                    }
                    else
                    {
                        formatter<T> fmtr;
                        parse_ctx.begin = fmtr.parse(parse_ctx);
                        fmtr.format(*static_cast<const T*>(obj), format_ctx);
                    }
                }} {}

            const void* obj;
            void(*func)(format_parse_context&, format_context&, const void*);
        };

        enum class arg_type : std::uint8_t
        {
            none,
            boolean,
            character,
            signed_integer,
            unsigned_integer,
            floating,
            string,
            pointer,
            custom
        };

        template<class T>
        [[nodiscard]] static consteval arg_type determine_format_arg() noexcept
        {
            if constexpr (std::same_as<T, char>)
            {
                return arg_type::character;
            }
            else if constexpr (std::same_as<T, bool>)
            {
                return arg_type::boolean;
            }
            else if constexpr (std::same_as<T, std::string_view> || std::same_as<T, const char*> || std::same_as<T, char> || (std::is_array_v<T> && std::same_as<std::remove_const_t<std::remove_extent_t<T>>, char>) || specialization_of<T, std::basic_string>)
            {
                return arg_type::string;
            }
            else if constexpr (std::same_as<T, const void*> || std::same_as<T, void*> || std::same_as<T, std::nullptr_t>)
            {
                return arg_type::pointer;
            }
            else if constexpr (std::signed_integral<T>)
            {
                return arg_type::signed_integer;
            }
            else if constexpr (std::unsigned_integral<T>)
            {
                return arg_type::unsigned_integer;
            }
            else if constexpr (std::floating_point<T>)
            {
                return arg_type::floating;
            }
            else
            {
                return arg_type::custom;
            }
        }

        constexpr format_arg() noexcept
            : type{arg_type::none} {}

        constexpr format_arg(const format_arg& other) noexcept = default;

        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            return type != arg_type::none;
        }

        template<class Visitor>
        constexpr decltype(auto) visit(Visitor&& vis) const
        {
            switch (type)
            {
            case arg_type::boolean:
                return std::invoke(std::forward<Visitor>(vis), b);
            case arg_type::character:
                return std::invoke(std::forward<Visitor>(vis), c);
            case arg_type::signed_integer:
                return std::invoke(std::forward<Visitor>(vis), si);
            case arg_type::unsigned_integer:
                return std::invoke(std::forward<Visitor>(vis), ui);
            case arg_type::floating:
                return std::invoke(std::forward<Visitor>(vis), f);
            case arg_type::string:
                return std::invoke(std::forward<Visitor>(vis), s);
            case arg_type::pointer:
                return std::invoke(std::forward<Visitor>(vis), p);
            case arg_type::custom:
                return std::invoke(std::forward<Visitor>(vis), h);
            default:
                throw std::bad_variant_access{};
            }
        }

        template<class R, class Visitor>
        constexpr R visit(Visitor&& vis) const
        {
            return visit(std::forward<Visitor>(vis));
        }
    private:
        arg_type type;
        union
        {
            bool b;
            char c;
            std::intmax_t si;
            std::uintmax_t ui;
            long double f;
            std::string_view s;
            const void* p;
            handle h;
        };

        template<class T>
            requires(std::same_as<T, bool>)
        constexpr explicit format_arg(const T value) noexcept
            : type{arg_type::boolean}, b{value} {}

        template<class T>
            requires(std::same_as<T, char>)
        constexpr explicit format_arg(const T value) noexcept
            : type{arg_type::character}, c{value} {}

        template<std::signed_integral T>
            requires(!std::same_as<T, char> && !std::same_as<T, bool>)
        constexpr explicit format_arg(const T value) noexcept
            : type{arg_type::signed_integer}, si{value} {}

        template<std::unsigned_integral T>
            requires(!std::same_as<T, char> && !std::same_as<T, bool>)
        constexpr explicit format_arg(const T value) noexcept
            : type{arg_type::unsigned_integer}, ui{value} {}

        template<std::floating_point T>
        constexpr explicit format_arg(const T value) noexcept
            : type{arg_type::floating}, f{value} {}

        constexpr format_arg(const std::string_view val) noexcept
            : type{arg_type::string}, s{val} {}

        template<class Allocator>
        constexpr format_arg(const std::basic_string<char, std::char_traits<char>, Allocator>& val) noexcept
            : format_arg{std::string_view{val.data(), val.size()}} {}

        constexpr format_arg(const char* const val) noexcept
            : format_arg{std::string_view{val}} {}

        constexpr format_arg(const void* const val) noexcept
            : type{arg_type::pointer}, p{val} {}

        template<class T>
            requires(!std::same_as<T, bool> && !std::same_as<T, char> && !std::integral<T> && !std::floating_point<T> && !std::same_as<T, std::string_view> && !std::same_as<std::remove_cvref_t<T>, void*> && !std::same_as<std::remove_cvref_t<T>, char*> && !std::same_as<T, format_arg>)
        constexpr format_arg(const T& val) noexcept
            : type{arg_type::custom}, h{val} {}

        template<class... Args>
        friend class format_arg_store;
    };

    template<class... Args>
    struct format_arg_store
    {
        constexpr format_arg_store(const Args&... args) noexcept
            : args{format_arg(args)...} {}

        const std::array<format_arg, sizeof...(Args)> args;
    };

    [[nodiscard]] constexpr format_arg format_args::get(const std::size_t i) const noexcept
    {
        if (i >= num_args)
        {
            return {};
        }

        return args_[i];
    }

    export
    template<class... Args>
    [[nodiscard]] inline constexpr format_arg_store<Args...> make_format_args(const Args&... args) noexcept
    {
        constexpr std::tuple<assert_formattable<Args>...> check {};
        return {args...};
    }

    template<class... Args>
    struct format_type_store
    {
        constexpr format_type_store() noexcept
            : args{format_arg::determine_format_arg<Args>()...} {}

        const std::array<format_arg::arg_type, sizeof...(Args)> args;
    };

    class format_type_args
    {
    public:
        template<class... Args>
        constexpr format_type_args(const format_type_store<Args...>& store) noexcept
            : args{store.args.data()}
            , num_args{store.args.size()} {}

        constexpr format_type_args(const format_type_args&) noexcept = default;

        [[nodiscard]] constexpr format_arg::arg_type get(const std::size_t i) const noexcept
        {
            return args[i];
        }

        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return num_args;
        }
    private:
        const format_arg::arg_type* const args;
        const std::size_t num_args;
    };

    class compile_format_parse_context : public format_parse_context
    {
    public:
        constexpr compile_format_parse_context(const std::string_view str, const format_type_args args) noexcept
            : format_parse_context{str, args.size()}
            , args{args} {}
        
        format_type_args args;
    };

    template<class T>
    struct approved_type : std::false_type {};

    template<>
    struct approved_type<bool> : std::true_type {};

    template<>
    struct approved_type<char> : std::true_type {};

    template<>
    struct approved_type<std::intmax_t> : std::true_type {};

    template<>
    struct approved_type<std::uintmax_t> : std::true_type {};

    template<>
    struct approved_type<long double> : std::true_type {};

    template<>
    struct approved_type<std::string_view> : std::true_type {};

    template<>
    struct approved_type<const void*> : std::true_type {};

    template<class... Ts>
    constexpr void format_parse_context::check_dynamic_spec(const std::size_t id) const
    {
        static_assert(sizeof...(Ts) >= 1, "Expected at least one type");
        static_assert(is_unique_v<Ts...>, "Types must be unique");
        static_assert(std::conjunction_v<approved_type<Ts>...>, "Type is not an expected fmt type");

        if (id >= num_args)
        {
            throw format_error{"Argument out of bounds"};
        }

        if consteval
        {
            const auto& compile_ctx {static_cast<const compile_format_parse_context&>(*this)};
            const auto type {compile_ctx.args.get(id)};

            if (((format_arg::determine_format_arg<Ts>() != type) && ...))
            {
                throw format_error{"check_dynamic_spec failed"};
            }
        }
    }
}