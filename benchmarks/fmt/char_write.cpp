import std;

import lib2;

void ostream_bench(lib2::benchmarking::state& state)
{
    std::ostringstream ss;

    for (auto _ : state)
    {
        ss.str({});
        ss << std::setw(5)
           << std::setfill('%')
           << std::internal
           << 'I';
        lib2::benchmarking::do_not_optimize(ss);
    }
}

void format_bench(lib2::benchmarking::state& state)
{
    std::string str;

    for (auto _ : state)
    {
        str = std::format("{:%^5}", 'I');
        lib2::benchmarking::do_not_optimize(str);
    }
}

void lib2_fmt_ct_bench(lib2::benchmarking::state& state)
{
    std::string str;

    for (auto _ : state)
    {
        str = lib2::format<"{:%^5}">('I');
        lib2::benchmarking::do_not_optimize(str);
    }
}

void lib2_fmt_rt_bench(lib2::benchmarking::state& state)
{
    std::string str;

    for (auto _ : state)
    {
        str = lib2::format("{:%^5}", 'I');
        lib2::benchmarking::do_not_optimize(str);
    }
}

int main()
{
    constexpr lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{5}};

    lib2::benchmarking::suite suite;

    suite.add("std::ostringstream", &ostream_bench);
    suite.add("std::format", &format_bench);
    suite.add("lib2::format<Fmt>", &lib2_fmt_ct_bench);
    suite.add("lib2::format(Fmt)", &lib2_fmt_rt_bench);

    lib2::benchmarking::print_benchmarks(ctx, suite);
}