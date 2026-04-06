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
                    return static_cast<T>(std::max(static_cast<std::intmax_t>(std::numeric_limits<T>::min()), static_cast<std::intmax_t>(-10'000)));
                }
                else {
                    return 0;
                }
            }()
        };
        static constexpr T max_num {static_cast<T>(std::min(static_cast<std::uintmax_t>(std::numeric_limits<T>::max()), static_cast<std::uintmax_t>(10'000)))};
    public:
        using lib2::test::test_case::test_case;

        void operator()() final
        {
            for (T v {min_num}; v < max_num; ++v)
            {
                const auto expected {std::to_string(v)};

                const auto str {lib2::format<"{}">(v)};
                lib2::test::assert_equal(str, expected);
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
            for (int v {-10000}; v <= 10000; ++v)
            {
                std::string str {lib2::format<"{:b}">(v)};
                lib2::test::assert_equal(str, std::format("{:b}", v));

                str = lib2::format<"{:B}">(v);
                lib2::test::assert_equal(str, std::format("{:b}", v));

                str = lib2::format<"{:#b}">(v);
                lib2::test::assert_equal(str, std::format("{:#b}", v));

                str = lib2::format<"{:#B}">(v);
                lib2::test::assert_equal(str, std::format("{:#B}", v));
                
                str = lib2::format<"{:o}">(v);
                lib2::test::assert_equal(str, std::format("{:o}", v));

                str = lib2::format<"{:#o}">(v);
                lib2::test::assert_equal(str, std::format("{:#o}", v));

                str = lib2::format<"{:x}">(v);
                lib2::test::assert_equal(str, std::format("{:x}", v));

                str = lib2::format<"{:X}">(v);
                lib2::test::assert_equal(str, std::format("{:X}", v));

                str = lib2::format<"{:#x}">(v);
                lib2::test::assert_equal(str, std::format("{:#x}", v));

                str = lib2::format<"{:#X}">(v);
                lib2::test::assert_equal(str, std::format("{:#X}", v));
            }
        }
    };
}