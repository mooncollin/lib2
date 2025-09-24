export module lib2.tests.fmt:chrono;

import std;
import lib2;

namespace lib2::tests::fmt
{
    export
    class duration_fmt_test : public lib2::test::test_case
    {
    public:
        duration_fmt_test()
            : lib2::test::test_case{"duration_fmt_test"} {}

        void operator()() final
        {
            auto str {lib2::format("{:%Q%q}", std::chrono::seconds{45})};
            lib2::test::assert_equal(str, "45s");

            str = lib2::format("{:%Q%q}", std::chrono::milliseconds{0});
            lib2::test::assert_equal(str, "0ms");

            str = lib2::format("{:%Q%q}", std::chrono::microseconds{1});
            lib2::test::assert_equal(str, "1us");

            str = lib2::format("{:%H:%M:%S (%Q%q)}", std::chrono::seconds{54321});
            lib2::test::assert_equal(str, "15:05:21 (54321s)"); 
        }
    };
}