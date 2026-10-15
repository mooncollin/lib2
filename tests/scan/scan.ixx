export module lib2.tests.scan;

import lib2.test;
import lib2.scan;

namespace lib2::tests::scan
{
    void no_arg()
    {
        constexpr std::string_view input {"Hello!"};
        const auto result {lib2::scan(input, "Hello!")};

        lib2::test::assert_equal(result, input.end());
    }

    void no_arg_unmatched()
    {
        lib2::test::assert_throws<lib2::scan_pattern_not_matched>([] {
            constexpr std::string_view input {"Hvllo!"};
            const auto result {lib2::scan(input, "Hello!")};
        });
    }

    void no_arg_eof()
    {
        lib2::test::assert_throws<lib2::scan_pattern_not_matched>([] {
            constexpr std::string_view input {"He!"};
            const auto result {lib2::scan(input, "Hello!")};
        });
    }

    void char_test()
    {
        constexpr std::string_view input {"R"};
        char ch;
        const auto result {lib2::scan(input, "{}", ch)};

        lib2::test::assert_equal(result, input.end());
        lib2::test::assert_equal(ch, 'R');
    }

    void char_eof()
    {
        lib2::test::assert_throws<lib2::scan_pattern_not_matched>([] {
            char ch;
            constexpr std::string_view input {""};
            const auto result {lib2::scan(input, "{}", ch)};
        });
    }

    void string()
    {
        constexpr std::string_view input {"   Hello!  "};
        std::string str;
        const auto result {lib2::scan(input, "{}", str)};

        lib2::test::assert_equal(result, input.begin() + 9);
        lib2::test::assert_equal(str, "Hello!");
    }

    void string_width()
    {
        constexpr std::string_view input {"   Hello!"};
        std::string str;
        const auto result {lib2::scan(input, "{:5}", str)};

        lib2::test::assert_equal(result, input.begin() + 8);
        lib2::test::assert_equal(str, "Hello");
    }

    void string_noskipws()
    {
        constexpr std::string_view input {"   Hello!  "};
        std::string str;
        const auto result {lib2::scan(input, "{:5c}", str)};

        lib2::test::assert_equal(result, input.begin() + 5);
        lib2::test::assert_equal(str, "   He");
    }

    void array()
    {
        constexpr std::string_view input {"   Hello!  "};
        char str[5];
        const auto result {lib2::scan(input, "{}", str)};

        lib2::test::assert_equal(result, input.begin() + 7);
        lib2::test::assert_cstrings_equal(str, "Hell");
    }

    void array_width()
    {
        constexpr std::string_view input {"   Hello!"};
        char str[15];
        const auto result {lib2::scan(input, "{:5}", str)};

        lib2::test::assert_equal(result, input.begin() + 8);
        lib2::test::assert_cstrings_equal(str, "Hello");
    }

    void array_noskipws()
    {
        constexpr std::string_view input {"   Hello!  "};
        char str[15];
        const auto result {lib2::scan(input, "{:5c}", str)};

        lib2::test::assert_equal(result, input.begin() + 5);
        lib2::test::assert_cstrings_equal(str, "   He");
    }

    void string_view()
    {
        constexpr std::string_view input {"   Hello!  "};
        std::string_view str;
        const auto result {lib2::scan(input, "{}", str)};

        lib2::test::assert_equal(result, input.begin() + 9);
        lib2::test::assert_equal(str, "Hello!");
    }

    void string_view_width()
    {
        constexpr std::string_view input {"   Hello!"};
        std::string_view str;
        const auto result {lib2::scan(input, "{:5}", str)};

        lib2::test::assert_equal(result, input.begin() + 8);
        lib2::test::assert_equal(str, "Hello");
    }

    void string_view_noskipws()
    {
        constexpr std::string_view input {"   Hello!  "};
        std::string_view str;
        const auto result {lib2::scan(input, "{:5c}", str)};

        lib2::test::assert_equal(result, input.begin() + 5);
        lib2::test::assert_equal(str, "   He");
    }

    export
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite;
        suite.add_test_case("scan::no_arg", no_arg);
        suite.add_test_case("scan::no_arg_unmatched", no_arg_unmatched);
        suite.add_test_case("scan::no_arg_eof", no_arg_eof);
        suite.add_test_case("scan::char", char_test);
        suite.add_test_case("scan::char_eof", char_eof);
        suite.add_test_case("scan::string", string);
        suite.add_test_case("scan::string_width", string_width);
        suite.add_test_case("scan::string_noskipws", string_noskipws);
        suite.add_test_case("scan::array", array);
        suite.add_test_case("scan::array_width", array_width);
        suite.add_test_case("scan::array_noskipws", array_noskipws);
        suite.add_test_case("scan::string_view", string_view);
        suite.add_test_case("scan::string_view_width", string_view_width);
        suite.add_test_case("scan::string_view_noskipws", string_view_noskipws);

        return std::move(suite);
    }
}