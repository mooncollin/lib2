#include <cstdio>

import std;

import lib2;

constexpr std::size_t buffer_size{64 * 1024};

class fstream_insertion final : public lib2::benchmarking::benchmark
{
public:
    fstream_insertion()
        : lib2::benchmarking::benchmark{"fstream"} {}

    void setup()
    {
        file.open(name());
        file.rdbuf()->pubsetbuf(buffer, buffer_size);
    }

    void operator()()
    {
        file << "Hello World! My name is " << name() << '\n';
    }

    void tear_down()
    {
        file.close();
    }
private:
    std::ofstream file;
    char buffer[buffer_size];
};

class fprintf_ final : public lib2::benchmarking::benchmark
{
public:
    fprintf_()
        : lib2::benchmarking::benchmark{"fprintf"} {}

    void setup()
    {
        file = std::fopen(name().c_str(), "w");
        std::setvbuf(file, buffer, _IOFBF, buffer_size);
    }

    void operator()()
    {
        std::fprintf(file, "Hello World! My name is %s\n", name().c_str());
    }

    void tear_down()
    {
        std::fclose(file);
    }
private:
    std::FILE* file;
    char buffer[buffer_size];
};

class fstream_print final : public lib2::benchmarking::benchmark
{
public:
    fstream_print()
        : lib2::benchmarking::benchmark{"fstream_print"} {}

    void setup()
    {
        file.open(name());
        file.rdbuf()->pubsetbuf(buffer, buffer_size);
    }

    void operator()()
    {
        std::print(file, "Hello World! My name is {}\n", name());
    }

    void tear_down()
    {
        file.close();
    }
private:
    std::ofstream file;
    char buffer[buffer_size];
};

class lib2_fstream_write final : public lib2::benchmarking::benchmark
{
public:
    lib2_fstream_write()
        : lib2::benchmarking::benchmark{"lib2_fstream_write"} {}

    void setup()
    {
        file.open(name());
        file.setbuf(buffer, buffer_size);
    }

    void operator()()
    {
        lib2::text_ostream tfile {file};
        tfile.write("Hello World! My name is ");
        tfile.write(name());
        tfile.put('\n');
    }

    void tear_down()
    {
        file.close();
    }
private:
    lib2::ofstream file;
    std::byte buffer[buffer_size];
};

class lib2_fstream_fmt final : public lib2::benchmarking::benchmark
{
public:
    lib2_fstream_fmt()
        : lib2::benchmarking::benchmark{"lib2_fstream_fmt"} {}

    void setup()
    {
        file.open(name());
        file.setbuf(buffer, buffer_size);
    }

    void operator()()
    {
        lib2::format_to<"Hello World! My name is {}\n">(file, name());
    }

    void tear_down()
    {
        file.close();
    }
private:
    lib2::ofstream file;
    std::byte buffer[buffer_size];
};

class lib2_fstream_rt_fmt final : public lib2::benchmarking::benchmark
{
public:
    lib2_fstream_rt_fmt()
        : lib2::benchmarking::benchmark{"lib2_fstream_rt_fmt"} {}

    void setup()
    {
        file.open(name());
        file.setbuf(buffer, buffer_size);
    }

    void operator()()
    {
        lib2::format_to(file, "Hello World! My name is {}\n", name());
    }

    void tear_down()
    {
        file.close();
    }
private:
    lib2::ofstream file;
    std::byte buffer[buffer_size];
};

class lib2_fstream_vfmt final : public lib2::benchmarking::benchmark
{
public:
    lib2_fstream_vfmt()
        : lib2::benchmarking::benchmark{"lib2_fstream_vfmt"} {}

    void setup()
    {
        file.open(name());
        file.setbuf(buffer, buffer_size);
    }

    void operator()()
    {
        lib2::vformat_to(file, "Hello World! My name is {}\n", lib2::make_format_args(name()));
    }

    void tear_down()
    {
        file.close();
    }
private:
    lib2::ofstream file;
    std::byte buffer[buffer_size];
};

class lib2_async_fstream : public lib2::benchmarking::benchmark
{
public:
    lib2_async_fstream()
        : lib2::benchmarking::benchmark{"lib2_async_fstream"} {}

    void setup() final
    {
        file.open(name());
        file.setbuf(buffer_size);
    }

    void operator()() final
    {
        lib2::format_to<"Hello World! My name is {}\n">(file, name());
    }

    void tear_down() final
    {
        file.close();
    }
private:
    lib2::async_ofstream file;
};

int main()
{
    const lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{8}};

    fstream_insertion b1;
    fprintf_ b2;
    fstream_print b3;
    lib2_fstream_write b4;
    lib2_fstream_fmt b5;
    lib2_async_fstream b6;
    lib2_fstream_rt_fmt b7;
    lib2_fstream_vfmt b8;

    lib2::benchmarking::print_benchmarks(ctx,
        b1, b2, b3, b4, b5, b6, b7, b8
    );
}