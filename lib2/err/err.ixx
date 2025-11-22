export module lib2.err;

import std;

namespace lib2
{
    export
    class os_error : public std::system_error
    {
    public:
        os_error();
        os_error(const std::string& what);
        os_error(const char* what);
    };
}