export module lib2.tests.strings:character;

import lib2;

namespace lib2::tests::strings
{
    export
    class isalpha_ascii final : public lib2::test::test_case
    {
    public:
        isalpha_ascii()
            : lib2::test::test_case{"isalpha_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::isalpha_ascii(c), static_cast<bool>(std::isalpha(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class isdigit_ascii final : public lib2::test::test_case
    {
    public:
        isdigit_ascii()
            : lib2::test::test_case{"isdigit_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::isdigit_ascii(c), static_cast<bool>(std::isdigit(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class isupper_ascii final : public lib2::test::test_case
    {
    public:
        isupper_ascii()
            : lib2::test::test_case{"isupper_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::isupper_ascii(c), static_cast<bool>(std::isupper(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class islower_ascii final : public lib2::test::test_case
    {
    public:
        islower_ascii()
            : lib2::test::test_case{"islower_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::islower_ascii(c), static_cast<bool>(std::islower(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class isalnum_ascii final : public lib2::test::test_case
    {
    public:
        isalnum_ascii()
            : lib2::test::test_case{"isalnum_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::isalnum_ascii(c), static_cast<bool>(std::isalnum(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class isxdigit_ascii final : public lib2::test::test_case
    {
    public:
        isxdigit_ascii()
            : lib2::test::test_case{"isxdigit_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::isxdigit_ascii(c), static_cast<bool>(std::isxdigit(static_cast<unsigned char>(c))), lib2::format<"For char '{:d}'">(c));
            }
        }
    };

    export
    class isspace_ascii final : public lib2::test::test_case
    {
    public:
        isspace_ascii()
            : lib2::test::test_case{"isspace_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::isspace_ascii(c), static_cast<bool>(std::isspace(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class isblank_ascii final : public lib2::test::test_case
    {
    public:
        isblank_ascii()
            : lib2::test::test_case{"isblank_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::isblank_ascii(c), static_cast<bool>(std::isblank(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class iscntrl_ascii final : public lib2::test::test_case
    {
    public:
        iscntrl_ascii()
            : lib2::test::test_case{"iscntrl_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::iscntrl_ascii(c), static_cast<bool>(std::iscntrl(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class isgraph_ascii final : public lib2::test::test_case
    {
    public:
        isgraph_ascii()
            : lib2::test::test_case{"isgraph_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::isgraph_ascii(c), static_cast<bool>(std::isgraph(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class isprint_ascii final : public lib2::test::test_case
    {
    public:
        isprint_ascii()
            : lib2::test::test_case{"isprint_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::isprint_ascii(c), static_cast<bool>(std::isprint(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class ispunct_ascii final : public lib2::test::test_case
    {
    public:
        ispunct_ascii()
            : lib2::test::test_case{"ispunct_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::ispunct_ascii(c), static_cast<bool>(std::ispunct(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class tolower_ascii final : public lib2::test::test_case
    {
    public:
        tolower_ascii()
            : lib2::test::test_case{"tolower_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::tolower_ascii(c), static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
            }
        }
    };

    export
    class toupper_ascii final : public lib2::test::test_case
    {
    public:
        toupper_ascii()
            : lib2::test::test_case{"toupper_ascii"} {}
        
        void operator()()
        {
            for (char c {0}; c < 127; ++c)
            {
                lib2::test::assert_equal(lib2::toupper_ascii(c), static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
            }
        }
    };
}