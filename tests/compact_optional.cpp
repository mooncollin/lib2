module lib2.tests.compact_optional;

import std;
import lib2;

namespace lib2::tests::compact_optional
{
    void default_constructor_test()
    {
        constexpr lib2::compact_optional<int, int{-1}> v1;

        lib2::test::assert_false(v1);
    }

    void constructor_test()
    {
        constexpr lib2::compact_optional<int, int{-1}> v1 {5};

        lib2::test::assert_true(v1);
        lib2::test::assert_equal(v1, 5);
    }

    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite;
        suite.add_test_case("compact_optional::default_constructor", default_constructor_test);
        suite.add_test_case("compact_optional::constructor", constructor_test);
        
        return std::move(suite);
    }
}