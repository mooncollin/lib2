import std;

import lib2;

int get_int() noexcept
{
    return std::rand();
}

void ostream_bench(lib2::benchmarking::state& state)
{
    std::ostringstream ss;

    for (auto _ : state)
    {
        ss.str({});
        ss << std::setbase(2) << std::showbase << get_int();
    }
}

void format_bench(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        const auto str {std::format("{:#b}", get_int())};
        lib2::benchmarking::do_not_optimize(str);
    }
}

void lib2_format(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        const auto str {lib2::format<"{:#b}">(get_int())};
        lib2::benchmarking::do_not_optimize(str);
    }
}

int main()
{
    constexpr lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{5}};

    lib2::benchmarking::suite suite;

    suite.add("std::ostringstream", &ostream_bench);
    suite.add("std::format", &format_bench);
    suite.add("lib2::format", &lib2_format);

    lib2::benchmarking::print_benchmarks(ctx, suite);
}