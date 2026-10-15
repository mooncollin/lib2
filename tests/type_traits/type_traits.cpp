module lib2.tests.type_traits;

import std;

namespace lib2::tests::type_traits
{
    void is_any_of_test()
    {
        lib2::test::assert_true(lib2::is_any_of_v<int, int>);
        lib2::test::assert_true(lib2::is_any_of_v<int, int, int, int>);
        lib2::test::assert_false(lib2::is_any_of_v<float, int>);
        lib2::test::assert_false(lib2::is_any_of_v<float>);
    }
    
    void is_unique_test()
    {
        lib2::test::assert_true(lib2::is_unique_v<>);
        lib2::test::assert_true(lib2::is_unique_v<int>);
        lib2::test::assert_true(lib2::is_unique_v<int, float>);

        lib2::test::assert_false(lib2::is_unique_v<int, int>);
    }

    void is_specialization_test()
    {
        lib2::test::assert_true(lib2::is_specialization_v<std::string, std::basic_string>);
        lib2::test::assert_true(lib2::is_specialization_v<std::wstring, std::basic_string>);

        lib2::test::assert_false(lib2::is_specialization_v<int, std::basic_string>);
        lib2::test::assert_false(lib2::is_specialization_v<std::string_view, std::basic_string>);
    }

    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite;
        suite.add_test_case("is_any_of_test", is_any_of_test);
        suite.add_test_case("is_unique_test", is_unique_test);
        suite.add_test_case("is_specialization_test", is_specialization_test);
        
        return std::move(suite);
    }
}