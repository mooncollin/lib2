export module lib2.scan:basic_scan_arg;

import std;

import :scan_error;
import :scanner;
import :basic_scan_parse_context;

namespace lib2
{
    export
    template<class Context>
    class basic_scan_arg
    {
    public:
        constexpr basic_scan_arg(const basic_scan_arg&) noexcept = default;

        template<class T>
            requires(!std::same_as<std::remove_cvref_t<T>, basic_scan_arg>)
        constexpr basic_scan_arg(T& obj) noexcept
            : obj{std::addressof(obj)}
            , func{[](basic_scan_parse_context<typename Context::char_type>& parse_ctx, Context& ctx, void* const obj) {
                scanner<T, typename Context::char_type> scn;
                parse_ctx.advance_to(scn.parse(parse_ctx));
                if (parse_ctx.begin() != parse_ctx.end() && *parse_ctx.begin() != '}')
                {
                    throw scan_parse_error{std::format("scanner<{}, {}>::parse did not finish parsing", typeid(T).name(), typeid(typename Context::char_type).name())}; 
                }

                try
                {
                    return scn.scan(*static_cast<T*>(obj), ctx);
                }
                catch (const std::bad_alloc&)
                {
                    throw;
                }
                catch (const std::bad_exception&)
                {
                    throw;
                }
                catch (const scan_parse_error&)
                {
                    throw;
                }
                catch (const std::exception&)
                {
                    throw bad_scan{};
                }
              }} {}

        auto scan(basic_scan_parse_context<typename Context::char_type>& parse_ctx, Context& ctx)
        {
            return std::invoke(func, parse_ctx, ctx, obj);
        }
    private:
        void* obj;
        typename Context::iterator (*func)(basic_scan_parse_context<typename Context::char_type>&, Context&, void*);
    };

    template<class Context, class... Ts>
    struct scan_arg_store
    {
        std::array<basic_scan_arg<Context>, sizeof...(Ts)> args;
    };

    export
    template<class Context>
    class basic_scan_args
    {
    public:
        template<class... Args>
        constexpr basic_scan_args(scan_arg_store<Context, Args...>& store) noexcept
            : args_{store.args.data()}, size_{store.args.size()} {}
            
        constexpr basic_scan_arg<Context> get(const std::size_t i) const
        {
            if (i >= size_)
            {
                throw scan_parse_error{"arg index out of range"};
            }

            return args_[i];
        }
    private:
        basic_scan_arg<Context>* args_;
        std::size_t size_;
    };

    export
    template<class Context, class... Args>
    basic_scan_args(scan_arg_store<Context, Args...>) -> basic_scan_args<Context>;

    export
    template<class Context, class... Args>
    constexpr scan_arg_store<Context, Args...> make_scan_args(Args&... args) noexcept
    {
        return {args...};
    }
}