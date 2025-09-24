export module lib2.match:match_result;

import std;

namespace lib2::match
{
    export
    template<typename T = void>
    struct matched
    {
        T value;
    };

    export
    template<typename T>
    struct matched<T&>
    {
        T& value;
    };

    export
    template<>
    struct matched<void> {};

    export
    template<class T>
    matched(T) -> matched<T>;

    export
    matched() -> matched<void>;

    export
    template<typename T>
    class match_result
    {
    public:
        constexpr match_result() noexcept = default;
        constexpr match_result(std::nullopt_t) noexcept {}
        constexpr match_result(matched<T>&& m) noexcept(std::is_nothrow_move_constructible_v<T>)
            : value_{std::move(m.value)} {}

        constexpr bool matched() const noexcept
        {
            return static_cast<bool>(value_);
        }

        constexpr operator bool() const noexcept
        {
            return matched();
        }

        constexpr T&& value() && noexcept
        {
            return std::move(value_);
        }
    private:
        std::optional<T> value_;
    };

    export
    template<typename T>
    class match_result<T&>
    {
    public:
        constexpr match_result() noexcept
            : value_{nullptr} {}

        constexpr match_result(std::nullopt_t) noexcept
            : value_{nullptr} {}

        constexpr match_result(matched<T>&& m) noexcept
            : value_{std::addressof(m.value)} {}

        constexpr bool matched() const noexcept
        {
            return static_cast<bool>(value_);
        }

        constexpr operator bool() const noexcept
        {
            return matched();
        }

        constexpr T& value() && noexcept
        {
            return *value_;
        }
    private:
        T* value_;
    };

    export
    template<>
    class match_result<void>
    {
    public:
        constexpr match_result() noexcept
            : matched_{false} {}

        constexpr match_result(std::nullopt_t) noexcept
            : matched_{false} {}

        constexpr match_result(matched<void>&& m) noexcept
            : matched_{true} {}

        constexpr bool matched() const noexcept
        {
            return matched_;
        }

        constexpr operator bool() const noexcept
        {
            return matched();
        }
    private:
        bool matched_;
    };
}