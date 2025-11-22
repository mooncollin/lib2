export module lib2.tests.meta:value_list;

import lib2;

namespace lib2::tests::meta
{
    export
    class value_list_constructor final : public lib2::test::test_case
    {
    public:
        value_list_constructor()
            : lib2::test::test_case{"value_list_constructor"} {}
        
        void operator()()
        {
            constexpr value_list<> vl1;
            constexpr value_list<1> vl2;
            constexpr value_list<1, 'c', 3.4> vl3;
        }
    };

    export
    class value_list_size final : public lib2::test::test_case
    {
    public:
        value_list_size()
            : lib2::test::test_case{"value_list_size"} {}
        
        void operator()()
        {
            using vl1 = value_list<>;
            using vl2 = value_list<1>;
            using vl3 = value_list<1, 'c', 3.4>;

            lib2::test::assert_equal(vl1::size(), 0);
            lib2::test::assert_equal(vl2::size(), 1);
            lib2::test::assert_equal(vl3::size(), 3);
            
            lib2::test::assert_true(vl1::empty());
        }
    };

    export
    class value_list_at final : public lib2::test::test_case
    {
    public:
        value_list_at()
            : lib2::test::test_case{"value_list_at"} {}
        
        void operator()()
        {
            using vl1 = value_list<1>;
            using vl2 = value_list<1, 'c', 3.4>;

            lib2::test::assert_equal(vl1::at<0>(), 1);

            lib2::test::assert_equal(vl2::at<0>(), 1);
            lib2::test::assert_equal(vl2::at<1>(), 'c');
            lib2::test::assert_equal(vl2::at<2>(), 3.4);

            lib2::test::assert_is_type<vl1::type_at<0>, int>();

            lib2::test::assert_is_type<vl2::type_at<0>, int>();
            lib2::test::assert_is_type<vl2::type_at<1>, char>();
            lib2::test::assert_is_type<vl2::type_at<2>, double>();
        }
    };
}