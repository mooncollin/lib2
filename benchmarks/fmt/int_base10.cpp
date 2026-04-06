import std;

import lib2;

template<class T>
class int_benchmark : public lib2::benchmarking::benchmark
{
public:
    using lib2::benchmarking::benchmark::benchmark;
protected:
    static T get_int()
    {
        return static_cast<T>(std::rand() % std::numeric_limits<T>::max());
    }
};

template<class T>
    requires(std::signed_integral<T>)
class int_benchmark<T> : public lib2::benchmarking::benchmark
{
public:
    using lib2::benchmarking::benchmark::benchmark;
protected:
    static T get_int()
    {
        return static_cast<T>(std::rand() % std::numeric_limits<T>::max()) * sign();
    }
private:
    static T sign()
    {
        return static_cast<T>(std::rand() % 2 ? -1 : 1);
    }
};

template<class T>
class ostream_int final : public int_benchmark<T>
{
public:
    ostream_int()
        : int_benchmark<T>{"std::ostream"} {}

    void operator()()
    {
        ss.str({});
        ss << this->get_int();
    }
private:
    std::ostringstream ss;
};

template<class T>
class format_int final : public int_benchmark<T>
{
public:
    format_int()
        : int_benchmark<T>{"std::format"} {}

    void operator()()
    {
        const auto str {std::format("{}", this->get_int())};
        lib2::benchmarking::do_not_optimize(str);
    }
};

template<class T>
class to_string_int final : public int_benchmark<T>
{
public:
    to_string_int()
        : int_benchmark<T>{"std::to_string"} {}

    void operator()()
    {
        const auto str {std::to_string(this->get_int())};
        lib2::benchmarking::do_not_optimize(str);
    }
};

template<class T>
class lib2_format_int final : public int_benchmark<T>
{
public:
    lib2_format_int()
        : int_benchmark<T>{"lib2::format"} {}

    void operator()()
    {
        const auto str {lib2::format<"{}">(this->get_int())};
        lib2::benchmarking::do_not_optimize(str);
    }
};

int main()
{
    const lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{5}};

    lib2::benchmarking::benchmark_suite uint8_benchmarks;
    uint8_benchmarks.add_benchmark<format_int<std::uint8_t>>();
    uint8_benchmarks.add_benchmark<to_string_int<std::uint8_t>>();
    uint8_benchmarks.add_benchmark<lib2_format_int<std::uint8_t>>();

    lib2::benchmarking::benchmark_suite int8_benchmarks;
    int8_benchmarks.add_benchmark<format_int<std::int8_t>>();
    int8_benchmarks.add_benchmark<to_string_int<std::int8_t>>();
    int8_benchmarks.add_benchmark<lib2_format_int<std::int8_t>>();

    lib2::benchmarking::benchmark_suite uint16_benchmarks;
    uint16_benchmarks.add_benchmark<ostream_int<std::uint16_t>>();
    uint16_benchmarks.add_benchmark<format_int<std::uint16_t>>();
    uint16_benchmarks.add_benchmark<to_string_int<std::uint16_t>>();
    uint16_benchmarks.add_benchmark<lib2_format_int<std::uint16_t>>();

    lib2::benchmarking::benchmark_suite int16_benchmarks;
    int16_benchmarks.add_benchmark<ostream_int<std::int16_t>>();
    int16_benchmarks.add_benchmark<format_int<std::int16_t>>();
    int16_benchmarks.add_benchmark<to_string_int<std::int16_t>>();
    int16_benchmarks.add_benchmark<lib2_format_int<std::int16_t>>();

    lib2::benchmarking::benchmark_suite uint32_benchmarks;
    uint32_benchmarks.add_benchmark<ostream_int<std::uint32_t>>();
    uint32_benchmarks.add_benchmark<format_int<std::uint32_t>>();
    uint32_benchmarks.add_benchmark<to_string_int<std::uint32_t>>();
    uint32_benchmarks.add_benchmark<lib2_format_int<std::uint32_t>>();

    lib2::benchmarking::benchmark_suite int32_benchmarks;
    int32_benchmarks.add_benchmark<ostream_int<std::int32_t>>();
    int32_benchmarks.add_benchmark<format_int<std::int32_t>>();
    int32_benchmarks.add_benchmark<to_string_int<std::int32_t>>();
    int32_benchmarks.add_benchmark<lib2_format_int<std::int32_t>>();

    lib2::benchmarking::benchmark_suite uint64_benchmarks;
    uint64_benchmarks.add_benchmark<ostream_int<std::uint64_t>>();
    uint64_benchmarks.add_benchmark<format_int<std::uint64_t>>();
    uint64_benchmarks.add_benchmark<to_string_int<std::uint64_t>>();
    uint64_benchmarks.add_benchmark<lib2_format_int<std::uint64_t>>();

    lib2::benchmarking::benchmark_suite int64_benchmarks;
    int64_benchmarks.add_benchmark<ostream_int<std::int64_t>>();
    int64_benchmarks.add_benchmark<format_int<std::int64_t>>();
    int64_benchmarks.add_benchmark<to_string_int<std::int64_t>>();
    int64_benchmarks.add_benchmark<lib2_format_int<std::int64_t>>();
    
    lib2::print<"Default formatting of std::uint8:\n">();
    lib2::benchmarking::print_benchmarks(ctx, uint8_benchmarks);

    lib2::print<"\nDefault formatting of std::int8:\n">();
    lib2::benchmarking::print_benchmarks(ctx, int8_benchmarks);

    lib2::print<"\nDefault formatting of std::uint16:\n">();
    lib2::benchmarking::print_benchmarks(ctx, uint16_benchmarks);

    lib2::print<"\nDefault formatting of std::int16:\n">();
    lib2::benchmarking::print_benchmarks(ctx, int16_benchmarks);

    lib2::print<"\nDefault formatting of std::uint32:\n">();
    lib2::benchmarking::print_benchmarks(ctx, uint32_benchmarks);

    lib2::print<"\nDefault formatting of std::int32:\n">();
    lib2::benchmarking::print_benchmarks(ctx, int32_benchmarks);

    lib2::print<"\nDefault formatting of std::uint64:\n">();
    lib2::benchmarking::print_benchmarks(ctx, uint64_benchmarks);

    lib2::print<"\nDefault formatting of std::int64:\n">();
    lib2::benchmarking::print_benchmarks(ctx, int64_benchmarks);
}