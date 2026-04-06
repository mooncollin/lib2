import std;

import lib2;

class int_benchmark : public lib2::benchmarking::benchmark
{
public:
    using lib2::benchmarking::benchmark::benchmark;
protected:
    static int get_int()
    {
        return std::rand();
    }
};

class ostream_bench final : public int_benchmark
{
public:
    ostream_bench()
        : int_benchmark{"std::ostream"} {}

    void operator()()
    {
        ss.str({});
        ss << std::setbase(2) << std::showbase << this->get_int();
    }
private:
    std::ostringstream ss;
};

class format_bench final : public int_benchmark
{
public:
    format_bench()
        : int_benchmark{"std::format"} {}

    void operator()()
    {
        const auto str {std::format("{:#b}", this->get_int())};
        lib2::benchmarking::do_not_optimize(str);
    }
private:
    std::string str;
};

class lib2_format final : public int_benchmark
{
public:
    lib2_format()
        : int_benchmark{"lib2::format"} {}

    void operator()()
    {
        const auto str {lib2::format<"{:#b}">(this->get_int())};
        lib2::benchmarking::do_not_optimize(str);
    }
};

int main()
{
    const lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{5}};

    ostream_bench b1;
    format_bench b2;
    lib2_format b3;

    lib2::benchmarking::print_benchmarks(ctx,
        b1, b2, b3
    );
}