export module lib2.strings:utility;

import std;

namespace lib2
{
    export
    template<class T>
    concept string_view_like = 
        requires(const T& t)
    {
        { std::basic_string_view{t} };
    };
}