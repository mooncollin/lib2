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
        file.open(name());
        file.setbuf(nullptr, buf_size);
    }

    void operator()() final
    {
        file << "Hello World! My name is " << name() << '\n';
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

class lib2_async_buf : public lib2::benchmarking::benchmark
{
public:
    lib2_async_buf(std::size_t buf_size)
        : lib2::benchmarking::benchmark{lib2::format("lib2_async_fstream ({} KB)", buf_size / 1024)}
        , buf_size{buf_size} {}

    void setup() final
    {
        file.open(name());
        file.setbuf(buf_size);
    }

    void operator()() final
    {
        file << "Hello World! My name is " << name() << '\n';
    }

    void tear_down() final
    {
        file.close();
        std::filesystem::remove(name());
    }
private:
    std::size_t buf_size;
    lib2::async_ofstream file;
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

    lib2_async_buf ba1 {4096};
    lib2_async_buf ba2 {8192};
    lib2_async_buf ba3 {16384};
    lib2_async_buf ba4 {32768};
    lib2_async_buf ba5 {65536};
    lib2_async_buf ba6 {131072};
    lib2_async_buf ba7 {262144};
    lib2_async_buf ba8 {1048576};

    lib2::benchmarking::print_benchmarks(ctx,
        b1, b2, b3, b4, b5, b6, b7, b8,
        ba1, ba2, ba3, ba4, ba5, ba6, ba7, ba8
    );
}