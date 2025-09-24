import std;

import lib2;

// Default Construct
class string_default_construct : public lib2::benchmarking::benchmark
{
public:
    string_default_construct()
        : lib2::benchmarking::benchmark{"string"} {}

    void operator()() final
    {
        std::string str;
        lib2::benchmarking::do_not_optimize(str);
    }
};

class string_view_default_construct : public lib2::benchmarking::benchmark
{
public:
    string_view_default_construct()
        : lib2::benchmarking::benchmark{"string_view"} {}

    void operator()() final
    {
        std::string_view str;
        lib2::benchmarking::do_not_optimize(str);
    }
};

class lazy_string_default_construct : public lib2::benchmarking::benchmark
{
public:
    lazy_string_default_construct()
        : lib2::benchmarking::benchmark{"lazy_string"} {}

    void operator()() final
    {
        lib2::lazy_string str;
        lib2::benchmarking::do_not_optimize(str);
    }
};

// String Literal Construct SSO
class string_literal_construct_sso : public lib2::benchmarking::benchmark
{
public:
    string_literal_construct_sso()
        : lib2::benchmarking::benchmark{"string"} {}

    void operator()() final
    {
        std::string str {"Hello!"};
        lib2::benchmarking::do_not_optimize(str);
    }
};

class string_view_literal_construct_sso : public lib2::benchmarking::benchmark
{
public:
    string_view_literal_construct_sso()
        : lib2::benchmarking::benchmark{"string_view"} {}

    void operator()() final
    {
        std::string_view str {"Hello!"};
        lib2::benchmarking::do_not_optimize(str);
    }
};

class lazy_string_literal_construct_sso : public lib2::benchmarking::benchmark
{
public:
    lazy_string_literal_construct_sso()
        : lib2::benchmarking::benchmark{"lazy_string"} {}

    void operator()() final
    {
        lib2::lazy_string str {"Hello!"};
        lib2::benchmarking::do_not_optimize(str);
    }
};

// String Literal Construct
class string_literal_construct : public lib2::benchmarking::benchmark
{
public:
    string_literal_construct()
        : lib2::benchmarking::benchmark{"string"} {}

    void operator()() final
    {
        std::string str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
        lib2::benchmarking::do_not_optimize(str);
    }
};

class string_view_literal_construct : public lib2::benchmarking::benchmark
{
public:
    string_view_literal_construct()
        : lib2::benchmarking::benchmark{"string_view"} {}

    void operator()() final
    {
        std::string_view str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
        lib2::benchmarking::do_not_optimize(str);
    }
};

class lazy_string_literal_construct : public lib2::benchmarking::benchmark
{
public:
    lazy_string_literal_construct()
        : lib2::benchmarking::benchmark{"lazy_string"} {}

    void operator()() final
    {
        lib2::lazy_string str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
        lib2::benchmarking::do_not_optimize(str);
    }
};

// Copy std::string SSO
class string_copy_constructor_sso : public lib2::benchmarking::benchmark
{
public:
    string_copy_constructor_sso()
        : lib2::benchmarking::benchmark{"string"} {}

    void operator()() final
    {
        std::string str {my_str};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    const std::string my_str {"Hello!"};
};

class lazy_string_copy_string_sso : public lib2::benchmarking::benchmark
{
public:
    lazy_string_copy_string_sso()
        : lib2::benchmarking::benchmark{"lazy_string"} {}

    void operator()() final
    {
        lib2::lazy_string str {my_str};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    const std::string my_str {"Hello!"};
};

// Copy std::string
class string_copy_constructor : public lib2::benchmarking::benchmark
{
public:
    string_copy_constructor()
        : lib2::benchmarking::benchmark{"string"} {}

    void operator()() final
    {
        std::string str {my_str};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    const std::string my_str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
};

class lazy_string_copy_string : public lib2::benchmarking::benchmark
{
public:
    lazy_string_copy_string()
        : lib2::benchmarking::benchmark{"lazy_string"} {}

    void operator()() final
    {
        lib2::lazy_string str {my_str};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    const std::string my_str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
};

// Move std::string SSO
class string_move_constructor_sso : public lib2::benchmarking::benchmark
{
public:
    string_move_constructor_sso()
        : lib2::benchmarking::benchmark{"string"} {}

    void operator()() final
    {
        std::string str {std::move(my_str)};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    std::string my_str {"Hello!"};
};

// Move std::string
class string_move_constructor : public lib2::benchmarking::benchmark
{
public:
    string_move_constructor()
        : lib2::benchmarking::benchmark{"string"} {}

