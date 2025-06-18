export module lib2.tests.io:fstream;

import std;

import lib2.test;
import lib2.io;

namespace lib2::tests::io
{
    class fstream_test : public lib2::test::test_case
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
        static constexpr std::wstring_view wtest_contents {
            L"This is a file!\n"
            L"It even has multiple lines!\n"
            L"Woo hoo!"
        };

        static constexpr auto write_test_name {"write_test.txt"};
        static constexpr auto wwrite_test_name {L"wwrite_test.txt"};
    };

    export
    class ofstream_default_constructor_test : public fstream_test
    {
    public:
        ofstream_default_constructor_test()
            : fstream_test{"ofstream_default_constructor"} {}

        void operator()() final
        {
            lib2::ofstream file;
        }
    };

    export
    class ofstream_open_test : public fstream_test
    {
    public:
        ofstream_open_test()
            : fstream_test{"ofstream_open"} {}

        void operator()() final
        {
            lib2::ofstream file;
            file.open(write_test_name);
        }
    };

    export
    class ofstream_write_test : public fstream_test
    {
    public:
        ofstream_write_test()
            : fstream_test{"ofstream_write"} {}

        void operator()() final
        {
            lib2::ofstream file {write_test_name};
            file.write(test_contents.data(), test_contents.size());
        }
    };

    export
    class wofstream_write_test : public fstream_test
    {
    public:
        wofstream_write_test()
            : fstream_test{"wofstream_write"} {}

        void operator()() final
        {
            lib2::wofstream file {wwrite_test_name, lib2::openmode::binary};
            file.write(wtest_contents.data(), wtest_contents.size());
        }
    };
}