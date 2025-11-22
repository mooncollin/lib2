module lib2.tests.strings;

import std;
import lib2;

import :lazy_string;
import :character;

namespace lib2::tests::strings
{
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite{"string library tests"};

        suite.add_test_case<lazy_string_default_construct>();
        suite.add_test_case<lazy_string_literal_construct>();
        suite.add_test_case<lazy_string_sv_construct>();
        suite.add_test_case<lazy_string_string_construct>();

        suite.add_test_case<is_ascii_alpha>();
        suite.add_test_case<is_ascii_digit>();
        suite.add_test_case<is_ascii_upper>();
        suite.add_test_case<is_ascii_lower>();
        suite.add_test_case<is_ascii_alnum>();
        suite.add_test_case<is_ascii_xdigit>();
        suite.add_test_case<is_ascii_space>();
        suite.add_test_case<is_ascii_blank>();
        suite.add_test_case<is_ascii_cntrl>();
        suite.add_test_case<is_ascii_graph>();
        suite.add_test_case<is_ascii_print>();
        suite.add_test_case<is_ascii_punct>();
        suite.add_test_case<ascii_to_lower>();
        suite.add_test_case<ascii_to_upper>();

        return std::move(suite);
    }
}