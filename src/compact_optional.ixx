export module lib2.compact_optional;

import std;

namespace lib2
{
    export
    template<class T, auto Sentinel>
    class compact_optional
    {
    public:
        using value_type     = T;
        using iterator       = T*;
        using const_iterator = const T*;
        using sentinel_t     = decltype(Sentinel);

        static_assert(!std::is_reference_v<T>, "T cannot be a reference type");
        static_assert(!std::is_array_v<T>, "T cannot be an array type");
        static_assert(std::equality_comparable_with<T, sentinel_t>, "T must be able to compare against the sentinel");
        static_assert(std::constructible_from<T, sentinel_t>, "T must be able to construct from sentinel");
        static_assert(std::assignable_from<T&, sentinel_t>, "T must be able to assign from sentinel");

        static constexpr auto sentinel {Sentinel};

        constexpr compact_optional() noexcept(std::is_nothrow_constructible_v<T, sentinel_t>)
            : value_{Sentinel} {}

        constexpr compact_optional(std::nullopt_t) noexcept(std::is_nothrow_constructible_v<T, sentinel_t>)
            : value_{Sentinel} {}

        constexpr compact_optional(const compact_optional& other) noexcept(std::is_nothrow_copy_assignable_v<T>)
            : value_{other.value_} {}

        constexpr compact_optional(compact_optional&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
            : value_{std::move(other.value_)} {}

        template<std::convertible_to<T> U>
        explicit(!std::convertible_to<const U&, T>) constexpr compact_optional(const compact_optional<U, Sentinel>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
            : value_{other.value_} {}

        template<std::convertible_to<T> U>
        explicit(!std::convertible_to<U, T>) constexpr compact_optional(compact_optional<U, Sentinel>&& other) noexcept(std::is_nothrow_constructible_v<T, U>)
            : value_{std::move(other.value_)} {}

        template<class... Args>
        constexpr explicit compact_optional(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
            : value_{std::forward<Args>(args)...} {}

        template<std::convertible_to<T> U = std::remove_cv_t<T>>
        explicit(!std::convertible_to<U, T>) constexpr compact_optional(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
            : value_{std::forward<U>(value)} {}

        constexpr compact_optional& operator=(std::nullopt_t) noexcept(std::is_nothrow_assignable_v<T, sentinel_t>)
        {
            value_ = Sentinel;
            return *this;
        }

        constexpr compact_optional& operator=(const compact_optional& other) noexcept(std::is_nothrow_copy_assignable_v<T>)
        {
            value_ = other.value_;
        }

        constexpr compact_optional& operator=(compact_optional&& other) noexcept(std::is_nothrow_move_assignable_v<T>)
        {
            value_ = std::move(other.value_);
            return *this;
        }

        template<std::convertible_to<T> U>
        constexpr compact_optional& operator=(const compact_optional<U, Sentinel>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
        {
            value_ = other.value_;
            return *this;
        }

        template<std::convertible_to<T> U>
        constexpr compact_optional& operator=(compact_optional<U, Sentinel>&& other) noexcept(std::is_nothrow_constructible_v<T, U>)
        {
            value_ = std::move(other.value_);
            return *this;
        }

        template<std::convertible_to<T> U = std::remove_cv_t<T>>
        constexpr compact_optional& operator=(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
        {
            value_ = std::forward<U>(value);
            return *this;
        }

        constexpr bool has_value() const noexcept
        {
            return value_ != Sentinel;
        }

        constexpr explicit operator bool() const noexcept
        {
            return has_value();
        }

        constexpr iterator begin() noexcept
        {
            return std::addressof(value_) + !has_value();
        }

        constexpr const_iterator begin() const noexcept
        {
            return std::addressof(value_) + !has_value();
        }

        constexpr const T* operator->() const noexcept
        {
            return std::addressof(value_);
        }

        constexpr T* operator->() noexcept
        {
            return std::addressof(value_);
        }

        constexpr const T& operator*() const& noexcept
        {
            return value_;
        }

        constexpr T& operator*() & noexcept
        {
            return value_;
        }

        constexpr const T&& operator*() const&& noexcept
        {
            return std::move(value_);
        }

        constexpr T&& operator*() && noexcept
        {
            return std::move(value_);
        }

        constexpr T& value() &
        {
            if (!has_value())
            {
                throw std::bad_optional_access{};
            }

            return value_;
        }

        constexpr const T& value() const&
        {
            if (!has_value())
            {
                throw std::bad_optional_access{};
            }

            return value_;
        }

        constexpr T&& value() &&
        {
            if (!has_value())
            {
                throw std::bad_optional_access{};
            }

            return std::move(value_);
        }

        constexpr const T&& value() const&&
        {
            if (!has_value())
            {
                throw std::bad_optional_access{};
            }

            return std::move(value_);
        }

        template<std::convertible_to<T> U = std::remove_cv_t<T>>
        constexpr T value_or(U&& default_value) const&
        {
            return has_value() ? **this : static_cast<T>(std::forward<U>(default_value));
        }

        template<std::convertible_to<T> U = std::remove_cv_t<T>>
        constexpr T value_or(U&& default_value) &&
        {
            return has_value() ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
        }

        void swap(compact_optional& other) noexcept(std::is_nothrow_swappable_v<T>)
        {
            using std::swap;
            swap(value_, other.value_);
        }

        void reset() noexcept(std::is_nothrow_assignable_v<sentinel_t, T>)
        {
            value_ = Sentinel;
        }

        template<class... Args>
        T& emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_move_assignable_v<T>)
        {
            value_ = T{std::forward<Args>(args)...};
            return value_;
        }

        constexpr std::compare_three_way_result_t<T, T> operator<=>(const compact_optional& other) const
        {
            return value_ <=> other.value_;
        }

        constexpr std::strong_ordering operator<=>(std::nullopt_t) const noexcept
        {
            return has_value() <=> false;
        }

        template<std::three_way_comparable_with<T> U>
        constexpr std::compare_three_way_result_t<T, U> operator<=>(const U& other) const
        {
            return value_ <=> other;
        }

        template<class U>
        constexpr bool operator==(const U& other) const
        {
            return value_ == other;
        }

        template<class U>
        constexpr bool operator!=(const U& other) const
        {
            return value_ != other;
        }

        template<class U>
        constexpr bool operator<(const U& other) const
        {
            return value_ < other;
        }

        template<class U>
        constexpr bool operator>(const U& other) const
        {
            return value_ > other;
        }

        template<class U>
        constexpr bool operator<=(const U& other) const
        {
            return value_ <= other;
        }

        template<class U>
        constexpr bool operator>=(const U& other) const
        {
            return value_ >= other;
        }
    private:
        T value_;
    };

}

export
template<class T, auto Sentinel>
constexpr bool std::ranges::enable_view<lib2::compact_optional<T, Sentinel>> = true;