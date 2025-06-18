export module lib2.tests.strings:lazy_string;

import lib2;

namespace lib2::tests::strings
{
    export
    class lazy_string_default_construct : public lib2::test::test_case
    {
    public:
        lazy_string_default_construct()
            : lib2::test::test_case{"lazy_string_default_construct"} {}
        
        void operator()() final
        {
            lib2::lazy_string s;

            lib2::test::assert_true(s.empty());
        }
    };

    export
    class lazy_string_literal_construct : public lib2::test::test_case
    {
    public:
        lazy_string_literal_construct()
            : lib2::test::test_case{"lazy_string_literal_construct"} {}
        
        void operator()() final
        {
            lib2::lazy_string s {"Hello!"};

            lib2::test::assert_equal(s, "Hello!");
        }
    };

    export
    class lazy_string_sv_construct : public lib2::test::test_case
    {
    public:
        lazy_string_sv_construct()
            : lib2::test::test_case{"lazy_string_sv_construct"} {}
        
        void operator()() final
        {
            constexpr std::string_view sv {"This is hopefully a really really long string!"};
            lib2::lazy_string s {sv};

            lib2::test::assert_equal(s, sv);
        }
    };

    export
    class lazy_string_string_construct : public lib2::test::test_case
    {
    public:
        lazy_string_string_construct()
            : lib2::test::test_case{"lazy_string_string_construct"} {}
        
        void operator()() final
        {
            const std::string sv {"This is hopefully a really really long string!"};
            lib2::lazy_string s {sv};

            lib2::test::assert_equal(s, sv);
        }
    };
}