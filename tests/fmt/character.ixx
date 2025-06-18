export module lib2.tests.fmt:character;

import lib2;

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

    export
    class wide_character_test : public lib2::test::test_case
    {
    public:
        wide_character_test()
            : lib2::test::test_case{"wide_character_to_stream"} {}
        
        void operator()() final
        {
            lib2::wostringstream ss;

            lib2::format_to(ss, L"{}", L'I');
            lib2::test::assert_equal(ss.view(), L"I");

            ss.str({});
            lib2::format_to(ss, L"{:4}", L'I');
            lib2::test::assert_equal(ss.view(), L"I   ");

            ss.str({});
            lib2::format_to(ss, L"{:%^5}", L'I');
            lib2::test::assert_equal(ss.view(), L"%%I%%");
        }
    };
}