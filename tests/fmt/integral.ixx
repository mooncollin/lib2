export module lib2.tests.fmt:integral;

import std;
import lib2;

namespace lib2::tests::fmt
{
    export
    template<std::integral T>
    class integral_test : public lib2::test::test_case
    {
        static constexpr T min_num {
            [] {
                if constexpr (std::signed_integral<T>) {
                    return static_cast<T>(std::max(static_cast<std::intmax_t>(std::numeric_limits<T>::min()), static_cast<std::intmax_t>(-1'000'000)));
                }
                else {
                    return 0;
                }
            }()
        };
        static constexpr T max_num {static_cast<T>(std::min(static_cast<std::uintmax_t>(std::numeric_limits<T>::max()), static_cast<std::uintmax_t>(1'000'000)))};
    public:
        using lib2::test::test_case::test_case;

        void operator()() final
        {
            lib2::ostringstream ss;
            
            for (T v {min_num}; v < max_num; ++v)
            {
                const auto expected {std::to_string(v)};
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
    class integral_base_test : public lib2::test::test_case
    {
    public:
        integral_base_test()
            : lib2::test::test_case{"integral_base"} {}

        void operator()() final
        {
            lib2::ostringstream ss;
            
            for (int v {-10000}; v <= 10000; ++v)
            {
                ss.str({});
                lib2::format_to(ss, "{:b}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:b}", v));

                ss.str({});
                lib2::format_to(ss, "{:B}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:b}", v));

                ss.str({});
                lib2::format_to(ss, "{:#b}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:#b}", v));

                ss.str({});
                lib2::format_to(ss, "{:#B}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:#B}", v));
                
                ss.str({});
                lib2::format_to(ss, "{:o}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:o}", v));

                ss.str({});
                lib2::format_to(ss, "{:#o}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:#o}", v));

                ss.str({});
                lib2::format_to(ss, "{:x}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:x}", v));

                ss.str({});
                lib2::format_to(ss, "{:X}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:X}", v));

                ss.str({});
                lib2::format_to(ss, "{:#x}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:#x}", v));

                ss.str({});
                lib2::format_to(ss, "{:#X}", v);
                lib2::test::assert_equal(ss.view(), std::format("{:#X}", v));
            }
        }
    };
}