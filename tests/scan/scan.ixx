export module lib2.tests.scan;

import lib2.test;
import lib2.scan;

namespace lib2::tests::scan
{
    export
    class scan_no_arg_test : public lib2::test::test_case
    {
    public:
        scan_no_arg_test()
            : lib2::test::test_case{"scan_no_arg_test"} {}

        void operator()() final
        {
            constexpr std::string_view input {"Hello!"};
            const auto result {lib2::scan(input, "Hello!")};

            lib2::test::assert_equal(result, input.end());
        }
    };

    export
    class scan_no_arg_unmatched_test : public lib2::test::test_case
    {
    public:
        scan_no_arg_unmatched_test()
            : lib2::test::test_case{"scan_no_arg_unmatched_test"} {}

        void operator()() final
        {
            lib2::test::assert_throws<lib2::scan_pattern_not_matched>([] {
                constexpr std::string_view input {"Hvllo!"};
                const auto result {lib2::scan(input, "Hello!")};
            });
        }
    };

    export
    class scan_no_arg_eof_test : public lib2::test::test_case
    {
    public:
        scan_no_arg_eof_test()
            : lib2::test::test_case{"scan_no_arg_eof_test"} {}

        void operator()() final
        {
            lib2::test::assert_throws<lib2::scan_pattern_not_matched>([] {
                constexpr std::string_view input {"He!"};
                const auto result {lib2::scan(input, "Hello!")};
            });
        }
    };

    export
    class scan_char_test : public lib2::test::test_case
    {
    public:
        scan_char_test()
            : lib2::test::test_case{"scan_char_test"} {}

        void operator()() final
        {
            constexpr std::string_view input {"R"};
            char ch;
            const auto result {lib2::scan(input, "{}", ch)};

            lib2::test::assert_equal(result, input.end());
            lib2::test::assert_equal(ch, 'R');
        }
    };

    export
    class scan_char_eof_test : public lib2::test::test_case
    {
    public:
        scan_char_eof_test()
            : lib2::test::test_case{"scan_char_eof_test"} {}

        void operator()() final
        {
            lib2::test::assert_throws<lib2::scan_pattern_not_matched>([] {
                char ch;
                constexpr std::string_view input {""};
                const auto result {lib2::scan(input, "{}", ch)};
            });
        }
    };

    export
    class scan_string_test : public lib2::test::test_case
    {
    public:
        scan_string_test()
            : lib2::test::test_case{"scan_string_test"} {}

        void operator()() final
        {
            constexpr std::string_view input {"   Hello!  "};
            std::string str;
            const auto result {lib2::scan(input, "{}", str)};

            lib2::test::assert_equal(result, input.begin() + 9);
            lib2::test::assert_equal(str, "Hello!");
        }
    };

    export
    class scan_string_width_test : public lib2::test::test_case
    {
    public:
        scan_string_width_test()
            : lib2::test::test_case{"scan_string_width_test"} {}

        void operator()() final
        {
            constexpr std::string_view input {"   Hello!"};
            std::string str;
            const auto result {lib2::scan(input, "{:5}", str)};

            lib2::test::assert_equal(result, input.begin() + 8);
            lib2::test::assert_equal(str, "Hello");
        }
    };

    export
    class scan_string_noskipws_test : public lib2::test::test_case
    {
    public:
        scan_string_noskipws_test()
            : lib2::test::test_case{"scan_string_noskipws_test"} {}

        void operator()() final
        {
            constexpr std::string_view input {"   Hello!  "};
            std::string str;
            const auto result {lib2::scan(input, "{:5c}", str)};

            lib2::test::assert_equal(result, input.begin() + 5);
            lib2::test::assert_equal(str, "   He");
        }
    };

    export
    class scan_array_test : public lib2::test::test_case
    {
    public:
        scan_array_test()
            : lib2::test::test_case{"scan_array_test"} {}

        void operator()() final
        {
            constexpr std::string_view input {"   Hello!  "};
            char str[5];
            const auto result {lib2::scan(input, "{}", str)};

            lib2::test::assert_equal(result, input.begin() + 7);
            lib2::test::assert_cstrings_equal(str, "Hell");
        }
    };

    export
    class scan_array_width_test : public lib2::test::test_case
    {
    public:
        scan_array_width_test()
            : lib2::test::test_case{"scan_array_width_test"} {}

        void operator()() final
        {
            constexpr std::string_view input {"   Hello!"};
            char str[15];
            const auto result {lib2::scan(input, "{:5}", str)};

            lib2::test::assert_equal(result, input.begin() + 8);
            lib2::test::assert_cstrings_equal(str, "Hello");
        }
    };

    export
    class scan_array_noskipws_test : public lib2::test::test_case
    {
    public:
        scan_array_noskipws_test()
            : lib2::test::test_case{"scan_array_noskipws_test"} {}

        void operator()() final
        {
            constexpr std::string_view input {"   Hello!  "};
            char str[15];
            const auto result {lib2::scan(input, "{:5c}", str)};

            lib2::test::assert_equal(result, input.begin() + 5);
            lib2::test::assert_cstrings_equal(str, "   He");
        }
    };

    export
    class scan_string_view_test : public lib2::test::test_case
    {
    public:
        scan_string_view_test()
            : lib2::test::test_case{"scan_string_view_test"} {}

        void operator()() final
        {
            constexpr std::string_view input {"   Hello!  "};
            std::string_view str;
            const auto result {lib2::scan(input, "{}", str)};

            lib2::test::assert_equal(result, input.begin() + 9);
            lib2::test::assert_equal(str, "Hello!");
        }
    };

    export
    class scan_string_view_width_test : public lib2::test::test_case
    {
    public:
        scan_string_view_width_test()
            : lib2::test::test_case{"scan_string_view_width_test"} {}

        void operator()() final
        {
            constexpr std::string_view input {"   Hello!"};
            std::string_view str;
            const auto result {lib2::scan(input, "{:5}", str)};

            lib2::test::assert_equal(result, input.begin() + 8);
            lib2::test::assert_equal(str, "Hello");
        }
    };

    export
    class scan_string_view_noskipws_test : public lib2::test::test_case
    {
    public:
        scan_string_view_noskipws_test()
            : lib2::test::test_case{"scan_string_view_noskipws_test"} {}

        void operator()() final
        {
            constexpr std::string_view input {"   Hello!  "};
            std::string_view str;
            const auto result {lib2::scan(input, "{:5c}", str)};

            lib2::test::assert_equal(result, input.begin() + 5);
            lib2::test::assert_equal(str, "   He");
        }
    };

    export
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite{"scan library tests"};
        suite.add_test_case<scan_no_arg_test>();
        suite.add_test_case<scan_no_arg_unmatched_test>();
        suite.add_test_case<scan_no_arg_eof_test>();
        suite.add_test_case<scan_char_test>();
        suite.add_test_case<scan_char_eof_test>();
        suite.add_test_case<scan_string_test>();
        suite.add_test_case<scan_string_width_test>();
        suite.add_test_case<scan_string_noskipws_test>();
        suite.add_test_case<scan_array_test>();
        suite.add_test_case<scan_array_width_test>();
        suite.add_test_case<scan_array_noskipws_test>();
        suite.add_test_case<scan_string_view_test>();
        suite.add_test_case<scan_string_view_width_test>();
        suite.add_test_case<scan_string_view_noskipws_test>();

        return std::move(suite);
    }
}