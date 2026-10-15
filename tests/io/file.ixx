export module lib2.tests.io:file;

import std;

import lib2.test;
import lib2.io;

namespace lib2::tests::io
{
    constexpr auto read_test_empty_name {"read_test_empty.txt"};
    constexpr auto read_test_name {"read_test.txt"};
    constexpr std::string_view test_contents {
        "This is a file!\n"
        "It even has multiple lines!\n"
        "Woo hoo!"
    };
    constexpr std::wstring_view wtest_contents {
        L"This is a file!\n"
        L"It even has multiple lines!\n"
        L"Woo hoo!"
    };

    constexpr auto write_test_name {"write_test.txt"};
    constexpr auto wwrite_test_name {L"wwrite_test.txt"};

    export
    void file_sink_default_constructor()
    {
        lib2::io::file_sink file;
    }

    export
    void file_sink_open()
    {
        lib2::io::file_sink file;
        file.open(write_test_name);
    }

    export
    void file_sink_write()
    {
        lib2::io::file_sink file {write_test_name};
        file.write(test_contents);
    }
}