import std;

import lib2;

void lib2_buf(const std::size_t buf_size, lib2::benchmarking::state& state)
{
    const auto name {lib2::format<"lib2_file_sink ({} KB)">(buf_size / 1024)};
    lib2::io::heap_buffered_sink<lib2::io::file_sink> file {buf_size, name};

    for (auto _ : state)
    {
        lib2::format_to<"Hello World! My name is {}\n">(file, name);
    }
    
    file.flush();
    file.sink.close();
    std::filesystem::remove(name);
}

// void lib2_async_buf(const std::size_t buf_size, lib2::benchmarking::state& state)
// {
//     const auto name {lib2::format<"lib2_async_file_sink ({} KB)">(buf_size / 1024)};
//     lib2::io::heap_buffered_sink<lib2::io::async_file_sink> file {buf_size, name};

//     for (auto _ : state)
//     {
//         lib2::format_to<"Hello World! My name is {}\n">(file, name);
//     }
    
//     file.flush();
//     file.sink.close();
//     std::filesystem::remove(name);
// }

int main()
{
    const lib2::benchmarking::benchmarking_min_time ctx {std::chrono::seconds{8}};

    lib2::benchmarking::suite suite;
    suite.add("lib2_file_sink (4 KB)", std::bind_front(&lib2_buf, 4096));
    suite.add("lib2_file_sink (8 KB)", std::bind_front(&lib2_buf, 8192));
    suite.add("lib2_file_sink (16 KB)", std::bind_front(&lib2_buf, 16384));
    suite.add("lib2_file_sink (32 KB)", std::bind_front(&lib2_buf, 32768));
    suite.add("lib2_file_sink (64 KB)", std::bind_front(&lib2_buf, 65536));
    suite.add("lib2_file_sink (128 KB)", std::bind_front(&lib2_buf, 131072));
    suite.add("lib2_file_sink (256 KB)", std::bind_front(&lib2_buf, 262144));
    suite.add("lib2_file_sink (1024 KB)", std::bind_front(&lib2_buf, 1048576));

    // suite.add("lib2_async_file_sink (4 KB)", std::bind_front(4096, &lib2_async_buf));
    // suite.add("lib2_async_file_sink (8 KB)", std::bind_front(8192, &lib2_async_buf));
    // suite.add("lib2_async_file_sink (16 KB)", std::bind_front(16384, &lib2_async_buf));
    // suite.add("lib2_async_file_sink (32 KB)", std::bind_front(32768, &lib2_async_buf));
    // suite.add("lib2_async_file_sink (64 KB)", std::bind_front(65536, &lib2_async_buf));
    // suite.add("lib2_async_file_sink (128 KB)", std::bind_front(131072, &lib2_async_buf));
    // suite.add("lib2_async_file_sink (256 KB)", std::bind_front(262144, &lib2_async_buf));
    // suite.add("lib2_async_file_sink (1024 KB)", std::bind_front(1048576, &lib2_async_buf));

    lib2::benchmarking::print_benchmarks(ctx, suite);
}