    void operator()() final
    {
        std::string str {std::move(my_str)};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    std::string my_str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
};

// Copy Constructor SSO
class lazy_string_copy_constructor_from_view_sso : public lib2::benchmarking::benchmark
{
public:
    lazy_string_copy_constructor_from_view_sso()
        : lib2::benchmarking::benchmark{"lazy_string_from_view"} {}

    void operator()() final
    {
        lib2::lazy_string str {my_str};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    const lib2::lazy_string my_str {"Hello!"};
};

class lazy_string_copy_constructor_sso : public lib2::benchmarking::benchmark
{
public:
    lazy_string_copy_constructor_sso()
        : lib2::benchmarking::benchmark{"lazy_string"} {}

    void operator()() final
    {
        lib2::lazy_string str {my_str};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    const lib2::lazy_string my_str {std::string{"Hello!"}};
};

// Copy Constructor
class lazy_string_copy_constructor_from_view : public lib2::benchmarking::benchmark
{
public:
    lazy_string_copy_constructor_from_view()
        : lib2::benchmarking::benchmark{"lazy_string_from_view"} {}

    void operator()() final
    {
        lib2::lazy_string str {my_str};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    const lib2::lazy_string my_str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
};

class lazy_string_copy_constructor : public lib2::benchmarking::benchmark
{
public:
    lazy_string_copy_constructor()
        : lib2::benchmarking::benchmark{"lazy_string"} {}

    void operator()() final
    {
        lib2::lazy_string str {my_str};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    const lib2::lazy_string my_str {std::string{"Hello! I hope this string is large enough to put on the heap and not do SSO."}};
};

// Move Construct SSO
class lazy_string_move_constructor_from_view_sso : public lib2::benchmarking::benchmark
{
public:
    lazy_string_move_constructor_from_view_sso()
        : lib2::benchmarking::benchmark{"lazy_string_from_view"} {}

    void operator()() final
    {
        lib2::lazy_string str {std::move(my_str)};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    lib2::lazy_string my_str {"Hello!"};
};

class lazy_string_move_constructor_sso : public lib2::benchmarking::benchmark
{
public:
    lazy_string_move_constructor_sso()
        : lib2::benchmarking::benchmark{"lazy_string"} {}

    void operator()() final
    {
        lib2::lazy_string str {std::move(my_str)};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    lib2::lazy_string my_str {std::string{"Hello!"}};
};

// Move Constructor
class lazy_string_move_constructor_from_view : public lib2::benchmarking::benchmark
{
public:
    lazy_string_move_constructor_from_view()
        : lib2::benchmarking::benchmark{"lazy_string_from_view"} {}

    void operator()() final
    {
        lib2::lazy_string str {std::move(my_str)};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    lib2::lazy_string my_str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
};

class lazy_string_move_constructor : public lib2::benchmarking::benchmark
{
public:
    lazy_string_move_constructor()
        : lib2::benchmarking::benchmark{"lazy_string"} {}

    void operator()() final
    {
        lib2::lazy_string str {std::move(my_str)};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    lib2::lazy_string my_str {std::string{"Hello! I hope this string is large enough to put on the heap and not do SSO."}};
};

// From std::format
class string_format_construct : public lib2::benchmarking::benchmark
{
public:
    string_format_construct()
        : lib2::benchmarking::benchmark{"string"} {}

    void operator()() final
    {
        std::string str {std::format("Hello! This is a {} example on how to utilize my {} new string class in lib{}!", "neat", "cool", 2)};
        lib2::benchmarking::do_not_optimize(str);
    }
};

class lazy_string_format_construct : public lib2::benchmarking::benchmark
{
public:
    lazy_string_format_construct()
        : lib2::benchmarking::benchmark{"lazy_string"} {}

    void operator()() final
    {
        lib2::lazy_string str {std::format("Hello! This is a {} example on how to utilize my {} new string class in lib{}!", "neat", "cool", 2)};
        lib2::benchmarking::do_not_optimize(str);
    }
};

int main()
{
    const lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{5}};

    lib2::benchmarking::benchmark_suite default_constructor;
    default_constructor.add_benchmark<string_default_construct>();
    default_constructor.add_benchmark<string_view_default_construct>();
    default_constructor.add_benchmark<lazy_string_default_construct>();

    lib2::benchmarking::benchmark_suite literal_construct_sso;
    literal_construct_sso.add_benchmark<string_literal_construct_sso>();
    literal_construct_sso.add_benchmark<string_view_literal_construct_sso>();
    literal_construct_sso.add_benchmark<lazy_string_literal_construct_sso>();

