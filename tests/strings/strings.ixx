export module lib2.tests.strings;

import lib2;

import :widen;
import :lazy_string;
import :character;

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

        suite.add_test_case<is_ascii_alpha<char>>();
        suite.add_test_case<is_ascii_alpha<wchar_t>>();
        suite.add_test_case<is_ascii_alpha<char8_t>>();
        suite.add_test_case<is_ascii_alpha<char16_t>>();
        suite.add_test_case<is_ascii_alpha<char32_t>>();

        suite.add_test_case<is_ascii_digit<char>>();
        suite.add_test_case<is_ascii_digit<wchar_t>>();
        suite.add_test_case<is_ascii_digit<char8_t>>();
        suite.add_test_case<is_ascii_digit<char16_t>>();
        suite.add_test_case<is_ascii_digit<char32_t>>();

        suite.add_test_case<is_ascii_upper<char>>();
        suite.add_test_case<is_ascii_upper<wchar_t>>();
        suite.add_test_case<is_ascii_upper<char8_t>>();
        suite.add_test_case<is_ascii_upper<char16_t>>();
        suite.add_test_case<is_ascii_upper<char32_t>>();

        suite.add_test_case<is_ascii_lower<char>>();
        suite.add_test_case<is_ascii_lower<wchar_t>>();
        suite.add_test_case<is_ascii_lower<char8_t>>();
        suite.add_test_case<is_ascii_lower<char16_t>>();
        suite.add_test_case<is_ascii_lower<char32_t>>();

        suite.add_test_case<is_ascii_alnum<char>>();
        suite.add_test_case<is_ascii_alnum<wchar_t>>();
        suite.add_test_case<is_ascii_alnum<char8_t>>();
        suite.add_test_case<is_ascii_alnum<char16_t>>();
        suite.add_test_case<is_ascii_alnum<char32_t>>();

        suite.add_test_case<is_ascii_xdigit<char>>();
        suite.add_test_case<is_ascii_xdigit<wchar_t>>();
        suite.add_test_case<is_ascii_xdigit<char8_t>>();
        suite.add_test_case<is_ascii_xdigit<char16_t>>();
        suite.add_test_case<is_ascii_xdigit<char32_t>>();

        suite.add_test_case<is_ascii_space<char>>();
        suite.add_test_case<is_ascii_space<wchar_t>>();
        suite.add_test_case<is_ascii_space<char8_t>>();
        suite.add_test_case<is_ascii_space<char16_t>>();
        suite.add_test_case<is_ascii_space<char32_t>>();

        suite.add_test_case<is_ascii_blank<char>>();
        suite.add_test_case<is_ascii_blank<wchar_t>>();
        suite.add_test_case<is_ascii_blank<char8_t>>();
        suite.add_test_case<is_ascii_blank<char16_t>>();
        suite.add_test_case<is_ascii_blank<char32_t>>();

        suite.add_test_case<is_ascii_cntrl<char>>();
        suite.add_test_case<is_ascii_cntrl<wchar_t>>();
        suite.add_test_case<is_ascii_cntrl<char8_t>>();
        suite.add_test_case<is_ascii_cntrl<char16_t>>();
        suite.add_test_case<is_ascii_cntrl<char32_t>>();

        suite.add_test_case<is_ascii_graph<char>>();
        suite.add_test_case<is_ascii_graph<wchar_t>>();
        suite.add_test_case<is_ascii_graph<char8_t>>();
        suite.add_test_case<is_ascii_graph<char16_t>>();
        suite.add_test_case<is_ascii_graph<char32_t>>();

        suite.add_test_case<is_ascii_print<char>>();
        suite.add_test_case<is_ascii_print<wchar_t>>();
        suite.add_test_case<is_ascii_print<char8_t>>();
        suite.add_test_case<is_ascii_print<char16_t>>();
        suite.add_test_case<is_ascii_print<char32_t>>();

        suite.add_test_case<is_ascii_punct<char>>();
        suite.add_test_case<is_ascii_punct<wchar_t>>();
        suite.add_test_case<is_ascii_punct<char8_t>>();
        suite.add_test_case<is_ascii_punct<char16_t>>();
        suite.add_test_case<is_ascii_punct<char32_t>>();

        suite.add_test_case<ascii_to_lower<char>>();
        suite.add_test_case<ascii_to_lower<wchar_t>>();
        suite.add_test_case<ascii_to_lower<char8_t>>();
        suite.add_test_case<ascii_to_lower<char16_t>>();
        suite.add_test_case<ascii_to_lower<char32_t>>();

        suite.add_test_case<ascii_to_upper<char>>();
        suite.add_test_case<ascii_to_upper<wchar_t>>();
        suite.add_test_case<ascii_to_upper<char8_t>>();
        suite.add_test_case<ascii_to_upper<char16_t>>();
        suite.add_test_case<ascii_to_upper<char32_t>>();

        return std::move(suite);
    }
}