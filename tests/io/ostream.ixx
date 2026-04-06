export module lib2.tests.io:ostream;

import std;
import lib2;

namespace lib2::tests::io
{
    export
    class ostream_char_test : public lib2::test::test_case
    {
    public:
        ostream_char_test()
            : lib2::test::test_case{"ostream_char"} {}

        void operator()() final
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
    };

    export
    class ostream_string_literal_test : public lib2::test::test_case
    {
    public:
        ostream_string_literal_test()
            : lib2::test::test_case{"ostream_string_literal"} {}

        void operator()() final
        {
            lib2::ostringstream ss;

            ss.write("Jello!");

            lib2::test::assert_equal(ss.view(), "Jello!");
        }
    };

    export
    class ostream_string_test : public lib2::test::test_case
    {
    public:
        ostream_string_test()
            : lib2::test::test_case{"ostream_string"} {}

        void operator()() final
        {
            lib2::ostringstream ss;

            ss.write(std::string{"Jello!"});

            lib2::test::assert_equal(ss.view(), "Jello!");
        }
    };

    // export
    // class ostream_istream_test : public lib2::test::test_case
    // {
    // public:
    //     ostream_istream_test()
    //         : lib2::test::test_case{"ostream_istream"} {}

    //     void operator()() final
    //     {
    //         lib2::istringstream is{"Howdy!"};
    //         lib2::ostringstream os;

    //         os << is;

    //         lib2::test::assert_equal(os.view(), "Howdy!");
    //     }
    // };
}