export module lib2.tests.fmt:character;

import lib2;
import lib2.fmt;

namespace lib2::tests::fmt
{
    export
    class character_test : public lib2::test::test_case
    {
    public:
        character_test()
            : lib2::test::test_case{"character_to_stream"} {}
        
        void operator()() final
        {
            lib2::ostringstream ss;

            format_of_t<std::chrono::seconds> s;

            lib2::format_to(ss, "{}", 'I');
            lib2::test::assert_equal(ss.view(), "I");

            ss.str({});
            lib2::format_to(ss, "{:4}", 'I');
            lib2::test::assert_equal(ss.view(), "I   ");

            ss.str({});
            lib2::format_to(ss, "{:%^5}", 'I');
            lib2::test::assert_equal(ss.view(), "%%I%%");
        }
    };
}