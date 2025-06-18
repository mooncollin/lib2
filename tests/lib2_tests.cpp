import std;

import lib2;

import lib2.tests.type_traits;
import lib2.tests.compact_optional;
import lib2.tests.strings;
import lib2.tests.io;
import lib2.tests.fmt;
// import lib2.tests.match;

constexpr std::string_view usage() noexcept
{
    return "usage: [-l] [<test_name>]\n"
    "-l: Lists test names\n"
    "<test_name>: Name of test to run\n";
}

int main(const int argc, char* const argv[])
{
    std::string_view test_name;
    bool list {false};

    for (auto i {1}; i < argc; ++i)
    {
        const std::string_view arg {argv[i]};
        if (arg == "-l")
        {
            list = true;
            break;
        }
        else if (test_name.empty())
        {
            test_name = arg;
            break;
        }
    }

    if (!list && test_name.empty())
    {
        lib2::cerr << usage()
                   << "\nMust give a test name\n";
        return 1;
    }

    lib2::test::test_suite tests;
    tests.add_test_suite(lib2::tests::type_traits::get_tests());
    tests.add_test_suite(lib2::tests::compact_optional::get_tests());
    tests.add_test_suite(lib2::tests::strings::get_tests());
    tests.add_test_suite(lib2::tests::io::get_tests());
    tests.add_test_suite(lib2::tests::fmt::get_tests());
    // tests.add_test_suite(lib2::tests::match::get_tests());
    // tests.add_test_suite(lib2::tests::scan::get_tests());

    if (list)
    {
        for (const auto& test : tests)
        {
            lib2::cout << test.name() << '\n';
        }
    }
    else
    {
        bool found {false};
        for (auto& test : tests)
        {
            if (test.name() == test_name)
            {
                const auto result {test.run()};
                if (result.failed())
                {
                    lib2::cerr << test.name() << ":\n";
                    result.on(lib2::overloaded{
                        [&](const lib2::test::assert_exception& e) {
                            lib2::cerr << "  " << e << '\n';
                        },
                        [&](const lib2::test::error_exception& e) {
                            lib2::cerr  << "  " << e << '\n';
                        },
                        [&](const std::exception& e) {
                            lib2::cerr << "  Uncaught exception (" << typeid(e).name() << "): " << e << '\n';
                        }
                    });
                    return 1;
                }
                found = true;
                break;
            }
        }

        if (!found)
        {
            lib2::cerr << "No test exists by the name: " << test_name << '\n';
            return 1;
        }
    }
}