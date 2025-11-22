module lib2.tests.meta;

import std;
import lib2;

import :value_list;

namespace lib2::tests::meta
{
    lib2::test::test_suite get_tests()
    {
        lib2::test::test_suite suite{"meta library tests"};
        suite.add_test_case<value_list_constructor>();
        suite.add_test_case<value_list_size>();
        suite.add_test_case<value_list_at>();

        return std::move(suite);
    }
}