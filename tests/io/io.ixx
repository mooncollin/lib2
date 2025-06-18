export module lib2.tests.io;
export import :file;
export import :iostream;
export import :stringstream;
export import :ostream;
export import :fstream;

import std;

import lib2.test;

namespace lib2::tests::io
{
    export
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite{"io library tests"};
        suite.add_test_case<file_ref_constructor_test>();
        suite.add_test_case<file_ref_getc_test>();
        suite.add_test_case<file_ref_read_test>();
        suite.add_test_case<file_ref_read_more_test>();
        suite.add_test_case<file_ref_putc_test>();
        suite.add_test_case<file_ref_write_test>();

        suite.add_test_case<file_default_constructor_test>();
        suite.add_test_case<file_constructor_test>();
        suite.add_test_case<file_move_test>();
        suite.add_test_case<file_open_test>();
        suite.add_test_case<file_not_opened_test>();

        suite.add_test_case<iostream_constructor_test>();
        suite.add_test_case<iostream_get_test>();
        suite.add_test_case<iostream_bump_test>();
        suite.add_test_case<iostream_next_test>();
        suite.add_test_case<iostream_read_test>();
        suite.add_test_case<iostream_put_test>();
        suite.add_test_case<iostream_write_test>();

        suite.add_test_case<stringstream_default_constructor_test>();
        suite.add_test_case<stringstream_constructor_test>();
        suite.add_test_case<stringstream_read_test>();
        suite.add_test_case<stringstream_write_test>();

        suite.add_test_case<ostream_char_test>();
        suite.add_test_case<ostream_string_literal_test>();
        suite.add_test_case<ostream_string_test>();
        suite.add_test_case<ostream_istream_test>();

        suite.add_test_case<ofstream_default_constructor_test>();
        suite.add_test_case<ofstream_open_test>();
        suite.add_test_case<ofstream_write_test>();
        suite.add_test_case<wofstream_write_test>();

        return std::move(suite);
    }
}