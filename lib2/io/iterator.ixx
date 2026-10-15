export module lib2.io:iterator;

import std;

import :sink;
import :source;

namespace lib2::io
{
    export
    class sink_iterator
    {
    public:
        sink_iterator(dyn_sink s) noexcept
            : s{s} {}

        sink_iterator& operator=(const char ch)
        {
            s.put(ch);
            return *this;
        }

        sink_iterator& operator++() noexcept
        {
            return *this;
        }

        sink_iterator operator++(int) noexcept
        {
            auto tmp {*this};
            ++*this;
            return tmp;
        }

        sink_iterator& operator*() noexcept
        {
            return *this;
        }
    private:
        dyn_sink s;
    };
}

namespace std
{
    export
    template<lib2::io::peekable_source Source>
    constexpr bool empty(Source&& src)
    {
        return src.peek().empty();
    }
}