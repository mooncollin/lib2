#include <cstdio>

import std;

import lib2;

constexpr std::size_t buffer_size {64 * 1024};
constexpr std::string_view name {"C++isawesome"};

void fstream_insertion(lib2::benchmarking::state& state)
{
    char buffer[buffer_size];
    std::ofstream file {"fstream"};
    file.rdbuf()->pubsetbuf(buffer, buffer_size);

    for (auto _ : state)
    {
        file << "Hello World! My name is " << name << '\n';
    }
}

void fprintf_(lib2::benchmarking::state& state)
{
    char buffer[buffer_size];
    auto file {std::fopen("fprintf", "w")};
    std::setvbuf(file, buffer, _IOFBF, buffer_size);

    for (auto _ : state)
    {
        std::fprintf(file, "Hello World! My name is %s\n", name.data());
    }

    std::fclose(file);
}

void fstream_print(lib2::benchmarking::state& state)
{
    char buffer[buffer_size];
    std::ofstream file {"fstream_print"};
    file.rdbuf()->pubsetbuf(buffer, buffer_size);

    for (auto _ : state)
    {
        std::print(file, "Hello World! My name is {}\n", name);
    }
}

void lib2_file_write(lib2::benchmarking::state& state)
{
    lib2::io::buffered_sink<lib2::io::file_sink, buffer_size> file {"lib2_file_write"};

    for (auto _ : state)
    {
        file.write("Hello World! My name is ");
        file.write(name);
        file.put('\n');
    }
}

void lib2_file_fmt(lib2::benchmarking::state& state)
{
    lib2::io::buffered_sink<lib2::io::file_sink, buffer_size> file {"lib2_file_fmt"};

    for (auto _ : state)
    {
        lib2::format_to<"Hello World! My name is {}\n">(file, name);
    }
}

void lib2_file_rt_fmt(lib2::benchmarking::state& state)
{
    lib2::io::buffered_sink<lib2::io::file_sink, buffer_size> file {"lib2_file_rt_fmt"};

    for (auto _ : state)
    {
        lib2::format_to(file, "Hello World! My name is {}\n", name);
    }
}

void lib2_file_vfmt(lib2::benchmarking::state& state)
{
    lib2::io::buffered_sink<lib2::io::file_sink, buffer_size> file {"lib2_file_vfmt"};

    for (auto _ : state)
    {
        lib2::vformat_to(file, "Hello World! My name is {}\n", lib2::make_format_args(name));
    }
}

// class lib2_async_fstream : public lib2::benchmarking::benchmark
// {
// public:
//     lib2_async_fstream()
//         : lib2::benchmarking::benchmark{"lib2_async_fstream"} {}

//     void setup() final
//     {
//         file.open(name());
//         file.setbuf(buffer_size);
//     }

//     void operator()() final
//     {
//         lib2::format_to<"Hello World! My name is {}\n">(file, name());
//     }

//     void tear_down() final
//     {
//         file.close();
//     }
// private:
//     lib2::async_ofstream file;
// };

int main()
{
    constexpr lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{8}};

    lib2::benchmarking::suite suite;

    suite.add("fstream", &fstream_insertion);
    suite.add("fprintf_", &fprintf_);
    suite.add("fstream_print", &fstream_print);
    suite.add("lib2_file_write", &lib2_file_write);
    suite.add("lib2_file_fmt", &lib2_file_fmt);
    suite.add("lib2_file_rt_fmt", &lib2_file_rt_fmt);
    suite.add("lib2_file_vfmt", &lib2_file_vfmt);

    lib2::benchmarking::print_benchmarks(ctx, suite);
}