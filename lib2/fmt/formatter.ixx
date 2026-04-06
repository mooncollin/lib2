export module lib2.fmt:formatter;

import std;

namespace lib2
{
    export
    template<class T>
    struct formatter;

    export
    class format_error : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}