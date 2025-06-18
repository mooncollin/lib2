export module lib2.io:stream_traits;

import std;

import :character;

import lib2.compact_optional;

namespace lib2
{
    template<class T, class U, U Sentinel>
    class stream_optional : private compact_optional<U, Sentinel>
    {
        using base = compact_optional<U, Sentinel>;
    public:
        constexpr stream_optional() noexcept = default;
        constexpr stream_optional(std::nullopt_t) noexcept
            : base{std::nullopt} {}

        constexpr stream_optional(const T value) noexcept
            : base{static_cast<U>(value)} {}

        constexpr stream_optional(const stream_optional&) noexcept = default;

        using base::has_value;
        
        constexpr explicit operator bool() const noexcept
        {
            return has_value();
        }

        constexpr T operator*() const noexcept
        {
            return static_cast<T>(base::operator*());
        }

        constexpr bool operator==(const stream_optional&) const noexcept = default;
        constexpr bool operator!=(const stream_optional&) const noexcept = default;

        constexpr bool operator==(const T value) const noexcept
        {
            return static_cast<U>(value) == base::operator*();
        }

        constexpr bool operator!=(const T value) const noexcept
        {
            return !(*this == value);
        }
    };

    export
    template<class T>
    struct stream_traits
    {
        using opt_type = std::optional<T>;
    };

    export
    template<>
    struct stream_traits<std::byte>
    {
        using opt_type = stream_optional<std::byte, int, -1>;
    };

    export
    template<io_character CharT>
    struct stream_traits<CharT>
    {
        using opt_type = stream_optional<CharT, typename std::char_traits<CharT>::int_type, -1>;
    };
}