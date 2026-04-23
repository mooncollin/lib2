export module lib2.tests.io:istream;

import std;

import lib2;

namespace lib2::tests::io
{
    class my_istream : public lib2::istream
    {
    public:
        my_istream(std::string_view str) noexcept
        {
            this->setg(reinterpret_cast<std::byte*>(const_cast<char*>(str.data())), reinterpret_cast<std::byte*>(const_cast<char*>(str.data())), reinterpret_cast<std::byte*>(const_cast<char*>(str.data() + str.size())));
        }
    };

    class my_unbuffered_istream : public lib2::istream
    {
    public:
        my_unbuffered_istream(std::string_view str) noexcept
            : str{str} {}
    protected:
        lib2::istream::opt_type underflow() override
        {
            if (str.empty())
            {
                return {};
            }

            ch = std::byte(str.front());
            str.remove_prefix(1);
            this->setg(&ch, &ch, &ch + 1);
            return ch;
        }
    private:
        std::string_view str;
        std::byte ch;
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

            lib2::test::assert_equal(*ss.bump(), std::byte{'H'});
            lib2::test::assert_equal(*ss.bump(), std::byte{'e'});
            lib2::test::assert_equal(*ss.bump(), std::byte{'l'});
            lib2::test::assert_equal(*ss.bump(), std::byte{'l'});
            lib2::test::assert_equal(*ss.bump(), std::byte{'o'});
            lib2::test::assert_false(ss.bump());

            my_unbuffered_istream us {"Hello"};

            lib2::test::assert_equal(*us.bump(), std::byte{'H'});
            lib2::test::assert_equal(*us.bump(), std::byte{'e'});
            lib2::test::assert_equal(*us.bump(), std::byte{'l'});
            lib2::test::assert_equal(*us.bump(), std::byte{'l'});
            lib2::test::assert_equal(*us.bump(), std::byte{'o'});
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
            std::byte buf[2];

            lib2::test::assert_equal(ss.read(buf, 2), 2);
            lib2::test::assert_equal(buf[0], std::byte{'H'});
            lib2::test::assert_equal(buf[1], std::byte{'e'});
            lib2::test::assert_equal(ss.read(buf, 2), 2);
            lib2::test::assert_equal(buf[0], std::byte{'l'});
            lib2::test::assert_equal(buf[1], std::byte{'l'});
            lib2::test::assert_equal(ss.read(buf, 2), 1);
            lib2::test::assert_equal(buf[0], std::byte{'o'});
            lib2::test::assert_equal(ss.read(buf, 2), 0);

            my_unbuffered_istream us {"Hello"};

            lib2::test::assert_equal(us.read(buf, 2), 2);
            lib2::test::assert_equal(buf[0], std::byte{'H'});
            lib2::test::assert_equal(buf[1], std::byte{'e'});
            lib2::test::assert_equal(us.read(buf, 2), 2);
            lib2::test::assert_equal(buf[0], std::byte{'l'});
            lib2::test::assert_equal(buf[1], std::byte{'l'});
            lib2::test::assert_equal(us.read(buf, 2), 1);
            lib2::test::assert_equal(buf[0], std::byte{'o'});
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
            std::byte buf[5];

            lib2::test::assert_equal(ss.read(buf, 5, std::byte{'l'}), 2);
            lib2::test::assert_equal(buf[0], std::byte{'H'});
            lib2::test::assert_equal(buf[1], std::byte{'e'});

            my_unbuffered_istream us {"Hello"};

            lib2::test::assert_equal(us.read(buf, 5, std::byte{'l'}), 2);
            lib2::test::assert_equal(buf[0], std::byte{'H'});
            lib2::test::assert_equal(buf[1], std::byte{'e'});
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

            lib2::test::assert_equal(all.ignore(std::numeric_limits<std::size_t>::max(), std::byte{'e'}), 2);

            my_istream some {"Hello"};

            lib2::test::assert_equal(some.ignore(2, std::byte{'e'}), 2);
            lib2::test::assert_equal(some.ignore(2, std::byte{'l'}), 1);
            lib2::test::assert_equal(some.ignore(2, std::byte{'o'}), 2);
            lib2::test::assert_equal(some.ignore(2, std::byte{'o'}), 0);

            my_unbuffered_istream uall {"Hello"};

            lib2::test::assert_equal(uall.ignore(std::numeric_limits<std::size_t>::max(), std::byte{'e'}), 2);

            my_unbuffered_istream usome {"Hello"};

            lib2::test::assert_equal(usome.ignore(2, std::byte{'e'}), 2);
            lib2::test::assert_equal(usome.ignore(2, std::byte{'l'}), 1);
            lib2::test::assert_equal(usome.ignore(2, std::byte{'o'}), 2);
            lib2::test::assert_equal(usome.ignore(2, std::byte{'o'}), 0);
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

            std::ranges::copy(lib2::text_istream{ss}, std::back_inserter(str));
            lib2::test::assert_equal(str, "Hello");

            str.clear();

            my_unbuffered_istream us {"Hello"};
            std::ranges::copy(lib2::text_istream{us}, std::back_inserter(str));
            lib2::test::assert_equal(str, "Hello");
        }
    };
}