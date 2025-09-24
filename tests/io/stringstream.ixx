export module lib2.tests.io:stringstream;

import std;

import lib2.test;
import lib2.io;

namespace lib2::tests::io
{
    export
    class stringstream_default_constructor_test : public lib2::test::test_case
    {
    public:
        stringstream_default_constructor_test()
            : lib2::test::test_case{"stringstream_default_constructor"} {}

        void operator()() final
        {
            lib2::stringstream ss;

            lib2::test::assert_true(ss.view().empty());
        }
    };

    export
    class stringstream_constructor_test : public lib2::test::test_case
    {
    public:
        stringstream_constructor_test()
            : lib2::test::test_case{"stringstream_constructor"} {}

        void operator()() final
        {
            const std::string s {"Hello!"};

            lib2::istringstream ss{s};
            lib2::test::assert_equal(ss.view(), s);

            lib2::istringstream ss2{std::string{"Hello!"}};
            lib2::test::assert_equal(ss2.view(), s);

            lib2::istringstream ss3{"Hello!"};
            lib2::test::assert_equal(ss3.view(), s);

            lib2::ostringstream ss4{s};
            lib2::test::assert_equal(ss4.view(), s);

            lib2::ostringstream ss5{std::string{"Hello!"}};
            lib2::test::assert_equal(ss5.view(), s);

            lib2::ostringstream ss6{"Hello!"};
            lib2::test::assert_equal(ss6.view(), s);
        }
    };

    export
    class stringstream_read_test : public lib2::test::test_case
    {
    public:
        stringstream_read_test()
            : lib2::test::test_case{"stringstream_read_test"} {}

        void operator()() final
        {
            using traits = lib2::stringstream::traits_type;

            lib2::istringstream ss{"Hello!"};

            lib2::test::assert_equal(ss.bump(), 'H');
            lib2::test::assert_equal(ss.bump(), 'e');
            lib2::test::assert_equal(ss.bump(), 'l');
            lib2::test::assert_equal(ss.bump(), 'l');
            lib2::test::assert_equal(ss.bump(), 'o');
            lib2::test::assert_equal(ss.bump(), '!');
            lib2::test::assert_false(ss.bump());

            ss.str("Hello!");
            char buf[7] {};

            auto amount = ss.read(buf, 2);
            lib2::test::assert_equal(amount, 2);
            lib2::test::assert_cstrings_equal(buf, "He");

            amount = ss.read(buf + amount, 10);
            lib2::test::assert_equal(amount, 4);
            lib2::test::assert_cstrings_equal(buf, "Hello!");

            amount = ss.read(buf, 2);
            lib2::test::assert_equal(amount, 0);
            lib2::test::assert_cstrings_equal(buf, "Hello!");

            lib2::test::assert_false(ss.get());
        }
    };

    export
    class stringstream_write_test : public lib2::test::test_case
    {
    public:
        stringstream_write_test()
            : lib2::test::test_case{"stringstream_write_test"} {}

        void operator()() final
        {
            using traits = lib2::stringstream::traits_type;

            lib2::ostringstream ss;

            ss.put('H');
            ss.put('e');
            ss.put('l');
            ss.put('l');
            ss.put('o');
            
            lib2::test::assert_equal(ss.view(), "Hello");

            constexpr std::string_view msg{" This is hopefully a long message"};
            ss.write(msg.data(), msg.size());

            lib2::test::assert_equal(ss.view(), "Hello This is hopefully a long message");
        }
    };
}