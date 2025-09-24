import std;

import lib2;

class ostream_bench : public lib2::benchmarking::benchmark
{
public:
    ostream_bench()
        : lib2::benchmarking::benchmark{"std::ostream"} {}

    void operator()() final
    {
        ss.str({});
        ss << std::setw(5)
           << std::setfill('%')
           << std::internal
           << 'I';
        lib2::benchmarking::do_not_optimize(ss);
    }
private:
    std::ostringstream ss;
};

class format_bench : public lib2::benchmarking::benchmark
{
public:
    format_bench()
        : lib2::benchmarking::benchmark{"std::format"} {}

    void operator()() final
    {
        str = std::format("{:%^5}", 'I');
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    std::string str;
};

class lib2_fmt_bench : public lib2::benchmarking::benchmark
{
public:
    lib2_fmt_bench()
        : lib2::benchmarking::benchmark{"lib2::format"} {}

    void operator()() final
    {
        str = lib2::format("{:%^5}", 'I');
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    std::string str;
};

int main()
{
    const lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{5}};
    
    ostream_bench b1;
    format_bench b2;
    lib2_fmt_bench b3;

    lib2::benchmarking::print_benchmarks(ctx,
        b1, b2, b3
    );
}