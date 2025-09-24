import std;

import lib2;

class multiple_lib2_fstreams final : public lib2::benchmarking::benchmark
{
public:
    multiple_lib2_fstreams(const std::size_t amount)
        : lib2::benchmarking::benchmark{"lib2_fstreams"}
        {
            fstreams.resize(amount);
        }

    void setup()
    {
        for (std::size_t i {0}; i < fstreams.size(); ++i)
        {
            fstreams[i].open(name() + std::to_string(i));
        }
    }

    void operator()()
    {
        for (std::size_t i {0}; i < fstreams.size(); ++i)
        {
            fstreams[i] << "Hello World! My name is " << name() << i << '\n';
        }
    }

    void tear_down()
    {
        fstreams.clear();
    }
private:
    std::vector<lib2::ofstream> fstreams;
};

class multiple_lib2_async_fstreams final : public lib2::benchmarking::benchmark
{
public:
    multiple_lib2_async_fstreams(const std::size_t amount)
        : lib2::benchmarking::benchmark{"lib2_async_fstreams"}
        {
            fstreams.resize(amount);
        }

    void setup()
    {
        for (std::size_t i {0}; i < fstreams.size(); ++i)
        {
            fstreams[i].open(name() + std::to_string(i));
        }
    }

    void operator()()
    {
        for (std::size_t i {0}; i < fstreams.size(); ++i)
        {
            fstreams[i] << "Hello World! My name is " << name() << i << '\n';
        }
    }

    void tear_down()
    {
        fstreams.clear();
    }
private:
    std::vector<lib2::async_ofstream> fstreams;
};

int main()
{
    const lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{10}};

    constexpr std::size_t amount_files {5};
    multiple_lib2_fstreams fstreams {amount_files};
    multiple_lib2_async_fstreams async_fstreams {amount_files};

    lib2::benchmarking::print_benchmarks(ctx,
       fstreams, async_fstreams
    );
}