export module lib2.tests.fmt:string;

import std;

import lib2;

namespace lib2::tests::fmt
{
    export
    void string_fmt()
    {
        const std::string_view base {"Hello, world!"};

        const auto compile_str1 {lib2::format<"{}">(base)};
        lib2::test::assert_equal(compile_str1, base);

        const auto compile_str2 {lib2::format<"{:>15}">(base)};
        lib2::test::assert_equal(compile_str2, "  Hello, world!");

        const auto str1 {lib2::format("{}", base)};
        lib2::test::assert_equal(str1, base);

        const auto str2 {lib2::format("{:>15}", base)};
        lib2::test::assert_equal(str2, "  Hello, world!");
    }

    export
    void string_unicode_fmt()
    {
        const std::string_view base {"Hello 😀 world!"};

        std::string str {lib2::format<"{}">(base)};
        lib2::test::assert_equal(str, base);

        str = lib2::format<"{:}">(base);
        lib2::test::assert_equal(str, base);

        str = lib2::format<"{0:}">(base);
        lib2::test::assert_equal(str, base);

        str = lib2::format<"{:>15}">(base);
        lib2::test::assert_equal(str, " Hello 😀 world!");

        str = lib2::format("{}", base);
        lib2::test::assert_equal(str, base);

        str = lib2::format("{:}", base);
        lib2::test::assert_equal(str, base);

        str = lib2::format("{0:}", base);
        lib2::test::assert_equal(str, base);

        str = lib2::format("{:>15}", base);
        lib2::test::assert_equal(str, " Hello 😀 world!");
    }

    export
    void string_debug_fmt()
    {
        const std::string_view base {"Hello 😀 world!"};

        std::string str {lib2::format<"{:?}">(base)};
        lib2::test::assert_equal(str, "\"Hello \\u1f600 world!\"");

        str = lib2::format("{:?}", base);
        lib2::test::assert_equal(str, "\"Hello \\u1f600 world!\"");
    }
}