    lib2::benchmarking::benchmark_suite literal_construct;
    literal_construct.add_benchmark<string_literal_construct>();
    literal_construct.add_benchmark<string_view_literal_construct>();
    literal_construct.add_benchmark<lazy_string_literal_construct>();

    lib2::benchmarking::benchmark_suite copy_std_string_sso;
    copy_std_string_sso.add_benchmark<string_copy_constructor_sso>();
    copy_std_string_sso.add_benchmark<lazy_string_copy_string_sso>();

    lib2::benchmarking::benchmark_suite copy_std_string;
    copy_std_string.add_benchmark<string_copy_constructor>();
    copy_std_string.add_benchmark<lazy_string_copy_string>();

    lib2::benchmarking::benchmark_suite move_std_string_soo;
    move_std_string_soo.add_benchmark<string_move_constructor_sso>();
    move_std_string_soo.add_benchmark<lazy_string_copy_string_sso>();

    lib2::benchmarking::benchmark_suite move_std_string;
    move_std_string.add_benchmark<string_move_constructor>();
    move_std_string.add_benchmark<lazy_string_copy_string>();

    lib2::benchmarking::benchmark_suite copy_constructor_sso;
    copy_constructor_sso.add_benchmark<string_copy_constructor_sso>();
    copy_constructor_sso.add_benchmark<lazy_string_copy_constructor_from_view_sso>();
    copy_constructor_sso.add_benchmark<lazy_string_copy_constructor_sso>();

    lib2::benchmarking::benchmark_suite copy_constructor;
    copy_constructor.add_benchmark<string_copy_constructor>();
    copy_constructor.add_benchmark<lazy_string_copy_constructor_from_view>();
    copy_constructor.add_benchmark<lazy_string_copy_constructor>();

    lib2::benchmarking::benchmark_suite move_constructor_sso;
    move_constructor_sso.add_benchmark<string_move_constructor_sso>();
    move_constructor_sso.add_benchmark<lazy_string_move_constructor_from_view_sso>();
    move_constructor_sso.add_benchmark<lazy_string_move_constructor_sso>();

    lib2::benchmarking::benchmark_suite move_constructor;
    move_constructor.add_benchmark<string_move_constructor>();
    move_constructor.add_benchmark<lazy_string_move_constructor_from_view>();
    move_constructor.add_benchmark<lazy_string_move_constructor>();

    lib2::benchmarking::benchmark_suite format_construct;
    format_construct.add_benchmark<string_format_construct>();
    format_construct.add_benchmark<lazy_string_format_construct>();

    lib2::cout << "SIZEOF(std::string) => " << sizeof(std::string) << '\n';
    lib2::cout << "SIZEOF(lazy_string) => " << sizeof(lib2::lazy_string) << '\n';

    lib2::cout << "Default Constructor:\n";
    lib2::benchmarking::print_benchmarks(ctx, default_constructor);

    lib2::cout << "\nString Literal Construct SSO:\n";
    lib2::benchmarking::print_benchmarks(ctx, literal_construct_sso);

    lib2::cout << "\nString Literal Construct:\n";
    lib2::benchmarking::print_benchmarks(ctx, literal_construct);

    lib2::cout << "\nCopying std::string SSO:\n";
    lib2::benchmarking::print_benchmarks(ctx, copy_std_string_sso);

    lib2::cout << "\nCopying std::string:\n";
    lib2::benchmarking::print_benchmarks(ctx, copy_std_string);

    lib2::cout << "\nMoving std::string SSO:\n";
    lib2::benchmarking::print_benchmarks(ctx, move_std_string_soo);

    lib2::cout << "\nMoving std::string:\n";
    lib2::benchmarking::print_benchmarks(ctx, move_std_string);

    lib2::cout << "\nCopy Constructor SSO:\n";
    lib2::benchmarking::print_benchmarks(ctx, copy_constructor_sso);

    lib2::cout << "\nCopy Constructor:\n";
    lib2::benchmarking::print_benchmarks(ctx, copy_constructor);

    lib2::cout << "\nMove Constructor SSO:\n";
    lib2::benchmarking::print_benchmarks(ctx, move_constructor_sso);

    lib2::cout << "\nMove Constructor:\n";
    lib2::benchmarking::print_benchmarks(ctx, move_constructor);

    lib2::cout << "\nstd::format call:\n";
    lib2::benchmarking::print_benchmarks(ctx, format_construct);
}