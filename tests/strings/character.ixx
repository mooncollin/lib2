export module lib2.tests.strings:character;

import std;

import lib2;

namespace lib2::tests::strings
{
    export
    void isalpha_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::isalpha_ascii(c), static_cast<bool>(std::isalpha(static_cast<unsigned char>(c))));
        }
    }

    export
    void isdigit_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::isdigit_ascii(c), static_cast<bool>(std::isdigit(static_cast<unsigned char>(c))));
        }
    }

    export
    void isupper_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::isupper_ascii(c), static_cast<bool>(std::isupper(static_cast<unsigned char>(c))));
        }
    }

    export
    void islower_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::islower_ascii(c), static_cast<bool>(std::islower(static_cast<unsigned char>(c))));
        }
    }

    export
    void isalnum_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::isalnum_ascii(c), static_cast<bool>(std::isalnum(static_cast<unsigned char>(c))));
        }
    }
    
    export
    void isxdigit_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::isxdigit_ascii(c), static_cast<bool>(std::isxdigit(static_cast<unsigned char>(c))), lib2::format<"For char '{:d}'">(c));
        }
    }

    export
    void isspace_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::isspace_ascii(c), static_cast<bool>(std::isspace(static_cast<unsigned char>(c))));
        }
    }

    export
    void isblank_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::isblank_ascii(c), static_cast<bool>(std::isblank(static_cast<unsigned char>(c))));
        }
    }
    
    export
    void iscntrl_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::iscntrl_ascii(c), static_cast<bool>(std::iscntrl(static_cast<unsigned char>(c))));
        }
    }
    
    export
    void isgraph_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::isgraph_ascii(c), static_cast<bool>(std::isgraph(static_cast<unsigned char>(c))));
        }
    }
    
    export
    void isprint_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::isprint_ascii(c), static_cast<bool>(std::isprint(static_cast<unsigned char>(c))));
        }
    }
    
    export
    void ispunct_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::ispunct_ascii(c), static_cast<bool>(std::ispunct(static_cast<unsigned char>(c))));
        }
    }

    export
    void tolower_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::tolower_ascii(c), static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
    }
    
    export
    void toupper_ascii()
    {
        for (char c {0}; c < 127; ++c)
        {
            lib2::test::assert_equal(lib2::toupper_ascii(c), static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
        }
    }
}