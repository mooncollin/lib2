#include "lib2/strings/widen.h"

import std;

import lib2;

class write_benchmark : public lib2::benchmarking::benchmark
{
public:
    using lib2::benchmarking::benchmark::benchmark;
protected:
    static constexpr std::string_view msg {"Hello, there! I hope this string is long enough, don't you?"};
    static inline const char* const msg_literal {"Hello, there! I hope this string is long enough, don't you?"};
};

class ostringstream_default_construct_benchmark : public lib2::benchmarking::benchmark
{
public:
    ostringstream_default_construct_benchmark()
        : lib2::benchmarking::benchmark{"std::ostringstream"} {}

    void operator()() final
    {
        std::ostringstream ss;
        lib2::benchmarking::do_not_optimize(ss);
    }
};

class stringbuf_default_construct_benchmark : public lib2::benchmarking::benchmark
{
public:
    stringbuf_default_construct_benchmark()
        : lib2::benchmarking::benchmark{"std::stringbuf"} {}

    void operator()() final
    {
        std::stringbuf sbuf {std::ios_base::out};
        lib2::benchmarking::do_not_optimize(sbuf);
    }
};

class string_default_construct_benchmark : public lib2::benchmarking::benchmark
{
public:
    string_default_construct_benchmark()
        : lib2::benchmarking::benchmark{"std::string"} {}

    void operator()() final
    {
        std::string str;
        lib2::benchmarking::do_not_optimize(str);
    }
};

class lib2_ostringstream_default_construct_benchmark : public lib2::benchmarking::benchmark
{
public:
    lib2_ostringstream_default_construct_benchmark()
        : lib2::benchmarking::benchmark{"lib2::ostringstream"} {}

    void operator()() final
    {
        lib2::ostringstream ss;
        lib2::benchmarking::do_not_optimize(ss);
    }
};



class ostringstream_init_write_benchmark : public write_benchmark
{
public:
    ostringstream_init_write_benchmark()
        : write_benchmark{"std::ostringstream"} {}

    void operator()() final
    {
        std::ostringstream ss;
        ss.write(msg.data(), msg.size());

        const auto view {ss.view()};
        lib2::benchmarking::do_not_optimize(view);
    }
};

class stringbuf_init_write_benchmark : public write_benchmark
{
public:
    stringbuf_init_write_benchmark()
        : write_benchmark{"std::stringbuf"} {}

    void operator()() final
    {
        std::stringbuf sbuf {std::ios_base::out};
        sbuf.sputn(msg.data(), msg.size());

        const auto view {sbuf.view()};
        lib2::benchmarking::do_not_optimize(view);
    }
};

class string_init_write_benchmark : public write_benchmark
{
public:
    string_init_write_benchmark()
        : write_benchmark{"std::string"} {}

    void operator()() final
    {
        std::string str;
        str.append(msg);
        lib2::benchmarking::do_not_optimize(std::string_view{str.data(), str.size()});
    }
};

class lib2_ostringstream_init_write_benchmark : public write_benchmark
{
public:
    lib2_ostringstream_init_write_benchmark()
        : write_benchmark{"lib2::ostringstream"} {}

    void operator()() final
    {
        lib2::ostringstream ss;
        ss.write(msg.data(), msg.size());

        const auto view {ss.view()};
        lib2::benchmarking::do_not_optimize(view);
    }
};

class ostringstream_char_write_benchmark : public lib2::benchmarking::benchmark
{
public:
    ostringstream_char_write_benchmark()
        : lib2::benchmarking::benchmark{"std::ostringstream"} {}

    void setup()
    {
        ss.str({});
    }

    void operator()() final
    {
        ss.put('H');
    }
private:
    std::ostringstream ss {std::ios_base::out};
};

class stringbuf_char_write_benchmark : public lib2::benchmarking::benchmark
{
public:
    stringbuf_char_write_benchmark()
        : lib2::benchmarking::benchmark{"std::stringbuf"} {}

    void setup()
    {
        sbuf.str({});
    }

    void operator()() final
    {
        sbuf.sputc('H');
    }
private:
    std::stringbuf sbuf {std::ios_base::out};
};

class string_char_write_benchmark : public lib2::benchmarking::benchmark
{
public:
    string_char_write_benchmark()
        : lib2::benchmarking::benchmark{"std::string"} {}

    void setup()
    {
        str.clear();
    }

    void operator()() final
    {
        str.push_back('H');
    }
private:
    std::string str;
};

class lib2_ostringstream_char_write_benchmark : public lib2::benchmarking::benchmark
{
public:
    lib2_ostringstream_char_write_benchmark()
        : lib2::benchmarking::benchmark{"lib2::ostringstream"} {}

    void setup()
    {
        ss.str({});
    }

    void operator()() final
    {
        ss.put('H');
    }
private:
    lib2::ostringstream ss;
};

