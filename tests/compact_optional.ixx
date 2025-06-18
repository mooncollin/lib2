export module lib2.tests.compact_optional;

import lib2;

namespace lib2::tests::compact_optional
{
    export
    class default_constructor_test : public lib2::test::test_case
    {
    public:
        default_constructor_test()
            : lib2::test::test_case{"compact_optional_default_constructor"} {}

        void operator()() final
        {
            constexpr lib2::compact_optional<int, int{-1}> v1;

            lib2::test::assert_false(v1);
        }
    };

    export
    class constructor_test : public lib2::test::test_case
    {
    public:
        constructor_test()
            : lib2::test::test_case{"constructor_test"} {}

        void operator()() final
        {
            constexpr lib2::compact_optional<int, int{-1}> v1 {5};

            lib2::test::assert_true(v1);
            lib2::test::assert_equal(v1, 5);
        }
    };

    export
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite{"compact_optional library tests"};
        suite.add_test_case<default_constructor_test>();
        suite.add_test_case<constructor_test>();
        
        return std::move(suite);
    }
}