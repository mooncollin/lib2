export module lib2.io:character;

import std;

import lib2.compact_optional;
import lib2.strings;

namespace lib2
{
    export
    template<character CharT>
    class optional_char : private compact_optional<typename std::char_traits<CharT>::int_type, std::numeric_limits<typename std::char_traits<CharT>::int_type>::max()>
    {
        using base = compact_optional<typename std::char_traits<CharT>::int_type, std::numeric_limits<typename std::char_traits<CharT>::int_type>::max()>;
    public:
        constexpr optional_char() noexcept = default;
        constexpr optional_char(std::nullopt_t) noexcept
            : base{std::nullopt} {}

        constexpr optional_char(const CharT value) noexcept
            : base{static_cast<typename std::char_traits<CharT>::int_type>(value)} {}

        constexpr optional_char(const optional_char&) noexcept = default;

        using base::has_value;

        constexpr explicit operator bool() const noexcept
        {
            return has_value();
        }

        constexpr CharT operator*() const noexcept
        {
            return static_cast<CharT>(base::operator*());
        }

        constexpr bool operator==(const optional_char&) const noexcept = default;
        constexpr bool operator!=(const optional_char&) const noexcept = default;

        constexpr bool operator==(const CharT value) const noexcept
        {
            return static_cast<typename std::char_traits<CharT>::int_type>(value) == base::operator*();
        }

        constexpr bool operator!=(const CharT value) const noexcept
        {
            return !(*this == value);
        }
    };

    export
    class optional_byte : private compact_optional<int, -1>
    {
        using base = compact_optional<int, -1>;
    public:
        constexpr optional_byte() noexcept = default;
        constexpr optional_byte(std::nullopt_t) noexcept
            : base{std::nullopt} {}

        constexpr optional_byte(const std::byte value) noexcept
            : base{static_cast<int>(value)} {}

        constexpr optional_byte(const optional_byte&) noexcept = default;

        using base::has_value;
        
        constexpr explicit operator bool() const noexcept
        {
            return has_value();
        }

        constexpr std::byte operator*() const noexcept
        {
            return static_cast<std::byte>(base::operator*());
        }

        constexpr bool operator==(const optional_byte&) const noexcept = default;
        constexpr bool operator!=(const optional_byte&) const noexcept = default;

        constexpr bool operator==(const std::byte value) const noexcept
        {
            return static_cast<int>(value) == base::operator*();
        }

        constexpr bool operator!=(const std::byte value) const noexcept
        {
            return !(*this == value);
        }
    };
}