module lib2.io;

import std;

namespace lib2
{
    istream::size_type istream::read(std::byte* const buf, const istream::size_type count)
    {
        ospanstream ss {{buf, count}};
        return read(ss, count);
    }

    istream::size_type istream::read(std::byte* const buf, const istream::size_type count, const std::byte delim)
    {
        ospanstream ss {{buf, count}};
        return read(ss, count, delim);
    }

    istream::size_type text_istream::read(char* const buf, const istream::size_type count)
    {
        ospanstream ss {{reinterpret_cast<std::byte*>(buf), count}};
        return read(ss, count);
    }

    istream::size_type text_istream::read(char* const buf, const istream::size_type count, const char delim)
    {
        ospanstream ss {{reinterpret_cast<std::byte*>(buf), count}};
        return read(ss, count, delim);
    }
}