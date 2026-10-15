export module lib2.scan:utility;

import std;

import lib2.io;

import :scan_error;

namespace lib2
{
    export
    template<class T>
    std::expected<T, scan_error> from_stream(text_istream& is)
    {
        T value;
        const auto result {from_stream(is, value)};
        if (result)
        {
            return value;
        }

        return std::unexpected{result.error()};
    }

    export
    template<class T, class Fmt>
    std::expected<T, scan_error> from_stream(text_istream& is, const Fmt& fmt)
    {
        T value;
        const auto result {from_stream(is, value, fmt)};
        if (result)
        {
            return value;
        }

        return std::unexpected{result.error()};
    }
    
    export
    template<class T>
    using scan_format_of_t = decltype(scan_format_of(std::declval<const T&>()));
}