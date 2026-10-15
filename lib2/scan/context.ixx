export module lib2.scan:context;

import std;

import lib2.io;

import :scan_error;
import :utility;

namespace lib2
{
    export
    class scan_context;

    export
    class scan_arg
    {
    public:
        constexpr scan_arg() noexcept
            : obj{nullptr}, func{nullptr} {}

        scan_arg(const scan_arg&) noexcept = default;

        template<class T>
            requires(!std::same_as<T, scan_arg>)
        constexpr scan_arg(T& obj) noexcept
            : obj{std::addressof(obj)}
            , func{[](text_istream& is, scan_context& ctx, void* const obj) -> std::expected<void, scan_error> {
                return from_stream(is, *static_cast<T*>(obj), scan_format_of_t<T>::parse(ctx));
              }} {}

        std::expected<void, scan_error> scan(text_istream& is, scan_context& ctx)
        {
            return func(is, ctx, obj);
        }

        constexpr explicit operator bool() const noexcept
        {
            return static_cast<bool>(obj);
        }
    private:
        void* obj;
        std::expected<void, scan_error> (*func)(text_istream& is, scan_context&, void*);
    };

    template<class... Ts>
    struct scan_arg_store
    {
        std::array<scan_arg, sizeof...(Ts)> args;
    };

    export
    class scan_args
    {
    public:
        constexpr scan_args(const scan_args&) noexcept = default;

        template<class... Args>
        constexpr scan_args(scan_arg_store<Args...>& store) noexcept
            : args_{store.args.data()}, size_{store.args.size()} {}
            
        [[nodiscard]] constexpr scan_arg get(const std::size_t i) const noexcept
        {
            if (i >= size_)
            {
                return {};
            }

            return args_[i];
        }

        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return size_;
        }
    private:
        scan_arg* args_;
        std::size_t size_;
    };

    export
    template<class... Args>
    constexpr scan_arg_store<Args...> make_scan_args(Args&... args) noexcept
    {
        return {args...};
    }
    
    export
    class scan_context
    {
        static constexpr std::size_t manual_indexing {std::numeric_limits<std::size_t>::max()};
    public:
        using iterator = std::string_view::const_iterator;
        using const_iterator = iterator;

        constexpr scan_context(const std::string_view fmt, const scan_args args) noexcept
            : fmt{fmt}, args{args}, current_arg{0}, loc{nullptr} {}

        constexpr scan_context(const std::string_view fmt, const scan_args args, const std::locale& loc) noexcept
            : fmt{fmt}, args{args}, current_arg{0}, loc{&loc} {}

        scan_context(const scan_context&) = delete;
        scan_context& operator=(const scan_context&) = delete;

        constexpr scan_arg arg(const std::size_t id) const noexcept
        {
            return args.get(id);
        }

        const std::locale* locale() const noexcept
        {
            return loc;
        }

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
            if (current_arg == manual_indexing)
            {
                throw scan_parse_error{"Cannot mix indexing modes"};
            }

            if (current_arg == args.size())
            {
                throw scan_parse_error{"Argument out of bounds"};
            }

            return current_arg++;
        }

        constexpr void check_arg_id(const std::size_t id)
        {
            if (current_arg != 0 && current_arg != manual_indexing)
            {
                throw scan_parse_error{"Cannot mix indexing modes"};
            }

            if (id >= args.size())
            {
                throw scan_parse_error{"Argument out of bounds"};
            }

            current_arg = manual_indexing;
        }
    private:
        std::string_view fmt;
        const scan_args args;
        std::size_t current_arg;
        const std::locale* const loc;
    };
}