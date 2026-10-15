export module lib2.tests.io:ostream;

import std;
import lib2;

namespace lib2::tests::io
{
    export
    void ostream_char()
    {
        lib2::ostringstream ss;

        ss.put('H');
        ss.put('E');
        ss.put('L');
        ss.put('L');
        ss.put('O');
        ss.put('!');

        lib2::test::assert_equal(ss.view(), "HELLO!");
    }

    export
    void ostream_string_literal()
    {
        lib2::ostringstream ss;

        ss.write("Jello!");

        lib2::test::assert_equal(ss.view(), "Jello!");
    }

    export
    void ostream_string()
    {
        lib2::ostringstream ss;

        ss.write(std::string{"Jello!"});

        lib2::test::assert_equal(ss.view(), "Jello!");
    }

    export
    void ostream_istream()
    {
        lib2::istringstream is{"Howdy!"};
        lib2::ostringstream os;

        lib2::test::assert_equal(is.read(os, 10), 6);
        lib2::test::assert_equal(os.view(), "Howdy!");
    }
}