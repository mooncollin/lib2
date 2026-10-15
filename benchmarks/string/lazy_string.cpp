import std;

import lib2;

// Default Construct
void string_default_construct(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::string str;
        lib2::benchmarking::do_not_optimize(str);
    }
}

void string_view_default_construct(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::string_view str;
        lib2::benchmarking::do_not_optimize(str);
    }
}

void lazy_string_default_construct(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        lib2::lazy_string str;
        lib2::benchmarking::do_not_optimize(str);
    }
}

// String Literal Construct SSO
void string_literal_construct_sso(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::string str {"Hello!"};
        lib2::benchmarking::do_not_optimize(str);
    }
}

void string_view_literal_construct_sso(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::string_view str {"Hello!"};
        lib2::benchmarking::do_not_optimize(str);
    }
}

void lazy_string_literal_construct_sso(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        lib2::lazy_string str {"Hello!"};
        lib2::benchmarking::do_not_optimize(str);
    }
}

// String Literal Construct
void string_literal_construct(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::string str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
        lib2::benchmarking::do_not_optimize(str);
    }
}

void string_view_literal_construct(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::string_view str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
        lib2::benchmarking::do_not_optimize(str);
    }
}

void lazy_string_literal_construct(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        lib2::lazy_string str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
        lib2::benchmarking::do_not_optimize(str);
    }
}

// Copy std::string SSO
const std::string sso_copy {"Hello!"};
void string_copy_constructor_sso(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::string str {sso_copy};
        lib2::benchmarking::do_not_optimize(str);
    }
}

void lazy_string_copy_string_sso(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        lib2::lazy_string str {sso_copy};
        lib2::benchmarking::do_not_optimize(str);
    }
}

// Copy std::string
const std::string copy {"Hello! I hope this string is large enough to put on the heap and not do SSO."};
void string_copy_constructor(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::string str {copy};
        lib2::benchmarking::do_not_optimize(str);
    }
}

void lazy_string_copy_string(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        lib2::lazy_string str {copy};
        lib2::benchmarking::do_not_optimize(str);
    }
}

// Move std::string SSO
void string_move_constructor_sso(lib2::benchmarking::state& state)
{
    std::string my_str {"Hello!"};
    
    for (auto _ : state)
    {
        std::string str {std::move(my_str)};
        lib2::benchmarking::do_not_optimize(str);
    }
}

// Move std::string
void string_move_constructor(lib2::benchmarking::state& state)
{
    std::string my_str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};

    for (auto _ : state)
    {
        std::string str {std::move(my_str)};
        lib2::benchmarking::do_not_optimize(str);
    }
}

// Copy Constructor SSO
void lazy_string_copy_constructor_sso(lib2::benchmarking::state& state)
{
    const lib2::lazy_string my_str {std::string{"Hello!"}};

    for (auto _ : state)
    {
        lib2::lazy_string str {my_str};
        lib2::benchmarking::do_not_optimize(str);
    }
}

// Copy Constructor
void lazy_string_copy_constructor(lib2::benchmarking::state& state)
{
    const lib2::lazy_string my_str {std::string{"Hello! I hope this string is large enough to put on the heap and not do SSO."}};

    for (auto _ : state)
    {
        lib2::lazy_string str {my_str};
        lib2::benchmarking::do_not_optimize(str);
    }
}

// Move Construct SSO
void lazy_string_move_constructor_sso(lib2::benchmarking::state& state)
{
    lib2::lazy_string my_str {std::string{"Hello!"}};

    for (auto _ : state)
    {
        lib2::lazy_string str {std::move(my_str)};
        lib2::benchmarking::do_not_optimize(str);
    }
}

// Move Constructor
void lazy_string_move_constructor(lib2::benchmarking::state& state)
{
    lib2::lazy_string my_str {"Hello! I hope this string is large enough to put on the heap and not do SSO."};

    for (auto _ : state)
    {
        lib2::lazy_string str {std::move(my_str)};
        lib2::benchmarking::do_not_optimize(str);
    }
}

// From std::format
void string_format_construct(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::string str {std::format("Hello! This is a {} example on how to utilize my {} new string class in lib{}!", "neat", "cool", 2)};
        lib2::benchmarking::do_not_optimize(str);
    }
}

void lazy_string_format_construct(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        lib2::lazy_string str {std::format("Hello! This is a {} example on how to utilize my {} new string class in lib{}!", "neat", "cool", 2)};
        lib2::benchmarking::do_not_optimize(str);
    }
}

int main()
{
    constexpr lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{5}};

    lib2::benchmarking::suite default_constructor;
    default_constructor.add("string", &string_default_construct);
    default_constructor.add("string_view", &string_view_default_construct);
    default_constructor.add("lazy_string", &lazy_string_default_construct);

    lib2::benchmarking::suite literal_construct_sso;
    literal_construct_sso.add("string", &string_literal_construct_sso);
    literal_construct_sso.add("string_view", &string_view_literal_construct_sso);
    literal_construct_sso.add("lazy_string", &lazy_string_literal_construct_sso);

    lib2::benchmarking::suite literal_construct;
    literal_construct.add("string", &string_literal_construct);
    literal_construct.add("string_view", &string_view_literal_construct);
    literal_construct.add("lazy_string", &lazy_string_literal_construct);

    lib2::benchmarking::suite copy_constructor_sso;
    copy_constructor_sso.add("string", &string_copy_constructor_sso);
    copy_constructor_sso.add("lazy_string", &lazy_string_copy_constructor_sso);

    lib2::benchmarking::suite copy_constructor;
    copy_constructor.add("string", &string_copy_constructor);
    copy_constructor.add("lazy_string", &lazy_string_copy_constructor);

    lib2::benchmarking::suite move_constructor_sso;
    move_constructor_sso.add("string", &string_move_constructor_sso);
    move_constructor_sso.add("lazy_string", &lazy_string_move_constructor_sso);

    lib2::benchmarking::suite move_constructor;
    move_constructor.add("string", &string_move_constructor);
    move_constructor.add("lazy_string", &lazy_string_move_constructor);

    lib2::benchmarking::suite format_construct;
    format_construct.add("string", &string_format_construct);
    format_construct.add("lazy_string", &lazy_string_format_construct);

    lib2::print<"SIZEOF(std::string) => {}\n">(sizeof(std::string));
    lib2::print<"SIZEOF(lazy_string) => {}\n">(sizeof(lib2::lazy_string));

    lib2::print<"Default Constructor:\n">();
    lib2::benchmarking::print_benchmarks(ctx, default_constructor);

    lib2::print<"\nString Literal Construct SSO:\n">();
    lib2::benchmarking::print_benchmarks(ctx, literal_construct_sso);

    lib2::print<"\nString Literal Construct:\n">();
    lib2::benchmarking::print_benchmarks(ctx, literal_construct);

    lib2::print<"\nCopy Constructor SSO:\n">();
    lib2::benchmarking::print_benchmarks(ctx, copy_constructor_sso);

    lib2::print<"\nCopy Constructor:\n">();
    lib2::benchmarking::print_benchmarks(ctx, copy_constructor);

    lib2::print<"\nMove Constructor SSO:\n">();
    lib2::benchmarking::print_benchmarks(ctx, move_constructor_sso);

    lib2::print<"\nMove Constructor:\n">();
    lib2::benchmarking::print_benchmarks(ctx, move_constructor);

    lib2::print<"\nstd::format call:\n">();
    lib2::benchmarking::print_benchmarks(ctx, format_construct);
}