export module lib2.scan:scan_error;

import std;

namespace lib2
{
    export
    class scan_error
    {
    public:
        enum code
        {
            end_of_input,
            invalid_scanned_value,
            invalid_literal,
            length_too_short,
            positive_overflow,
            negative_overflow,
            positive_underflow,
            negative_underflow
        };

        constexpr scan_error(enum code c, const char* msg) noexcept
            : code_{c}, msg_{msg} {}

        [[nodiscard]] constexpr enum code code() const noexcept
        {
            return code_;
        }

        [[nodiscard]] constexpr const char* msg() const noexcept
        {
            return msg_;
        }
    private:
        enum code code_;
        const char* msg_;
    };

    export
    class scan_parse_error : public std::logic_error
    {
    public:
        using std::logic_error::logic_error;
    };
}