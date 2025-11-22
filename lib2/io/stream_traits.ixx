export module lib2.io:stream_traits;

import std;

import :character;

import lib2.compact_optional;

namespace lib2
{
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
        using opt_type = optional_byte;
    };

    export
    template<character CharT>
    struct stream_traits<CharT>
    {
        using opt_type = optional_char<CharT>;
    };
}