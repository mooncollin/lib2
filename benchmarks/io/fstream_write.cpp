import std;

import lib2;

class lib2_buf : public lib2::benchmarking::benchmark
{
public:
    lib2_buf(std::size_t buf_size)
        : lib2::benchmarking::benchmark{lib2::format("lib2_fstream ({} KB)", buf_size / 1024)}
        , buf_size{buf_size} {}

    void setup() final
    {
        const auto err {file.open(name())};
        if (err)
        {
            lib2::test::fail(lib2::format("Cannot open file: {}", name()));
        }

        file.setbuf(nullptr, buf_size);
    }

    void operator()() final
    {
        lib2::format_to(file, "Hello World! My name is {}\n", name());
    }

    void tear_down() final
    {
        file.close();
        std::filesystem::remove(name());
    }
private:
    std::size_t buf_size;
    lib2::ofstream file;
};

int main()
{
    const lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{8}};

    lib2_buf b1 {4096};
    lib2_buf b2 {8192};
    lib2_buf b3 {16384};
    lib2_buf b4 {32768};
    lib2_buf b5 {65536};
    lib2_buf b6 {131072};
    lib2_buf b7 {262144};
    lib2_buf b8 {1048576};

    lib2::benchmarking::print_benchmarks(ctx,
        b1, b2, b3, b4, b5, b6, b7, b8
    );
}