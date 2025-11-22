module lib2.tests.type_traits;

namespace lib2::tests::type_traits
{
    class is_any_of_test : public lib2::test::test_case
    {
    public:
        is_any_of_test()
            : lib2::test::test_case{"is_any_of_test"} {}

        void operator()() final
        {
            lib2::test::assert_true(lib2::is_any_of_v<int, int>);
            lib2::test::assert_true(lib2::is_any_of_v<int, int, int, int>);
            lib2::test::assert_false(lib2::is_any_of_v<float, int>);
            lib2::test::assert_false(lib2::is_any_of_v<float>);
        }
    };

    class is_unique_test : public lib2::test::test_case
    {
    public:
        is_unique_test()
            : lib2::test::test_case{"is_unique_test"} {}

        void operator()() final
        {
            lib2::test::assert_true(lib2::is_unique_v<>);
            lib2::test::assert_true(lib2::is_unique_v<int>);
            lib2::test::assert_true(lib2::is_unique_v<int, float>);

            lib2::test::assert_false(lib2::is_unique_v<int, int>);
        }
    };

    class is_specialization_test : public lib2::test::test_case
    {
    public:
        is_specialization_test()
            : lib2::test::test_case{"is_specialization_test"} {}

        void operator()() final
        {
            lib2::test::assert_true(lib2::is_specialization_v<std::string, std::basic_string>);
            lib2::test::assert_true(lib2::is_specialization_v<std::wstring, std::basic_string>);

            lib2::test::assert_false(lib2::is_specialization_v<int, std::basic_string>);
            lib2::test::assert_false(lib2::is_specialization_v<std::string_view, std::basic_string>);
        }
    };

    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite{"type_traits library tests"};
        suite.add_test_case<is_any_of_test>();
        suite.add_test_case<is_unique_test>();
        suite.add_test_case<is_specialization_test>();
        
        return std::move(suite);
    }
}