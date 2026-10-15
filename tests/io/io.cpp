module lib2.tests.io;

import std;

import lib2;

import :string;
// import :ostream;
// import :istream;
import :file;

namespace lib2::tests::io
{
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite;

        suite.add_test_case("string_sink::default_constructor", string_sink_default_constructor);
        suite.add_test_case("string_sink::constructor", string_sink_constructor);
        suite.add_test_case("string_sink::write", string_sink_write);
        // suite.add_test_case("string_source::read", string_source_read);

        // suite.add_test_case("ostream::char", ostream_char);
        // suite.add_test_case("ostream::string_literal", ostream_string_literal);
        // suite.add_test_case("ostream::string", ostream_string);
        // suite.add_test_case("ostream::istream", ostream_istream);

        // suite.add_test_case("istream::bump", istream_bump);
        // suite.add_test_case("istream::read", istream_read);
        // suite.add_test_case("istream::read_delim", istream_read_delim);
        // suite.add_test_case("istream::ignore", istream_ignore);
        // suite.add_test_case("istream::ignore_delim", istream_ignore_delim);
        // suite.add_test_case("istream::iterator", istream_iterator);

        suite.add_test_case("file_sink::default_constructor", file_sink_default_constructor);
        suite.add_test_case("file_sink::open", file_sink_open);
        suite.add_test_case("file_sink::write", file_sink_write);

        return std::move(suite);
    }
}