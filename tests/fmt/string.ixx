export module lib2.tests.fmt:string;

import lib2;

namespace lib2::tests::fmt
{
    export
    class string_fmt_test : public lib2::test::test_case
    {
    public:
        string_fmt_test()
            : lib2::test::test_case{"string_fmt"} {}

        void operator()() final
        {
            const std::string_view base {"Hello, world!"};

            lib2::ostringstream ss;
            lib2::to_stream(ss, base, {});
            lib2::test::assert_equal(ss.view(), base);

            ss.str({});
            lib2::to_stream(ss, base, {.align = lib2::align_type::right, .width=15});
            lib2::test::assert_equal(ss.view(), "  Hello, world!");
        }
    };

    export
    class string_fmt_string_test : public lib2::test::test_case
    {
    public:
        string_fmt_string_test()
            : lib2::test::test_case{"string_fmt_string"} {}

        void operator()() final
        {
            const std::string_view base {"Hello, world!"};

            std::string str {lib2::format("{}", base)};
            lib2::test::assert_equal(str, base);

            str = lib2::format("{:}", base);
            lib2::test::assert_equal(str, base);

            str = lib2::format("{0:}", base);
            lib2::test::assert_equal(str, base);

            str = lib2::format("{:>15}", base);
            lib2::test::assert_equal(str, "  Hello, world!");
        }
    };

    export
    class string_unicode_fmt_test : public lib2::test::test_case
    {
    public:
        string_unicode_fmt_test()
            : lib2::test::test_case{"string_unicode_fmt"} {}

        void operator()() final
        {
            const std::string_view base {"Hello 😀 world!"};

            std::string str {lib2::format("{}", base)};
            lib2::test::assert_equal(str, base);

            str = lib2::format("{:}", base);
            lib2::test::assert_equal(str, base);

            str = lib2::format("{0:}", base);
            lib2::test::assert_equal(str, base);

            str = lib2::format("{:>15}", base);
            lib2::test::assert_equal(str, " Hello 😀 world!");
        }
    };

    export
    class string_debug_fmt_test : public lib2::test::test_case
    {
    public:
        string_debug_fmt_test()
            : lib2::test::test_case{"string_debug_fmt"} {}

        void operator()() final
        {
            const std::string_view base {"Hello 😀 world!"};

            std::string str {lib2::format("{:?}", base)};
            lib2::test::assert_equal(str, "\"Hello \\u1f600 world!\"");
        }
    };
}