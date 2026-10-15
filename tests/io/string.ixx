export module lib2.tests.io:string;

import std;

import lib2;

namespace lib2::tests::io
{
    export
    void string_sink_default_constructor()
    {
        lib2::io::string_sink ss;

        lib2::test::assert_true(ss.str.empty());
    }
    
    export
    void string_sink_constructor()
    {
        const std::string s {"Hello!"};

        lib2::io::string_sink ss4{s};
        lib2::test::assert_equal(ss4.str, s);

        lib2::io::string_sink ss5{std::string{"Hello!"}};
        lib2::test::assert_equal(ss5.str, s);

        lib2::io::string_sink ss6{"Hello!"};
        lib2::test::assert_equal(ss6.str, s);
    }
    
    // export
    // void stringstream_read()
    // {
    //     using traits = lib2::stringstream::traits_type;

    //     lib2::istringstream ss{"Hello!"};

    //     lib2::test::assert_equal(ss.bump(), 'H');
    //     lib2::test::assert_equal(ss.bump(), 'e');
    //     lib2::test::assert_equal(ss.bump(), 'l');
    //     lib2::test::assert_equal(ss.bump(), 'l');
    //     lib2::test::assert_equal(ss.bump(), 'o');
    //     lib2::test::assert_equal(ss.bump(), '!');
    //     lib2::test::assert_false(ss.bump());

    //     ss.str("Hello!");
    //     char buf[7] {};

    //     auto amount = ss.read(buf, 2);
    //     lib2::test::assert_equal(amount, 2);
    //     lib2::test::assert_cstrings_equal(buf, "He");

    //     amount = ss.read(buf + amount, 10);
    //     lib2::test::assert_equal(amount, 4);
    //     lib2::test::assert_cstrings_equal(buf, "Hello!");

    //     amount = ss.read(buf, 2);
    //     lib2::test::assert_equal(amount, 0);
    //     lib2::test::assert_cstrings_equal(buf, "Hello!");

    //     lib2::test::assert_false(ss.get());
    // }

    export
    void string_sink_write()
    {
        lib2::io::string_sink ss;

        ss.put('H');
        ss.put('e');
        ss.put('l');
        ss.put('l');
        ss.put('o');
        
        lib2::test::assert_equal(ss.str, "Hello");

        constexpr std::string_view msg {" This is hopefully a long message"};
        ss.write(msg);

        lib2::test::assert_equal(ss.str, "Hello This is hopefully a long message");
    }
}