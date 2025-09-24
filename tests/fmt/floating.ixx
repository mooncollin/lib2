export module lib2.tests.fmt:floating;

import lib2;

namespace lib2::tests::fmt
{
    export
    template<std::floating_point T>
    class floating_test : public lib2::test::test_case
    {
        static constexpr T min_num {static_cast<T>(-10)};
        static constexpr T max_num {static_cast<T>(10)};
    public:
        using lib2::test::test_case::test_case;

        void operator()() final
        {
            lib2::ostringstream ss;
            
            for (T v {min_num}; v < max_num; v += T{0.01})
            {
                const auto expected {std::format("{}", v)};
                ss.str({});
                ss << v;
                lib2::test::assert_equal(ss.view(), expected);
                ss.str({});
                lib2::format_to(ss, "{}", v);
                lib2::test::assert_equal(ss.view(), expected);
            }
        }
    };

    export
    class floating_type_test : public lib2::test::test_case
    {
    public:
        floating_type_test()
            : lib2::test::test_case{"floating_type"} {}

        void operator()() final
        {
            lib2::ostringstream ss;
            
            for (double v {-100.0}; v <= 100.0; v += .01)
            {
                ss.str({});
                lib2::format_to(ss, "{:.2e}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:.2e}", v));

                ss.str({});
                lib2::format_to(ss, "{:.2a}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:.2a}", v));

                ss.str({});
                lib2::format_to(ss, "{:.2f}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:.2f}", v));

                ss.str({});
                lib2::format_to(ss, "{:.2g}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:.2g}", v));
            }
        }
    };
}