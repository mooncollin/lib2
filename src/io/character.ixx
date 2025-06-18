export module lib2.io:character;

import std;

namespace lib2
{
    export
    template<class T>
    concept io_character = std::same_as<T, char> || std::same_as<T, wchar_t>;
}