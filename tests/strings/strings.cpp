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

        suite.add_test_case<isalpha_ascii>();
        suite.add_test_case<isdigit_ascii>();
        suite.add_test_case<isupper_ascii>();
        suite.add_test_case<islower_ascii>();
        suite.add_test_case<isalnum_ascii>();
        suite.add_test_case<isxdigit_ascii>();
        suite.add_test_case<isspace_ascii>();
        suite.add_test_case<isblank_ascii>();
        suite.add_test_case<iscntrl_ascii>();
        suite.add_test_case<isgraph_ascii>();
        suite.add_test_case<isprint_ascii>();
        suite.add_test_case<ispunct_ascii>();
        suite.add_test_case<tolower_ascii>();
        suite.add_test_case<toupper_ascii>();

        return std::move(suite);
    }
}