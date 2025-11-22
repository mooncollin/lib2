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
        lib2::test::test_suite suite{"fmt library tests"};
        suite.add_test_case<character_test>();
        
        suite.add_test_case<string_fmt_test>();
        suite.add_test_case<string_fmt_string_test>();
        suite.add_test_case<string_unicode_fmt_test>();
        suite.add_test_case<string_debug_fmt_test>();
        
        suite.add_test_case<integral_test<std::uint8_t>>("fmt_uint8");
        suite.add_test_case<integral_test<std::int8_t>>("fmt_int8");
        suite.add_test_case<integral_test<std::uint16_t>>("fmt_uint16");
        suite.add_test_case<integral_test<std::int16_t>>("fmt_int16");
        suite.add_test_case<integral_test<std::uint32_t>>("fmt_uint32");
        suite.add_test_case<integral_test<std::int32_t>>("fmt_int32");
        suite.add_test_case<integral_test<std::uint64_t>>("fmt_uint64");
        suite.add_test_case<integral_test<std::int64_t>>("fmt_int64");
        suite.add_test_case<integral_base_test>();

        suite.add_test_case<floating_test<float>>("fmt_float");
        suite.add_test_case<floating_test<double>>("fmt_double");
        suite.add_test_case<floating_test<long double>>("fmt_long_double");
        suite.add_test_case<floating_type_test>();

        suite.add_test_case<duration_fmt_test>();

        return std::move(suite);
    }
}