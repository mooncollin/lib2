import std;

import lib2;

template<class CharT, class Traits, class... Ts>
std::basic_istream<CharT, Traits>& scan(std::basic_istream<CharT, Traits>& is, Ts&... ts)
{
    (is >> ... >> ts);
    return is;
}

template<class CharT, class Traits, class... Ts>
std::basic_string_view<CharT, Traits> scan(const std::basic_string_view<CharT, Traits> str, Ts&... ts)
{
    std::basic_ispanstream<CharT, Traits> is {str};
    is.exceptions(std::ios_base::badbit | std::ios_base::failbit);
    scan(is, ts...);

    if (is.eof())
    {
        return {};
    }

    return {str.data() + is.tellg(), str.size() - is.tellg()};
}

template<class CharT, class... Ts>
std::basic_string_view<CharT> scan(const CharT* const str, Ts&... ts)
{
    return scan(std::basic_string_view<CharT>{str}, ts...);
}

class istream_benchmark : public lib2::benchmarking::benchmark
{
public:
    istream_benchmark()
        : lib2::benchmarking::benchmark{"istream"} {}

    void operator()() final
    {
        int v;
        const auto leftover {scan(str, v)};
        lib2::benchmarking::do_not_optimize(leftover);
    }
private:
    static constexpr std::string_view str {"123f"};
};

class istream_setup_benchmark : public lib2::benchmarking::benchmark
{
public:
    istream_setup_benchmark()
        : lib2::benchmarking::benchmark{"istream_setup"}
        , is{str} {}

    void setup() override
    {
        is.exceptions(std::ios_base::badbit | std::ios_base::failbit);
    }
    
    void operator()() final
    {
        is.span(str);
        int v;
        is >> v;
        lib2::benchmarking::do_not_optimize(v);
    }
private:
    std::ispanstream is;
    static constexpr std::string_view str {"123f"};
};

class sscanf_benchmark : public lib2::benchmarking::benchmark
{
public:
    sscanf_benchmark()
        : lib2::benchmarking::benchmark{"sscanf"} {}

    void operator()() final
    {
        int v;
        std::sscanf("123f", "%d", &v);
        lib2::benchmarking::do_not_optimize(v);
    }
};

class from_chars_benchmark : public lib2::benchmarking::benchmark
{
public:
    from_chars_benchmark()
        : lib2::benchmarking::benchmark{"from_chars"} {}

    void operator()() final
    {
        const auto str {"123f"};
        int v;
        std::from_chars(str, str + 4, v);
        lib2::benchmarking::do_not_optimize(v);
    }
};

int main()
{
    const lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{5}};

    istream_benchmark b1;
    sscanf_benchmark b2;
    from_chars_benchmark b3;
    istream_setup_benchmark b4;

    lib2::benchmarking::print_benchmarks(ctx,
        b1, b2, b3, b4
    );
}