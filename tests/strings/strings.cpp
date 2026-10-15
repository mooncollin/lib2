module lib2.tests.strings;

import std;
import lib2;

import :lazy_string;
import :character;

namespace lib2::tests::strings
{
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite;

        suite.add_test_case("lazy_string::default_construct", lazy_string_default_construct);
        suite.add_test_case("lazy_string::literal_construct", lazy_string_literal_construct);
        suite.add_test_case("lazy_string::sv_construct", lazy_string_sv_construct);
        suite.add_test_case("lazy_string::string_construct", lazy_string_string_construct);

        suite.add_test_case("isalpha_ascii", isalpha_ascii);
        suite.add_test_case("isdigit_ascii", isdigit_ascii);
        suite.add_test_case("isupper_ascii", isupper_ascii);
        suite.add_test_case("islower_ascii", islower_ascii);
        suite.add_test_case("isalnum_ascii", isalnum_ascii);
        suite.add_test_case("isxdigit_ascii", isxdigit_ascii);
        suite.add_test_case("isspace_ascii", isspace_ascii);
        suite.add_test_case("isblank_ascii", isblank_ascii);
        suite.add_test_case("iscntrl_ascii", iscntrl_ascii);
        suite.add_test_case("isgraph_ascii", isgraph_ascii);
        suite.add_test_case("isprint_ascii", isprint_ascii);
        suite.add_test_case("ispunct_ascii", ispunct_ascii);
        suite.add_test_case("tolower_ascii", tolower_ascii);
        suite.add_test_case("toupper_ascii", toupper_ascii);

        return std::move(suite);
    }
}