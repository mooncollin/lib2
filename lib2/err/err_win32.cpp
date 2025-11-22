module;

#include <windows.h>

module lib2.err;

namespace lib2
{
    os_error::os_error()
        : std::system_error{static_cast<int>(::GetLastError()), std::system_category()} {}

    os_error::os_error(const std::string& msg)
        : std::system_error{static_cast<int>(::GetLastError()), std::system_category(), msg} {}

    os_error::os_error(const char* const msg)
        : std::system_error{static_cast<int>(::GetLastError()), std::system_category(), msg} {}
}