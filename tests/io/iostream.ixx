export module lib2.tests.io:iostream;

import std;

import lib2.test;
import lib2.io;

namespace lib2::tests::io
{
    class iostream_impl : public lib2::text_iostream
    {
    public:
        iostream_impl()
        {
            setg(getarr, getarr, getarr + sizeof(getarr));
            setp(putarr, putarr + sizeof(putarr) - 1);
        }
        char putarr[6] {};
        char getarr[5] {'H', 'e', 'l', 'l', 'o'};
    };

    export
    class iostream_constructor_test : public lib2::test::test_case
    {
    public:
        iostream_constructor_test()
            : lib2::test::test_case{"iostream_constructor"} {}

        void operator()() final
        {
            iostream_impl ios;
        }
    };

    export
    class iostream_get_test : public lib2::test::test_case
    {
    public:
        iostream_get_test()
            : lib2::test::test_case{"iostream_get"} {}

        void operator()() final
        {
            iostream_impl ios;
            lib2::test::assert_equal(ios.get(), 'H');
        }
    };

    export
    class iostream_bump_test : public lib2::test::test_case
    {
    public:
        iostream_bump_test()
            : lib2::test::test_case{"iostream_bump"} {}

        void operator()() final
        {
            iostream_impl ios;

            lib2::test::assert_equal(ios.bump(), 'H');
            lib2::test::assert_equal(ios.bump(), 'e');
            lib2::test::assert_equal(ios.bump(), 'l');
            lib2::test::assert_equal(ios.bump(), 'l');
            lib2::test::assert_equal(ios.bump(), 'o');

            lib2::test::assert_false(ios.bump());
        }
    };

    export
    class iostream_next_test : public lib2::test::test_case
    {
    public:
        iostream_next_test()
            : lib2::test::test_case{"iostream_next"} {}

        void operator()() final
        {
            iostream_impl ios;

            lib2::test::assert_equal(ios.next(), 'e');
            lib2::test::assert_equal(ios.next(), 'l');
            lib2::test::assert_equal(ios.next(), 'l');
            lib2::test::assert_equal(ios.next(), 'o');

            lib2::test::assert_false(ios.next());
        }
    };

    export
    class iostream_read_test : public lib2::test::test_case
    {
    public:
        iostream_read_test()
            : lib2::test::test_case{"iostream_read"} {}

        void operator()() final
        {
            iostream_impl ios;

            char buf[6] {};
            auto amount {ios.read(buf, 5)};
            
            lib2::test::assert_equal(amount, 5);
            lib2::test::assert_cstrings_equal(buf, "Hello");

            amount = ios.read(buf, 5);
            lib2::test::assert_equal(amount, 0);

            std::fill_n(buf, 6, '\0');
            iostream_impl ios2;

            amount = ios2.read(buf, 4);
            lib2::test::assert_equal(amount, 4);

            amount = ios2.read(buf + 4, 1);
            lib2::test::assert_equal(amount, 1);

            amount = ios2.read(buf, 2);
            lib2::test::assert_equal(amount, 0);

            lib2::test::assert_cstrings_equal(buf, "Hello");
        }
    };

    export
    class iostream_put_test : public lib2::test::test_case
    {
    public:
        iostream_put_test()
            : lib2::test::test_case{"iostream_put"} {}

        void operator()() final
        {
            iostream_impl ios;
            ios.put('A');
            lib2::test::assert_equal(ios.putarr[0], 'A');

            ios.put('B');
            lib2::test::assert_equal(ios.putarr[1], 'B');

            ios.put('C');
            lib2::test::assert_equal(ios.putarr[2], 'C');

            ios.put('D');
            lib2::test::assert_equal(ios.putarr[3], 'D');

            ios.put('E');
            lib2::test::assert_equal(ios.putarr[4], 'E');

            lib2::test::assert_throws<std::exception>([&] {
                ios.put('F');
            });
        }
    };

    export
    class iostream_write_test : public lib2::test::test_case
    {
    public:
        iostream_write_test()
            : lib2::test::test_case{"iostream_write"} {}

        void operator()() final
        {
            iostream_impl ios;
            ios.write("Jello", 5);
            lib2::test::assert_cstrings_equal(ios.putarr, "Jello");

            lib2::test::assert_throws<std::exception>([&] {
                ios.put('F');
            });

            iostream_impl ios2;
            ios2.write("Jel", 3);
            lib2::test::assert_cstrings_equal(ios2.putarr, "Jel");

            ios2.write("lo", 2);
            lib2::test::assert_cstrings_equal(ios2.putarr, "Jello");

            lib2::test::assert_throws<std::exception>([&] {
                ios2.put('F');
            });
        }
    };
}