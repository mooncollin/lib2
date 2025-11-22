export module lib2.fmt:context;

import std;

import lib2.io;

import :utility;

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
    class format_context
    {
    public:
        using iterator        = std::string_view::const_iterator;
        using const_iterator  = iterator;
    private:
        enum class arg_mode : std::uint8_t
        {
            none,
            automatic,
            manual
        };
    public:
        constexpr format_context(const std::string_view fmt, const format_args args) noexcept
            : fmt{fmt}, args{args}, loc_{nullptr}, current_arg{0}, mode{arg_mode::none} {}

        format_context(const std::string_view fmt, const format_args args, const std::locale& loc) noexcept
            : fmt{fmt}, args{args}, loc_{std::addressof(loc)}, current_arg{0}, mode{arg_mode::none} {}

        format_context(const format_context&) = delete;
        format_context& operator=(const format_context&) = delete;

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

        constexpr const std::locale* locale() const noexcept
        {
            return loc_;
        }

        constexpr std::size_t next_arg_id()
        {
            if (mode == arg_mode::manual)
            {
                throw std::format_error{"Cannot get next arg while in manual mode"};
            }
            
            if (current_arg == args.size())
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

            if (id >= args.size())
            {
                throw std::format_error{"Argument out of bounds"};
            }

            mode = arg_mode::manual;
        }

        [[nodiscard]] constexpr format_arg arg(const std::size_t id) const noexcept;

        // template<class... Ts>
        // constexpr void check_dynamic_spec(const std::size_t id) const noexcept;

        // constexpr void check_dynamic_spec_integral(const std::size_t id) noexcept
        // {
        //     check_dynamic_spec<int, unsigned int, long long, unsigned long long>(id);
        // }

        // constexpr void check_dynamic_spec_string(const std::size_t id) const noexcept
        // {
        //     check_dynamic_spec<const char_type*, std::basic_string_view<CharT>>(id);
        // }
    protected:
        std::string_view fmt;
        const format_args args;
        const std::locale* loc_;
        std::size_t current_arg;
        arg_mode mode;
    };

    export
    class format_arg
    {
    public:
        class handle
        {
        public:
            void format(text_ostream& os, format_context& ctx) const
            {
                func(os, ctx, obj);
            }
            
            template<class T>
            handle(const T& obj) noexcept
                : obj{std::addressof(obj)}
                , func{[](text_ostream& os, format_context& ctx, const void* const obj) {
                    to_stream(os, *static_cast<const T*>(obj), format_of_t<T>::parse(ctx));
                }} {}

            const void* obj;
            void(*func)(text_ostream& os, format_context&, const void*);
        };

        format_arg() noexcept = default;

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
        ,   char
        ,   std::intmax_t
        ,   std::uintmax_t
        ,   float
        ,   double
        ,   long double
        ,   std::string_view
        ,   const void*
        ,   handle
        > arg;

        template<class T>
        struct transform : std::type_identity<handle> {};

        template<>
        struct transform<char> : std::type_identity<char> {};

        template<>
        struct transform<bool> : std::type_identity<bool> {};

        template<std::signed_integral T>
        struct transform<T> : std::type_identity<std::intmax_t> {};

        template<std::unsigned_integral T>
        struct transform<T> : std::type_identity<std::uintmax_t> {};

        template<>
        struct transform<float> : std::type_identity<float> {};

        template<>
        struct transform<double> : std::type_identity<double> {};

        template<>
        struct transform<long double> : std::type_identity<long double> {};

        template<class Traits>
        struct transform<std::basic_string_view<char, Traits>> : std::type_identity<std::string_view> {};

        template<>
        struct transform<const char*> : std::type_identity<std::string_view> {};

        template<std::size_t N>
        struct transform<char[N]> : std::type_identity<std::string_view> {};

        template<class Traits, class Allocator>
        struct transform<std::basic_string<char, Traits, Allocator>> : std::type_identity<std::string_view> {};

        template<>
        struct transform<const void*> : std::type_identity<const void*> {};

        template<>
        struct transform<std::nullptr_t> : std::type_identity<const void*> {};

        template<class T>
        constexpr format_arg(const T& val) noexcept
            : arg{std::in_place_type<typename transform<T>::type>, val} {}

        template<class... Args>
        friend class format_arg_store;
    };

    template<class... Args>
    struct format_arg_store
    {
        constexpr format_arg_store(const Args&... args) noexcept
            : args{args...} {}

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

    [[nodiscard]] constexpr format_arg format_context::arg(const std::size_t id) const noexcept
    {
        return args.get(id);
    }

    export
    template<class... Args>
    [[nodiscard]] inline constexpr format_arg_store<Args...> make_format_args(const Args&... args) noexcept
    {
        constexpr std::tuple<assert_formattable<Args>...> check {};
        return {args...};
    }

    // template<class CharT>
    // class compile_parse_context : public basic_format_parse_context<CharT>
    // {
    // public:
    //     using basic_format_parse_context<CharT>::basic_format_parse_context;

    //     template<class T>
    //     consteval compile_parse_context(const std::basic_string_view<CharT> str, const T& container) noexcept
    //         : basic_format_parse_context<CharT>{str, std::ranges::size(container)}
    //         , types_{container} {}

    //     template<class... Ts>
    //     consteval void check_dynamic_spec(const std::size_t id) const noexcept
    //     {
    //         static_assert(id < this->num_args, "Argument out of bounds");
    //         static_assert(((types_[id] == format_arg<CharT>::template transform_arg<Ts>::value) || ...), "Dynamic spec is not one of the given types");
    //     }
    // private:
    //     const std::span<const typename format_arg<CharT>::fmt_type> types_;
    // };

    // template<class CharT>
    // template<class... Ts>
    // constexpr void format_context<CharT>::check_dynamic_spec(const std::size_t id) const noexcept
    // {
    //     static_assert(sizeof...(Ts) >= 1, "Expected at least one type");
    //     static_assert(is_unique_v<Ts...>, "Types must be unique");
    //     static_assert(std::conjunction_v<approved_type<Ts>...>, "Type is not an expected fmt type");

    //     // if consteval
    //     // {
    //         // using compile_context = compile_parse_context<CharT>;
    //         // static_cast<const compile_context*>(this)->check_dynamic_spec(id);
    //     // }
    // }
}