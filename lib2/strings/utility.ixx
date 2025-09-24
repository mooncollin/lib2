export module lib2.strings:utility;

import std;

namespace lib2
{
    export
    template<class T>
    concept string_view_like = 
        std::convertible_to<T, std::string_view> ||
        std::convertible_to<T, std::u8string_view> ||
        std::convertible_to<T, std::u16string_view> ||
        std::convertible_to<T, std::u32string_view> ||
        std::convertible_to<T, std::wstring_view>;
}