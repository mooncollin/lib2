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

class ostream_bench : public int_benchmark
{
public:
    ostream_bench()
        : int_benchmark{"std::ostream"} {}

    void operator()() final
    {
        ss << std::setbase(2) << std::showbase << this->get_int();
    }
private:
    std::ostringstream ss;
};

class format_bench : public int_benchmark
{
public:
    format_bench()
        : int_benchmark{"std::format"} {}

    void operator()() final
    {
        std::format_to(std::back_inserter(str), "{:#b}", this->get_int());
    }
private:
    std::string str;
};

class lib2_format : public int_benchmark
{
public:
    lib2_format()
        : int_benchmark{"lib2::format"} {}

    void operator()() final
    {
        lib2::format_to(ss, "{:#b}", this->get_int());
    }
private:
    lib2::ostringstream ss;
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