class ostringstream_string_write_benchmark : public write_benchmark
{
public:
    ostringstream_string_write_benchmark()
        : write_benchmark{"std::ostringstream"} {}

    void setup()
    {
        ss.str({});
    }

    void operator()() final
    {
        ss.write(msg.data(), msg.size());
    }
private:
    std::ostringstream ss {std::ios_base::out};
};

class stringbuf_string_write_benchmark : public write_benchmark
{
public:
    stringbuf_string_write_benchmark()
        : write_benchmark{"std::stringbuf"} {}

    void setup()
    {
        sbuf.str({});
    }

    void operator()() final
    {
        sbuf.sputn(msg.data(), msg.size());
    }
private:
    std::stringbuf sbuf {std::ios_base::out};
};

class string_string_write_benchmark : public write_benchmark
{
public:
    string_string_write_benchmark()
        : write_benchmark{"std::string"} {}

    void setup()
    {
        str.clear();
    }

    void operator()() final
    {
        str.append(msg);
    }
private:
    std::string str;
};

class lib2_ostringstream_string_write_benchmark : public write_benchmark
{
public:
    lib2_ostringstream_string_write_benchmark()
        : write_benchmark{"lib2::ostringstream"} {}

    void setup()
    {
        ss.str({});
    }

    void operator()() final
    {
        ss.write(msg.data(), msg.size());
    }
private:
    lib2::ostringstream ss;
};


class ostringstream_string_literal_write_benchmark : public write_benchmark
{
public:
    ostringstream_string_literal_write_benchmark()
        : write_benchmark{"std::ostringstream"} {}

    void setup()
    {
        ss.str({});
    }

    void operator()() final
    {
        ss << msg_literal;
    }
private:
    std::ostringstream ss {std::ios_base::out};
};

class string_string_literal_write_benchmark : public write_benchmark
{
public:
    string_string_literal_write_benchmark()
        : write_benchmark{"std::string"} {}

    void setup()
    {
        str.clear();
    }

    void operator()() final
    {
        str += msg_literal;
    }
private:
    std::string str;
};

class lib2_ostringstream_string_literal_write_benchmark : public write_benchmark
{
public:
    lib2_ostringstream_string_literal_write_benchmark()
        : write_benchmark{"lib2::ostringstream"} {}

    void setup()
    {
        ss.str({});
    }

    void operator()() final
    {
        ss << msg_literal;
    }
private:
    lib2::ostringstream ss;
};

int main()
{
    const lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{5}};
    const lib2::benchmarking::benchmarking_iterations ctx2 {30'000'000};

    lib2::benchmarking::benchmark_suite default_construct;
    default_construct.add_benchmark<ostringstream_default_construct_benchmark>();
    default_construct.add_benchmark<stringbuf_default_construct_benchmark>();
    default_construct.add_benchmark<string_default_construct_benchmark>();
    default_construct.add_benchmark<lib2_ostringstream_default_construct_benchmark>();

    lib2::benchmarking::benchmark_suite initialize_and_write;
    initialize_and_write.add_benchmark<ostringstream_init_write_benchmark>();
    initialize_and_write.add_benchmark<stringbuf_init_write_benchmark>();
    initialize_and_write.add_benchmark<string_init_write_benchmark>();
    initialize_and_write.add_benchmark<lib2_ostringstream_init_write_benchmark>();

    lib2::benchmarking::benchmark_suite char_write;
    char_write.add_benchmark<ostringstream_char_write_benchmark>();
    char_write.add_benchmark<stringbuf_char_write_benchmark>();
    char_write.add_benchmark<string_char_write_benchmark>();
    char_write.add_benchmark<lib2_ostringstream_char_write_benchmark>();

    lib2::benchmarking::benchmark_suite string_write;
    string_write.add_benchmark<ostringstream_string_write_benchmark>();
    string_write.add_benchmark<stringbuf_string_write_benchmark>();
    string_write.add_benchmark<string_string_write_benchmark>();
    string_write.add_benchmark<lib2_ostringstream_string_write_benchmark>();

    lib2::benchmarking::benchmark_suite string_literal_write;
    string_literal_write.add_benchmark<ostringstream_string_literal_write_benchmark>();
    string_literal_write.add_benchmark<string_string_literal_write_benchmark>();
    string_literal_write.add_benchmark<lib2_ostringstream_string_literal_write_benchmark>();

    lib2::cout << "Default construction:\n";
    lib2::benchmarking::print_benchmarks(ctx, default_construct);

    lib2::cout << "\nInitialize and write:\n";
    lib2::benchmarking::print_benchmarks(ctx, initialize_and_write);

    lib2::cout << "\nWrite characters:\n";
    lib2::benchmarking::print_benchmarks(ctx, char_write);

    lib2::cout << "\nWrite strings:\n";
    lib2::benchmarking::print_benchmarks(ctx2, string_write);

    lib2::cout << "\nWrite literal strings:\n";
    lib2::benchmarking::print_benchmarks(ctx2, string_literal_write);
}