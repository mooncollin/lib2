#include <cstdio>

import std;

import lib2;

constexpr std::size_t buffer_size{64 * 1024};

class fstream_insertion : public lib2::benchmarking::benchmark
{
public:
    fstream_insertion()
        : lib2::benchmarking::benchmark{"fstream"} {}

    void setup() final
    {
        file.open(name());
        file.rdbuf()->pubsetbuf(buffer, buffer_size);
    }

    void operator()() final
    {
        file << "Hello World! My name is " << name() << '\n';
    }

    void tear_down() final
    {
        file.close();
    }
private:
    std::ofstream file;
    char buffer[buffer_size];
};

class fprintf_ : public lib2::benchmarking::benchmark
{
public:
    fprintf_()
        : lib2::benchmarking::benchmark{"fprintf"} {}

    void setup() final
    {
        file = std::fopen(name().c_str(), "w");
        std::setvbuf(file, buffer, _IOFBF, buffer_size);
    }

    void operator()() final
    {
        std::fprintf(file, "Hello World! My name is %s\n", name().c_str());
    }

    void tear_down() final
    {
        std::fclose(file);
    }
private:
    std::FILE* file;
    char buffer[buffer_size];
};

class fstream_print : public lib2::benchmarking::benchmark
{
public:
    fstream_print()
        : lib2::benchmarking::benchmark{"fstream_print"} {}

    void setup() final
    {
        file.open(name());
        file.rdbuf()->pubsetbuf(buffer, buffer_size);
    }

    void operator()() final
    {
        std::print(file, "Hello World! My name is {}\n", name());
    }

    void tear_down() final
    {
        file.close();
    }
private:
    std::ofstream file;
    char buffer[buffer_size];
};

class lib2_fstream : public lib2::benchmarking::benchmark
{
public:
    lib2_fstream()
        : lib2::benchmarking::benchmark{"lib2_fstream"} {}

    void setup() final
    {
        file.open(name());
        file.setbuf(buffer, buffer_size);
    }

    void operator()() final
    {
        file << "Hello World! My name is " << name() << '\n';
    }

    void tear_down() final
    {
        file.close();
    }
private:
    lib2::ofstream file;
    char buffer[buffer_size];
};

class lib2_fstream_fmt : public lib2::benchmarking::benchmark
{
public:
    lib2_fstream_fmt()
        : lib2::benchmarking::benchmark{"lib2_fstream_fmt"} {}

    void setup() final
    {
        file.open(name());
        file.setbuf(buffer, buffer_size);
    }

    void operator()() final
    {
        lib2::format_to(file, "Hello World! My name is {}\n", name());
    }

    void tear_down() final
    {
        file.close();
    }
private:
    lib2::ofstream file;
    char buffer[buffer_size];
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
        file << "Hello World! My name is " << name() << '\n';
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
    lib2_fstream b4;
    lib2_fstream_fmt b5;
    lib2_async_fstream b6;

    lib2::benchmarking::print_benchmarks(ctx,
        b1, b2, b3, b4, b5, b6
    );
}