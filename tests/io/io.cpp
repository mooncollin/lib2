module lib2.tests.io;

import std;

import lib2;

import :stringstream;
import :ostream;
import :istream;
import :fstream;

namespace lib2::tests::io
{
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite{"io library tests"};

        suite.add_test_case<stringstream_default_constructor_test>();
        suite.add_test_case<stringstream_constructor_test>();
        suite.add_test_case<stringstream_read_test>();
        suite.add_test_case<stringstream_write_test>();

        suite.add_test_case<ostream_char_test>();
        suite.add_test_case<ostream_string_literal_test>();
        suite.add_test_case<ostream_string_test>();
        suite.add_test_case<ostream_istream_test>();

        suite.add_test_case<istream_bump_test>();
        suite.add_test_case<istream_read_test>();
        suite.add_test_case<istream_read_delim_test>();
        suite.add_test_case<istream_ignore_test>();
        suite.add_test_case<istream_ignore_delim_test>();
        suite.add_test_case<istream_iterator_test>();
        suite.add_test_case<istream_ws_test>();

        suite.add_test_case<ofstream_default_constructor_test>();
        suite.add_test_case<ofstream_open_test>();
        suite.add_test_case<ofstream_write_test>();

        return std::move(suite);
    }
}