import std;

import lib2;

class fstream_insertion : public lib2::benchmarking::benchmark
{
public:
    fstream_insertion()
        : lib2::benchmarking::benchmark{"fstream_insertion"} {}

    void setup() final
    {
        file.open(name());
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
};

class fprintf_ : public lib2::benchmarking::benchmark
{
public:
    fprintf_()
        : lib2::benchmarking::benchmark{"fprintf"} {}

    void setup() final
    {
        file = std::fopen(name().c_str(), "w");
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
};

class fstream_print : public lib2::benchmarking::benchmark
{
public:
    fstream_print()
        : lib2::benchmarking::benchmark{"fstream_print"} {}

    void setup() final
    {
        file.open(name());
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
};

class lib2_fstream : public lib2::benchmarking::benchmark
{
public:
    lib2_fstream()
        : lib2::benchmarking::benchmark{"lib2_fstream"} {}

    void setup() final
    {
        const auto err {file.open(name())};
        if (err)
        {
            lib2::test::fail(lib2::format("Cannot open file: {}", name()));
        }
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
};

class lib2_fstream_fmt : public lib2::benchmarking::benchmark
{
public:
    lib2_fstream_fmt()
        : lib2::benchmarking::benchmark{"lib2_fstream_fmt"} {}

    void setup() final
    {
        const auto err {file.open(name())};
        if (err)
        {
            lib2::test::fail(lib2::format("Cannot open file: {}", name()));
        }
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
};

int main()
{
    const lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{8}};

    fstream_insertion b1;
    fprintf_ b2;
    fstream_print b3;
    lib2_fstream b4;
    lib2_fstream_fmt b5;

    lib2::benchmarking::print_benchmarks(ctx,
        b1, b2, b3, b4, b5
    );
}