export module lib2.tests.strings:character;

import lib2;

namespace lib2::tests::strings
{
    template<character CharT>
    static const std::ctype<CharT>& classic_ctype {std::use_facet<std::ctype<CharT>>(std::locale::classic())};

    export
    template<character CharT>
    class is_ascii_alpha final : public lib2::test::test_case
    {
    public:
        is_ascii_alpha()
            : lib2::test::test_case{lib2::format("is_ascii_alpha_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::alpha, c)};
                lib2::test::assert_equal(lib2::is_ascii_alpha(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class is_ascii_digit final : public lib2::test::test_case
    {
    public:
        is_ascii_digit()
            : lib2::test::test_case{lib2::format("is_ascii_digit_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::digit, c)};
                lib2::test::assert_equal(lib2::is_ascii_digit(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class is_ascii_upper final : public lib2::test::test_case
    {
    public:
        is_ascii_upper()
            : lib2::test::test_case{lib2::format("is_ascii_upper_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::upper, c)};
                lib2::test::assert_equal(lib2::is_ascii_upper(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class is_ascii_lower final : public lib2::test::test_case
    {
    public:
        is_ascii_lower()
            : lib2::test::test_case{lib2::format("is_ascii_lower_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::lower, c)};
                lib2::test::assert_equal(lib2::is_ascii_lower(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class is_ascii_alnum final : public lib2::test::test_case
    {
    public:
        is_ascii_alnum()
            : lib2::test::test_case{lib2::format("is_ascii_alnum_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::alnum, c)};
                lib2::test::assert_equal(lib2::is_ascii_alnum(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class is_ascii_xdigit final : public lib2::test::test_case
    {
    public:
        is_ascii_xdigit()
            : lib2::test::test_case{lib2::format("is_ascii_xdigit_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::xdigit, c)};
                lib2::test::assert_equal(lib2::is_ascii_xdigit(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class is_ascii_space final : public lib2::test::test_case
    {
    public:
        is_ascii_space()
            : lib2::test::test_case{lib2::format("is_ascii_space_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::space, c)};
                lib2::test::assert_equal(lib2::is_ascii_space(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class is_ascii_blank final : public lib2::test::test_case
    {
    public:
        is_ascii_blank()
            : lib2::test::test_case{lib2::format("is_ascii_blank_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::blank, c)};
                lib2::test::assert_equal(lib2::is_ascii_blank(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class is_ascii_cntrl final : public lib2::test::test_case
    {
    public:
        is_ascii_cntrl()
            : lib2::test::test_case{lib2::format("is_ascii_cntrl_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::cntrl, c)};
                lib2::test::assert_equal(lib2::is_ascii_cntrl(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class is_ascii_graph final : public lib2::test::test_case
    {
    public:
        is_ascii_graph()
            : lib2::test::test_case{lib2::format("is_ascii_graph_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::graph, c)};
                lib2::test::assert_equal(lib2::is_ascii_graph(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class is_ascii_print final : public lib2::test::test_case
    {
    public:
        is_ascii_print()
            : lib2::test::test_case{lib2::format("is_ascii_print_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::print, c)};
                lib2::test::assert_equal(lib2::is_ascii_print(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class is_ascii_punct final : public lib2::test::test_case
    {
    public:
        is_ascii_punct()
            : lib2::test::test_case{lib2::format("is_ascii_punct_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const bool expected {classic_ctype<CharT>.is(std::ctype_base::punct, c)};
                lib2::test::assert_equal(lib2::is_ascii_punct(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class ascii_to_lower final : public lib2::test::test_case
    {
    public:
        ascii_to_lower()
            : lib2::test::test_case{lib2::format("ascii_to_lower_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const auto expected {classic_ctype<CharT>.toupper(c)};
                lib2::test::assert_equal(lib2::ascii_to_lower(c), expected);
            }
        }
    };

    export
    template<character CharT>
    class ascii_to_upper final : public lib2::test::test_case
    {
    public:
        ascii_to_upper()
            : lib2::test::test_case{lib2::format("ascii_to_upper_{}", typeid(CharT).name())} {}
        
        void operator()()
        {
            for (CharT c {0}; c < 127; ++c)
            {
                const auto expected {classic_ctype<CharT>.toupper(c)};
                lib2::test::assert_equal(lib2::ascii_to_upper(c), expected);
            }
        }
    };
}