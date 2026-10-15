export module lib2.io:string;

import std;

import :sink;

namespace lib2::io
{
    export
    template<class Allocator = std::allocator<char>>
    struct basic_string_sink
    {
        std::basic_string<char, std::char_traits<char>, Allocator> str;

        inline void put(const char c)
        {
            str.push_back(c);
        }

        inline void write(const char* const buf, const std::size_t n)
        {
            str.append(buf, n);
        }

        inline void write(const std::span<const char> view)
        {
            write(view.data(), view.size());
        }

        inline void write(const char* const cstr)
        {
            str.append(cstr);
        }

        inline void fill(const char c, const std::size_t n)
        {
            str.insert(str.end(), n, c);
        }
    };

    export
    using string_sink = basic_string_sink<>;
    static_assert(sink<string_sink>);
}