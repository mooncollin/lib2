export module lib2.tests.strings;

import lib2;

import :widen;
import :lazy_string;

namespace lib2::tests::strings
{
    export
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite{"string library tests"};
        suite.add_test_case<widen_string_test>();

        suite.add_test_case<lazy_string_default_construct>();
        suite.add_test_case<lazy_string_literal_construct>();
        suite.add_test_case<lazy_string_sv_construct>();
        suite.add_test_case<lazy_string_string_construct>();

        return std::move(suite);
    }
}