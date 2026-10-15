module lib2.tests.fmt;

import std;
import lib2;

import :character;
import :string;
import :integral;
import :floating;
import :chrono;

namespace lib2::tests::fmt
{
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite;
        suite.add_test_case("character_fmt", character_fmt);
        
        suite.add_test_case("string_fmt", string_fmt);
        suite.add_test_case("string_unicode_fmt", string_unicode_fmt);
        suite.add_test_case("string_debug_fmt", string_debug_fmt);
        
        suite.add_test_case("fmt_uint8", integral_test<std::uint8_t>);
        suite.add_test_case("fmt_int8", integral_test<std::int8_t>);
        suite.add_test_case("fmt_uint16", integral_test<std::uint16_t>);
        suite.add_test_case("fmt_int16", integral_test<std::int16_t>);
        suite.add_test_case("fmt_uint32", integral_test<std::uint32_t>);
        suite.add_test_case("fmt_int32", integral_test<std::int32_t>);
        suite.add_test_case("fmt_uint64", integral_test<std::uint64_t>);
        suite.add_test_case("fmt_int64", integral_test<std::int64_t>);
        suite.add_test_case("integral_base", integral_base_test);

        suite.add_test_case("fmt_float", floating_test<float>);
        suite.add_test_case("fmt_double", floating_test<double>);
        suite.add_test_case("fmt_long_double", floating_test<long double>);
        suite.add_test_case("floating_type", floating_type_test);

        suite.add_test_case("duration_fmt", duration_fmt);

        return std::move(suite);
    }
}