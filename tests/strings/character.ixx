export module lib2.tests.strings:character;

import lib2;

namespace lib2::tests::strings
{
    export
    class is_ascii_alpha final : public lib2::test::test_case
    {
    public:
        is_ascii_alpha()
            : lib2::test::test_case{"is_ascii_alpha"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_alpha(c), static_cast<bool>(std::isalpha(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class is_ascii_digit final : public lib2::test::test_case
    {
    public:
        is_ascii_digit()
            : lib2::test::test_case{"is_ascii_digit"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_digit(c), static_cast<bool>(std::isdigit(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class is_ascii_upper final : public lib2::test::test_case
    {
    public:
        is_ascii_upper()
            : lib2::test::test_case{"is_ascii_upper"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_upper(c), static_cast<bool>(std::isupper(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class is_ascii_lower final : public lib2::test::test_case
    {
    public:
        is_ascii_lower()
            : lib2::test::test_case{"is_ascii_lower"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_lower(c), static_cast<bool>(std::islower(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class is_ascii_alnum final : public lib2::test::test_case
    {
    public:
        is_ascii_alnum()
            : lib2::test::test_case{"is_ascii_alnum"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_alnum(c), static_cast<bool>(std::isalnum(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class is_ascii_xdigit final : public lib2::test::test_case
    {
    public:
        is_ascii_xdigit()
            : lib2::test::test_case{"is_ascii_xdigit"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_xdigit(c), static_cast<bool>(std::isxdigit(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class is_ascii_space final : public lib2::test::test_case
    {
    public:
        is_ascii_space()
            : lib2::test::test_case{"is_ascii_space"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_space(c), static_cast<bool>(std::isspace(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class is_ascii_blank final : public lib2::test::test_case
    {
    public:
        is_ascii_blank()
            : lib2::test::test_case{"is_ascii_blank"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_blank(c), static_cast<bool>(std::isblank(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class is_ascii_cntrl final : public lib2::test::test_case
    {
    public:
        is_ascii_cntrl()
            : lib2::test::test_case{"is_ascii_cntrl"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_cntrl(c), static_cast<bool>(std::iscntrl(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class is_ascii_graph final : public lib2::test::test_case
    {
    public:
        is_ascii_graph()
            : lib2::test::test_case{"is_ascii_graph"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_graph(c), static_cast<bool>(std::isgraph(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class is_ascii_print final : public lib2::test::test_case
    {
    public:
        is_ascii_print()
            : lib2::test::test_case{"is_ascii_print"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_print(c), static_cast<bool>(std::isprint(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class is_ascii_punct final : public lib2::test::test_case
    {
    public:
        is_ascii_punct()
            : lib2::test::test_case{"is_ascii_punct"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::is_ascii_punct(c), static_cast<bool>(std::ispunct(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class ascii_to_lower final : public lib2::test::test_case
    {
    public:
        ascii_to_lower()
            : lib2::test::test_case{"ascii_to_lower"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::ascii_to_lower(c), static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class ascii_to_upper final : public lib2::test::test_case
    {
    public:
        ascii_to_upper()
            : lib2::test::test_case{"ascii_to_upper"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::ascii_to_upper(c), static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
            }
        }
    };
}