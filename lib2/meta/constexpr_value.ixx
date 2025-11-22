export module lib2.meta:constexpr_value;

import std;

namespace lib2
{
    export
    template<auto V>
    using constexpr_value = std::integral_constant<decltype(V), V>;
}