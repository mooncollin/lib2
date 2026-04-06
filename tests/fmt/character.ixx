export module lib2.tests.fmt:character;

import lib2;
import lib2.fmt;

namespace lib2::tests::fmt
{
    export
    class character_fmt : public lib2::test::test_case
    {
    public:
        character_fmt()
            : lib2::test::test_case{"character_fmt"} {}
        
        void operator()() final
        {
            std::string str {lib2::format<"{}">('I')};
            lib2::test::assert_equal(str, "I");

            str = lib2::format<"{:4}">('I');
            lib2::test::assert_equal(str, "I   ");

            str = lib2::format<"{:%^5}">('I');
            lib2::test::assert_equal(str, "%%I%%");
        }
    };
}