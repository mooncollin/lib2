import std;

import lib2;

template<std::integral T>
T sign() noexcept
{
    return static_cast<T>(std::rand() % 2 ? -1 : 1);
}

template<std::integral T>
T get_int() noexcept
{
    const auto value {std::rand() % std::numeric_limits<T>::max()};

    if constexpr (std::signed_integral<T>)
    {
        return value * sign<T>();
    }
    else
    {
        return value;
    }
}

template<class T>
void ostream_int(lib2::benchmarking::state& state)
{
    std::ostringstream ss;

    for (auto _ : state)
    {
        ss.str({});
        ss << get_int<T>();
    }
}

template<class T>
void format_int(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        const auto str {std::format("{}", get_int<T>())};
        lib2::benchmarking::do_not_optimize(str);
    }
}

template<class T>
void to_string_int(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        const auto str {std::to_string(get_int<T>())};
        lib2::benchmarking::do_not_optimize(str);
    }
}

template<class T>
void lib2_format_int(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        const auto str {lib2::format<"{}">(get_int<T>())};
        lib2::benchmarking::do_not_optimize(str);
    }
}

int main()
{
    constexpr lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{5}};

    lib2::benchmarking::suite uint8_benchmarks;
    uint8_benchmarks.add("std::format", &format_int<std::uint8_t>);
    uint8_benchmarks.add("std::to_string", &to_string_int<std::uint8_t>);
    uint8_benchmarks.add("lib2::format", &lib2_format_int<std::uint8_t>);

    lib2::benchmarking::suite int8_benchmarks;
    int8_benchmarks.add("std::format", &format_int<std::int8_t>);
    int8_benchmarks.add("std::to_string", &to_string_int<std::int8_t>);
    int8_benchmarks.add("lib2::format", &lib2_format_int<std::int8_t>);

    lib2::benchmarking::suite uint16_benchmarks;
    uint16_benchmarks.add("std::ostringstream", &ostream_int<std::uint16_t>);
    uint16_benchmarks.add("std::format", &format_int<std::uint16_t>);
    uint16_benchmarks.add("std::to_string", &to_string_int<std::uint16_t>);
    uint16_benchmarks.add("lib2::format", &lib2_format_int<std::uint16_t>);

    lib2::benchmarking::suite int16_benchmarks;
    int16_benchmarks.add("std::ostringstream", &ostream_int<std::int16_t>);
    int16_benchmarks.add("std::format", &format_int<std::int16_t>);
    int16_benchmarks.add("std::to_string", &to_string_int<std::int16_t>);
    int16_benchmarks.add("lib2::format", &lib2_format_int<std::int16_t>);

    lib2::benchmarking::suite uint32_benchmarks;
    uint32_benchmarks.add("std::ostringstream", &ostream_int<std::uint32_t>);
    uint32_benchmarks.add("std::format", &format_int<std::uint32_t>);
    uint32_benchmarks.add("std::to_string", &to_string_int<std::uint32_t>);
    uint32_benchmarks.add("lib2::format", &lib2_format_int<std::uint32_t>);

    lib2::benchmarking::suite int32_benchmarks;
    int32_benchmarks.add("std::ostringstream", &ostream_int<std::int32_t>);
    int32_benchmarks.add("std::format", &format_int<std::int32_t>);
    int32_benchmarks.add("std::to_string", &to_string_int<std::int32_t>);
    int32_benchmarks.add("lib2::format", &lib2_format_int<std::int32_t>);

    lib2::benchmarking::suite uint64_benchmarks;
    uint64_benchmarks.add("std::ostringstream", &ostream_int<std::uint64_t>);
    uint64_benchmarks.add("std::format", &format_int<std::uint64_t>);
    uint64_benchmarks.add("std::to_string", &to_string_int<std::uint64_t>);
    uint64_benchmarks.add("lib2::format", &lib2_format_int<std::uint64_t>);

    lib2::benchmarking::suite int64_benchmarks;
    int64_benchmarks.add("std::ostringstream", &ostream_int<std::int64_t>);
    int64_benchmarks.add("std::format", &format_int<std::int64_t>);
    int64_benchmarks.add("std::to_string", &to_string_int<std::int64_t>);
    int64_benchmarks.add("lib2::format", &lib2_format_int<std::int64_t>);
    
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