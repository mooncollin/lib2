export module lib2.tests.match;

import std;

import lib2.test;
import lib2.match;

namespace lib2::tests::match
{
    export
    class patterns_construct_test : public lib2::test::test_case
    {
    public:
        patterns_construct_test()
            : lib2::test::test_case{"patterns_construct"} {}

        void operator()() final
        {
            constexpr auto pats {lib2::match::patterns(
                lib2::match::Ok([]{})
            ,   lib2::match::Err([]{})
            )};
        }
    };

    export
    class patterns_expected_test : public lib2::test::test_case
    {
    public:
        patterns_expected_test()
            : lib2::test::test_case{"patterns_expected"} {}

        void operator()() final
        {
            constexpr std::expected<int, int> v {5};
            constexpr std::expected<int, int> err {std::unexpected{-1}};
            constexpr auto r1 {lib2::match::patterns(
                lib2::match::Ok([] (const int v) {
                    return v;
                })
            ,   lib2::match::Err([] (const int err) {
                    return 10;
                })
            )(v)};

            lib2::test::assert_equal(r1, 5);

            constexpr auto r2 {lib2::match::patterns(
                lib2::match::Ok([] (const int v) {
                    return v;
                })
            ,   lib2::match::Err([] (const int err) {
                    return 10;
                })
            )(err)};

            lib2::test::assert_equal(r2, 10);
        }
    };

    export
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite{"match library tests"};

        suite.add_test_case<patterns_construct_test>();
        suite.add_test_case<patterns_expected_test>();

        return std::move(suite);
    }
}