import std;

import lib2;

static constexpr std::string_view msg {"Hello, there! I hope this string is long enough, don't you?"};
static inline const char* const msg_literal {"Hello, there! I hope this string is long enough, don't you?"};


void ostringstream_default_construct_benchmark(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::ostringstream ss;
        lib2::benchmarking::do_not_optimize(ss);
    }
}

void stringbuf_default_construct_benchmark(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::stringbuf sbuf {std::ios_base::out};
        lib2::benchmarking::do_not_optimize(sbuf);
    }
}

void string_default_construct_benchmark(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::string str;
        lib2::benchmarking::do_not_optimize(str);
    }
}

void lib2_string_sink_default_construct_benchmark(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        lib2::io::string_sink ss;
        lib2::benchmarking::do_not_optimize(ss);
    }
}


void ostringstream_init_write_benchmark(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::ostringstream ss;
        ss.write(msg.data(), msg.size());

        const auto view {ss.view()};
        lib2::benchmarking::do_not_optimize(view);
    }
}

void stringbuf_init_write_benchmark(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::stringbuf sbuf {std::ios_base::out};
        sbuf.sputn(msg.data(), msg.size());

        const auto view {sbuf.view()};
        lib2::benchmarking::do_not_optimize(view);
    }
}

void string_init_write_benchmark(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        std::string str;
        str.append(msg);

        lib2::benchmarking::do_not_optimize(str);
    }
}

void lib2_string_sink_init_write_benchmark(lib2::benchmarking::state& state)
{
    for (auto _ : state)
    {
        lib2::io::string_sink ss;
        ss.write(msg);

        lib2::benchmarking::do_not_optimize(ss.str);
    }
}


void ostringstream_char_write_benchmark(lib2::benchmarking::state& state)
{
    std::ostringstream ss;

    for (auto _ : state)
    {
        ss.put('H');
    }
}

void stringbuf_char_write_benchmark(lib2::benchmarking::state& state)
{
    std::stringbuf sbuf {std::ios_base::out};

    for (auto _ : state)
    {
        sbuf.sputc('H');
    }
}

void string_char_write_benchmark(lib2::benchmarking::state& state)
{
    std::string str;

    for (auto _ : state)
    {
        str.push_back('H');
    }
}

void lib2_string_sink_char_write_benchmark(lib2::benchmarking::state& state)
{
    lib2::io::string_sink ss;

    for (auto _ : state)
    {
        ss.put('H');
    }
}


void ostringstream_string_write_benchmark(lib2::benchmarking::state& state)
{
    std::ostringstream ss;

    for (auto _ : state)
    {
        ss.write(msg.data(), msg.size());
    }
}

void stringbuf_string_write_benchmark(lib2::benchmarking::state& state)
{
    std::stringbuf sbuf {std::ios_base::out};

    for (auto _ : state)
    {
        sbuf.sputn(msg.data(), msg.size());
    }
}

void string_string_write_benchmark(lib2::benchmarking::state& state)
{
    std::string str;

    for (auto _ : state)
    {
        str.append(msg);
    }
}

void lib2_string_sink_string_write_benchmark(lib2::benchmarking::state& state)
{
    lib2::io::string_sink ss;

    for (auto _ : state)
    {
        ss.write(msg);
    }
}


void ostringstream_string_literal_write_benchmark(lib2::benchmarking::state& state)
{
    std::ostringstream ss;

    for (auto _ : state)
    {
        ss << msg_literal;
    }
}

void string_string_literal_write_benchmark(lib2::benchmarking::state& state)
{
    std::string str;

    for (auto _ : state)
    {
        str += msg_literal;
    }
}

void lib2_string_sink_string_literal_write_benchmark(lib2::benchmarking::state& state)
{
    lib2::io::string_sink ss;

    for (auto _ : state)
    {
        ss.write(msg_literal);
    }
}

int main()
{
    constexpr lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{5}};
    constexpr lib2::benchmarking::benchmarking_iterations ctx2 {30'000'000};

    lib2::benchmarking::suite default_construct;
    default_construct.add("std::ostringstream", &ostringstream_default_construct_benchmark);
    default_construct.add("std::stringbuf", &stringbuf_default_construct_benchmark);
    default_construct.add("std::string", &string_default_construct_benchmark);
    default_construct.add("lib2::io::string_sink", &lib2_string_sink_default_construct_benchmark);

    lib2::benchmarking::suite initialize_and_write;
    initialize_and_write.add("std::ostringstream", &ostringstream_init_write_benchmark);
    initialize_and_write.add("std::stringbuf", &stringbuf_init_write_benchmark);
    initialize_and_write.add("std::string", &string_init_write_benchmark);
    initialize_and_write.add("lib2::io::string_sink", &lib2_string_sink_init_write_benchmark);

    lib2::benchmarking::suite char_write;
    char_write.add("std::ostringstream", &ostringstream_char_write_benchmark);
    char_write.add("std::stringbuf", &stringbuf_char_write_benchmark);
    char_write.add("std::string", &string_char_write_benchmark);
    char_write.add("lib2::io::string_sink", &lib2_string_sink_char_write_benchmark);

    lib2::benchmarking::suite string_write;
    string_write.add("std::ostringstream", &ostringstream_string_write_benchmark);
    string_write.add("std::stringbuf", &stringbuf_string_write_benchmark);
    string_write.add("std::string", &string_string_write_benchmark);
    string_write.add("lib2::io::string_sink", &lib2_string_sink_string_write_benchmark);

    lib2::benchmarking::suite string_literal_write;
    string_literal_write.add("std::ostringstream", &ostringstream_string_literal_write_benchmark);
    string_literal_write.add("std::string", &string_string_literal_write_benchmark);
    string_literal_write.add("lib2::io::string_sink", &lib2_string_sink_string_literal_write_benchmark);

    lib2::print<"Default construction:\n">();
    lib2::benchmarking::print_benchmarks(ctx, default_construct);

    lib2::print<"\nInitialize and write:\n">();
    lib2::benchmarking::print_benchmarks(ctx, initialize_and_write);

    lib2::print<"\nWrite characters:\n">();
    lib2::benchmarking::print_benchmarks(ctx, char_write);

    lib2::print<"\nWrite strings:\n">();
    lib2::benchmarking::print_benchmarks(ctx2, string_write);

    lib2::print<"\nWrite literal strings:\n">();
    lib2::benchmarking::print_benchmarks(ctx2, string_literal_write);
}