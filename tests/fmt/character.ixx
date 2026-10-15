export module lib2.tests.fmt:character;

import std;

import lib2;

namespace lib2::tests::fmt
{
    export
    void character_fmt()
    {
        std::string str {lib2::format<"{}">('I')};
        lib2::test::assert_equal(str, "I");

        str = lib2::format<"{:4}">('I');
        lib2::test::assert_equal(str, "I   ");

        str = lib2::format<"{:%^5}">('I');
        lib2::test::assert_equal(str, "%%I%%");
    }
}