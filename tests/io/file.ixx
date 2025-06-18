export module lib2.tests.io:file;

import std;

import lib2.test;
import lib2.io;

namespace lib2::tests::io
{
    class file_test : public lib2::test::test_case
    {
    public:
        using lib2::test::test_case::test_case;
    protected:
        static constexpr auto read_test_empty_name {"read_test_empty.txt"};
        static constexpr auto read_test_name {"read_test.txt"};
        static constexpr std::string_view test_contents {
            "This is a file!\n"
            "It even has multiple lines!\n"
            "Woo hoo!"
        };

        static constexpr auto write_test_name {"write_test.txt"};
    };

    class file_ref_read_base : public file_test
    {
    public:
        using file_test::file_test;

        void setup() final
        {
            #pragma warning(disable : 4996)
            empty_file = std::fopen(read_test_empty_name, "r");
            if (!empty_file)
            {
                lib2::test::error(std::format("{} could not be opened", read_test_empty_name));
            }

            file = std::fopen(read_test_name, "r");
            if (!file)
            {
                lib2::test::error(std::format("{} could not be opened", read_test_name));
            }
        }

        void tear_down() noexcept final
        {
            std::fclose(file);
            std::fclose(empty_file);
        }
    protected:
        std::FILE* empty_file;
        std::FILE* file;
    };

    class file_ref_write_base : public file_test
    {
    public:
        using file_test::file_test;

        void setup() final
        {
            #pragma warning(disable : 4996)
            file = std::fopen(write_test_name, "w");
            if (file == nullptr)
            {
                lib2::test::error(std::format("{} could not be opened", write_test_name));
            }
        }

        void tear_down() noexcept final
        {
            std::fclose(file);
        }
    protected:
        std::FILE* file;
    };

    export
    class file_ref_constructor_test : public file_ref_read_base
    {
    public:
        file_ref_constructor_test()
            : file_ref_read_base{"file_ref_constructor_test"} {}

        void operator()() final
        {
            lib2::file_ref fr {*file};
            lib2::test::assert_false(fr.eof());

            lib2::file_ref fr2 {*empty_file};
            lib2::test::assert_false(fr2.eof());
        }
    };

    export
    class file_ref_getc_test : public file_ref_read_base
    {
    public:
        file_ref_getc_test()
            : file_ref_read_base{"file_ref_getc_test"} {}

        void operator()() final
        {
            char ch;
            lib2::file_ref fr {*file};

            for (const auto test : test_contents)
            {
                lib2::test::assert_true(fr.get(ch));
                lib2::test::assert_equal(ch, test);
            }

            lib2::test::assert_false(fr.get(ch));
            lib2::test::assert_true(fr.eof());

            lib2::file_ref fr2 {*empty_file};

            lib2::test::assert_false(fr2.get(ch));
            lib2::test::assert_true(fr2.eof());
        }
    };

    export
    class file_ref_read_test : public file_ref_read_base
    {
    public:
        file_ref_read_test()
            : file_ref_read_base{"file_ref_read_test"} {}

        void operator()() final
        {
            std::array<char, test_contents.size()> buf;
            
            lib2::file_ref fr {*file};

            lib2::test::assert_equal(fr.read(buf), buf.size());
            lib2::test::assert_equal(std::string_view{buf.data(), buf.size()}, test_contents);
            lib2::test::assert_equal(fr.read(buf), 0);
            lib2::test::assert_true(fr.eof());

            lib2::file_ref fr2 {*empty_file};
            lib2::test::assert_equal(fr2.read(buf), 0);
            lib2::test::assert_true(fr2.eof());
        }
    };

    export
    class file_ref_read_more_test : public file_ref_read_base
    {
    public:
        file_ref_read_more_test()
            : file_ref_read_base{"file_ref_read_more_test"} {}

        void operator()() final
        {
            std::array<char, test_contents.size() + 5> buf;
            
            lib2::file_ref fr {*file};

            lib2::test::assert_equal(fr.read(buf), buf.size() - 5);
            lib2::test::assert_equal(std::string_view{buf.data(), buf.size() - 5}, test_contents);
            lib2::test::assert_true(fr.eof());
        }
    };

    export
    class file_ref_putc_test : public file_ref_write_base
    {
    public:
        file_ref_putc_test()
            : file_ref_write_base{"file_ref_putc_test"} {}

        void operator()() final
        {
            lib2::file_ref fr {*file};

            for (const auto ch : test_contents)
            {
                fr.put(ch);
            }
        }
    };

    export
    class file_ref_write_test : public file_ref_write_base
    {
    public:
        file_ref_write_test()
            : file_ref_write_base{"file_ref_write_test"} {}

        void operator()() final
        {
            lib2::file_ref fr {*file};
            fr.write(test_contents);
        }
    };

    export
    class file_default_constructor_test : public file_test
    {
    public:
        file_default_constructor_test()
            : file_test{"file_default_constructor_test"} {}

        void operator()() final
        {
            lib2::file f;
            lib2::test::assert_false(f.is_open());
        }
    };

    export
    class file_constructor_test : public file_test
    {
    public:
        file_constructor_test()
            : file_test{"file_constructor_test"} {}

        void operator()() final
        {
            #pragma warning(disable : 4996)
            auto fp = std::fopen(read_test_name, "r");

            lib2::file f {std::move(fp)};
            lib2::test::assert_true(f.is_open());
        }
    };

    export
    class file_move_test : public file_test
    {
    public:
        file_move_test()
            : file_test{"file_move_test"} {}

        void operator()() final
        {
            #pragma warning(disable : 4996)
            auto fp = std::fopen(read_test_name, "r");

            lib2::file f {std::move(fp)};

            lib2::file f2 {std::move(f)};
            lib2::test::assert_true(f2.is_open());
            lib2::test::assert_false(f.is_open());

            f = std::move(f2);
            lib2::test::assert_false(f2.is_open());
            lib2::test::assert_true(f.is_open());
        }
    };

    export
    class file_open_test : public file_test
    {
    public:
        file_open_test()
            : file_test{"file_open_test"} {}

        void operator()() final
        {
            lib2::file f;
            f.open(read_test_name, lib2::openmode::in);
            lib2::test::assert_true(f.is_open());
        }
    };

    export
    class file_not_opened_test : public file_test
    {
    public:
        file_not_opened_test()
            : file_test{"file_not_opened_test"} {}

        void operator()() final
        {
            lib2::file f;

            const auto result {f.ref()};
            lib2::test::assert_false(result);
        }
    };
}