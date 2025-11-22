export module lib2.tests.io:istream;

import std;

import lib2.test;
import lib2.io;

namespace lib2::tests::io
{
    class my_istream : public lib2::text_istream
    {
    public:
        my_istream(std::string_view str) noexcept
        {
            this->setg(const_cast<char*>(str.data()), const_cast<char*>(str.data()), const_cast<char*>(str.data() + str.size()));
        }
    };

    class my_unbuffered_istream : public lib2::text_istream
    {
    public:
        my_unbuffered_istream(std::string_view str) noexcept
            : str{str} {}
    protected:
        lib2::text_istream::opt_type underflow() override
        {
            if (str.empty())
            {
                return {};
            }

            ch = str.front();
            str.remove_prefix(1);
            this->setg(&ch, &ch, &ch + 1);
            return ch;
        }
    private:
        std::string_view str;
        char ch;
    };

    export
    class istream_bump_test : public lib2::test::test_case
    {
    public:
        istream_bump_test()
            : lib2::test::test_case{"istream_bump"} {}

        void operator()() final
        {
            my_istream ss {"Hello"};

            lib2::test::assert_equal(*ss.bump(), 'H');
            lib2::test::assert_equal(*ss.bump(), 'e');
            lib2::test::assert_equal(*ss.bump(), 'l');
            lib2::test::assert_equal(*ss.bump(), 'l');
            lib2::test::assert_equal(*ss.bump(), 'o');
            lib2::test::assert_false(ss.bump());

            my_unbuffered_istream us {"Hello"};

            lib2::test::assert_equal(*us.bump(), 'H');
            lib2::test::assert_equal(*us.bump(), 'e');
            lib2::test::assert_equal(*us.bump(), 'l');
            lib2::test::assert_equal(*us.bump(), 'l');
            lib2::test::assert_equal(*us.bump(), 'o');
            lib2::test::assert_false(us.bump());
        }
    };

    export
    class istream_read_test : public lib2::test::test_case
    {
    public:
        istream_read_test()
            : lib2::test::test_case{"istream_read"} {}

        void operator()() final
        {
            my_istream ss {"Hello"};
            char buf[2];

            lib2::test::assert_equal(ss.read(buf, 2), 2);
            lib2::test::assert_equal(buf[0], 'H');
            lib2::test::assert_equal(buf[1], 'e');
            lib2::test::assert_equal(ss.read(buf, 2), 2);
            lib2::test::assert_equal(buf[0], 'l');
            lib2::test::assert_equal(buf[1], 'l');
            lib2::test::assert_equal(ss.read(buf, 2), 1);
            lib2::test::assert_equal(buf[0], 'o');
            lib2::test::assert_equal(ss.read(buf, 2), 0);

            my_unbuffered_istream us {"Hello"};

            lib2::test::assert_equal(us.read(buf, 2), 2);
            lib2::test::assert_equal(buf[0], 'H');
            lib2::test::assert_equal(buf[1], 'e');
            lib2::test::assert_equal(us.read(buf, 2), 2);
            lib2::test::assert_equal(buf[0], 'l');
            lib2::test::assert_equal(buf[1], 'l');
            lib2::test::assert_equal(us.read(buf, 2), 1);
            lib2::test::assert_equal(buf[0], 'o');
            lib2::test::assert_equal(us.read(buf, 2), 0);
        }
    };

    export
    class istream_read_delim_test : public lib2::test::test_case
    {
    public:
        istream_read_delim_test()
            : lib2::test::test_case{"istream_read_delim"} {}

        void operator()() final
        {
            my_istream ss {"Hello"};
            char buf[5];

            lib2::test::assert_equal(ss.read(buf, 5, 'l'), 2);
            lib2::test::assert_equal(buf[0], 'H');
            lib2::test::assert_equal(buf[1], 'e');

            my_unbuffered_istream us {"Hello"};

            lib2::test::assert_equal(us.read(buf, 5, 'l'), 2);
            lib2::test::assert_equal(buf[0], 'H');
            lib2::test::assert_equal(buf[1], 'e');
        }
    };

    export
    class istream_ignore_test : public lib2::test::test_case
    {
    public:
        istream_ignore_test()
            : lib2::test::test_case{"istream_ignore"} {}

        void operator()() final
        {
            my_istream all {"Hello"};

            lib2::test::assert_equal(all.ignore(std::numeric_limits<std::size_t>::max()), 5);

            my_istream some {"Hello"};

            lib2::test::assert_equal(some.ignore(2), 2);
            lib2::test::assert_equal(some.ignore(2), 2);
            lib2::test::assert_equal(some.ignore(2), 1);

            my_unbuffered_istream uall {"Hello"};

            lib2::test::assert_equal(uall.ignore(std::numeric_limits<std::size_t>::max()), 5);

            my_unbuffered_istream usome {"Hello"};

            lib2::test::assert_equal(usome.ignore(2), 2);
            lib2::test::assert_equal(usome.ignore(2), 2);
            lib2::test::assert_equal(usome.ignore(2), 1);
        }
    };

    export
    class istream_ignore_delim_test : public lib2::test::test_case
    {
    public:
        istream_ignore_delim_test()
            : lib2::test::test_case{"istream_ignore_delim"} {}

        void operator()() final
        {
            my_istream all {"Hello"};

            lib2::test::assert_equal(all.ignore(std::numeric_limits<std::size_t>::max(), 'e'), 2);

            my_istream some {"Hello"};

            lib2::test::assert_equal(some.ignore(2, 'e'), 2);
            lib2::test::assert_equal(some.ignore(2, 'l'), 1);
            lib2::test::assert_equal(some.ignore(2, 'o'), 2);
            lib2::test::assert_equal(some.ignore(2, 'o'), 0);

            my_unbuffered_istream uall {"Hello"};

            lib2::test::assert_equal(uall.ignore(std::numeric_limits<std::size_t>::max(), 'e'), 2);

            my_unbuffered_istream usome {"Hello"};

            lib2::test::assert_equal(usome.ignore(2, 'e'), 2);
            lib2::test::assert_equal(usome.ignore(2, 'l'), 1);
            lib2::test::assert_equal(usome.ignore(2, 'o'), 2);
            lib2::test::assert_equal(usome.ignore(2, 'o'), 0);
        }
    };

    export
    class istream_iterator_test : public lib2::test::test_case
    {
    public:
        istream_iterator_test()
            : lib2::test::test_case{"istream_iterator"} {}

        void operator()() final
        {
            std::string str;
            my_istream ss {"Hello"};

            std::ranges::copy(ss, std::back_inserter(str));
            lib2::test::assert_equal(str, "Hello");

            str.clear();

            my_unbuffered_istream us {"Hello"};
            std::ranges::copy(us, std::back_inserter(str));
            lib2::test::assert_equal(str, "Hello");
        }
    };

    export
    class istream_ws_test : public lib2::test::test_case
    {
    public:
        istream_ws_test()
            : lib2::test::test_case{"istream_ws_test"} {}

        void operator()() final
        {
            std::string str;
            my_istream ss {"              Hello"};

            ss >> lib2::ws;
            std::ranges::copy(ss, std::back_inserter(str));
            lib2::test::assert_equal(str, "Hello");

            str.clear();

            my_unbuffered_istream us {"              Hello"};
            us >> lib2::ws;
            std::ranges::copy(us, std::back_inserter(str));
            lib2::test::assert_equal(str, "Hello");
        }
    };
}