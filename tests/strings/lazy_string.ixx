export module lib2.tests.strings:lazy_string;

import std;

import lib2;

namespace lib2::tests::strings
{
    export
    void lazy_string_default_construct()
    {
        lib2::lazy_string s;

        lib2::test::assert_true(s.empty());
    }

    export
    void lazy_string_literal_construct()
    {
        lib2::lazy_string s {"Hello!"};

        lib2::test::assert_equal(s, "Hello!");
    }

    export
    void lazy_string_sv_construct()
    {
        constexpr std::string_view sv {"This is hopefully a really really long string!"};
        lib2::lazy_string s {sv};

        lib2::test::assert_equal(s, sv);
    }

    export
    void lazy_string_string_construct()
    {
        const std::string sv {"This is hopefully a really really long string!"};
        lib2::lazy_string s {sv};

        lib2::test::assert_equal(s, sv);
    }
}