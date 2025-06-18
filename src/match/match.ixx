export module lib2.match;
export import :match_result;

import std;

import lib2.type_traits;
import lib2.concepts;

namespace lib2::match
{
    export
    class pattern_not_matched : public std::exception
    {
    public:
        constexpr pattern_not_matched() noexcept = default;

        const char* what() const noexcept override
        {
            return "pattern not matched";
        }
    };

    export
    template<class F>
    struct Ok
    {
        F func;
    };

    export
    template<class F>
    Ok(F) -> Ok<F>;

    export
    template<class F>
    struct Err
    {
        F func;
    };

    export
    template<class F>
    Err(F) -> Err<F>;

    export
    template<class... Ps>
    class patterns
    {
        static_assert(false, "A user must specialize on patterns");
    };

    export
    template<class... Ps>
    patterns(Ps...) -> patterns<Ps...>;

    export
    template<class F1, class... Errs>
        requires(std::conjunction_v<lib2::is_specialization<Errs, Err>...>)
    class patterns<Errs...>
    {
    public:
        constexpr patterns(Errs... errs)
            : errs_{std::move(errs)...} {}

        template<class T, class E>
        constexpr decltype(auto) operator()(std::expected<T, E> ex)
        {
            if (!ex)
            {
                return std::invoke(err_.func, ex.error());
            }
        }

        template<class E>
        constexpr decltype(auto) operator()(std::expected<void, E> ex)
        {
            if (ex)
            {
                return std::invoke(ok_.func);
            }

            return std::invoke(err_.func, ex.error());
        }
    protected:
        std::tuple<Errs...> errs_;

        template<class E>
        constexpr decltype(auto) call_on_err(E&& err)
        {
            static_assert(std::conjunction_v<std::is_invocable<Errs, E>...>, "pattern not matched");
            return std::apply()
        }
    private:
        
    };

    export
    template<class F1, class... Errs>
        requires(std::conjunction_v<lib2::is_specialization<Errs, Err>...>)
    class patterns<Ok<F1>, Errs...> : public patterns<Errs...>
    {
    public:
        constexpr patterns(Ok<F1> ok_, Errs... errs_)
            : patterns<Errs...>{std::move(errs_)...}
            , ok_{std::move(ok_)} {}

        template<class T, class E>
        constexpr decltype(auto) operator()(std::expected<T, E> ex)
        {
            if (ex)
            {
                return std::invoke(ok_.func, ex.value());
            }

            return std::invoke(err_.func, ex.error());
        }

        template<class E>
        constexpr decltype(auto) operator()(std::expected<void, E> ex)
        {
            if (ex)
            {
                return std::invoke(ok_.func);
            }

            return std::invoke(err_.func, ex.error());
        }

        template<class F, class... Args>
            requires(std::invocable<F, Args...>)
        constexpr decltype(auto) operator()(F&& func, Args&&... args)
        {
            using result_t = std::invoke_result_t<F, Args...>;

            if constexpr (lib2::is_function_arity<F2, 1>)
            {
                if constexpr (std::is_void_v<result_t>)
                {
                    try
                    {
                        std::invoke(std::forward<F>(func), std::forward<Args>(args)...);
                    }
                    catch (const typename function_traits<F2>::template arg<1>& e)
                    {
                        return std::invoke(err_.func, e);
                    }

                    return std::invoke(ok_.func);
                }
                else
                {
                    std::optional<result_t> r;

                    try
                    {
                        r = std::invoke(std::forward<F>(func), std::forward<Args>(args)...);
                    }
                    catch (const typename function_traits<F2>::template arg<1>& e)
                    {
                        return std::invoke(err_.func, e);
                    }

                    return std::invoke(ok_.func, std::move(*r));
                }
            }
            else
            {
                if constexpr (std::is_void_v<result_t>)
                {
                    return std::invoke(ok_.func);
                }
                else
                {
                    return std::invoke(ok_.func, std::invoke(std::forward<F>(func), std::forward<Args>(args)...));
                }
            }
        }

        template<class F, class... Args>
            requires(std::invocable<F, Args...>)
        constexpr void operator()(F&& func, Args&&... args)
        {
            bool called {false};
            return call_with_errs<Errs...>(called, std::forward<F>(func), std::forward<Args>(args)...);
        }

        // template<class F, class... Args>
        //     requires(std::invocable<F, Args...> && lib2::specialization_of<std::invoke_result_t<F, Args...>, std::expected>)
        // constexpr decltype(auto) operator()(F&& func, Args&&... args)
        // {
        //     if constexpr (lib2::is_function_arity<F2, 1>)
        //     {
        //         std::optional<std::invoke_result_t<F, Args...>> r;
        //         try
        //         {
        //             r = std::invoke(std::forward<F>(func), std::forward<Args>(args)...);
        //         }
        //         catch (const typename function_traits<F2>::template arg<1>& e)
        //         {
        //             return std::invoke(err_.func, e);
        //         }

        //         return (*this)(*std::move(r));
        //     }
        //     else
        //     {
        //         return (*this)(std::invoke(std::forward<F>(func), std::forward<Args>(args)...));
        //     }
        // }
    private:
        Ok<F1> ok_;

        template<class Err, class... Errs, class F, class... Args>
        constexpr void call_with_errs(bool& called, F&& func, Args&&... args)
        {
            try
            {
                call_with_errs<Errs...>(called, std::forward<F>(func), std::forward<Args>(args)...);
            }
            catch (const Err& e)
            {
                if (!called)
                {
                    called = true;
                    std::invoke(<Get Function Handler Corresponding to Err>, e);
                }
            }
        }

        template<class Err, class F, class... Args>
        constexpr void call_with_errs(bool& called, F&& func, Args&&... args)
        {
            try
            {
                std::invoke(ok_.func, std::forward<F>(func), std::forward<Args>(args)...);
            }
            catch (const Err& e)
            {
                if (!called)
                {
                    called = true;
                    std::invoke(<Get Function Handler Corresponding to Err>, e);
                }
            }
        }
    };

    export
    template<class F1, class F2>
    class patterns<Err<F2>, Ok<F1>> : public patterns<Ok<F1>, Err<F2>>
    {
    public:
        using patterns<Ok<F1>, Err<F2>>::patterns;
    };
}