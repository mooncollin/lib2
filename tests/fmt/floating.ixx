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
            for (T v {min_num}; v < max_num; v += T{0.01})
            {
                const auto expected {std::format("{}", v)};
                const auto str {lib2::format<"{}">(v)};
                lib2::test::assert_equal(str, expected);
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
            std::string str;

            for (double v {-100.0}; v <= 100.0; v += .01)
            {
                str = lib2::format<"{:.2e}">(v);
                lib2::test::assert_equal(str, std::format("{:.2e}", v));

                str = lib2::format<"{:.2a}">(v);
                lib2::test::assert_equal(str, std::format("{:.2a}", v));

                str = lib2::format<"{:.2f}">(v);
                lib2::test::assert_equal(str, std::format("{:.2f}", v));

                str = lib2::format<"{:.2g}">(v);
                lib2::test::assert_equal(str, std::format("{:.2g}", v));
            }
        }
    };
}