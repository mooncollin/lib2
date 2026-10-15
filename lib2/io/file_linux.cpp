module;

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

module lib2.io;

import std;

import lib2.err;

namespace lib2::io
{
    int get_linux_flags(const openmode mode) noexcept
    {
        int flags {};

        if ((mode & openmode::in) && (mode & openmode::out))
        {
            flags |= O_RDWR | O_CREAT;
        }
        else if (mode & openmode::in)
        {
            flags |= O_RDONLY;
        }
        else if (mode & openmode::out)
        {
            flags |= O_WRONLY | O_CREAT;
            if (!(mode & openmode::app))
            {
                flags |= O_TRUNC;
            }
        }

        if (mode & openmode::app)
        {
            flags |= O_APPEND;
        }

        return flags;
    }

    void file_sink::close()
    {
        if (handle)
        {
            if (::close(static_cast<int>(handle)) == -1)
            {
                handle = 0;
                throw os_error{"file_sink::close failed"};
            }
            handle = 0;
        }
    }

    void file_sink::open(const std::filesystem::path::string_type::value_type* const filename, openmode mode)
    {
        close();

        mode |= openmode::out;
        const auto result {::open(filename, get_linux_flags(mode), 0664)};
        if (result == -1)
        {
            handle = 0;
            throw os_error{"file_sink::open failed"};
        }

        handle = result;
    }

    void file_sink::write(const char* buf, std::size_t count)
    {
        if (!is_open())
        {
            throw std::logic_error{"file_sink: file not open"};
        }

        const auto written {::write(static_cast<int>(handle),  buf, count)};
        if (written == -1)
        {
            throw os_error{"file_sink::write failed"};
        }

        if (written != count)
        {
            // It is possible the disk is full, try again and get the proper error
            count -= written;
            buf += written;
            if (::write(static_cast<int>(handle), buf, count) != count)
            {
                throw os_error{"file_sink::write failed"};
            }
        }
    }

    void file_sink::put(const char c)
    {
        write(&c, 1);
    }

    void file_sink::fill(const char c, std::size_t count)
    {
        if (!is_open())
        {
            throw std::logic_error{"file_sink: file not open"};
        }

        while (count--)
        {
            if (::write(static_cast<int>(handle), &c, 1) != 1)
            {
                throw os_error{"file_sink::fill failed"};
            }
        }
    }

    void file_source::close()
    {
        if (handle)
        {
            if (::close(static_cast<int>(handle)) == -1)
            {
                handle = 0;
                throw os_error{"file_source::close failed"};
            }
            handle = 0;
        }
    }

    void file_source::open(const std::filesystem::path::string_type::value_type* const filename, openmode mode)
    {
        close();

        mode |= openmode::in;
        const auto result {::open(filename, get_linux_flags(mode), 0664)};
        if (result == -1)
        {
            handle = 0;
            throw os_error{"file_source::open failed"};
        }

        handle = result;
    }

    opt_char file_source::get()
    {
        char ch;
        if (read(&ch, 1))
        {
            return ch;
        }

        return {};
    }

    std::size_t file_source::read(char* const buf, const std::size_t n)
    {
        if (!is_open())
        {
            throw std::logic_error{"file_source: file not open"};
        }

        const auto result {::read(handle, buf, n)};
        if (result == -1)
        {
            throw os_error{"file_source::read failed"};
        }

        return static_cast<std::size_t>(result);
    }

    class std_sink
    {
    public:
        std_sink(FILE* fd) noexcept
            : fd{fd} {}

        std_sink(const std_sink&) = delete;
        std_sink(std_sink&&) = delete;

        std_sink& operator=(const std_sink&) = delete;
        std_sink& operator=(std_sink&&) = delete;

        inline void put(const char c) noexcept
        {
            std::fputc(c, fd);
        }

        inline void write(const char* const buf, const std::size_t n) noexcept
        {
            std::fwrite(buf, 1, n, fd);
        }

        inline void write(const char* const buf) noexcept
        {
            std::fputs(buf, fd);
        }

        inline void write(const std::span<const char> buf) noexcept
        {
            write(buf.data(), buf.size());
        }

        inline void fill(const char c, std::size_t n) noexcept
        {
            while (n--)
            {
                put(c);
            }
        }
    private:
        FILE* const fd;
    };
    static_assert(sink<std_sink>);

    class std_source
    {
    public:
        std_source(FILE* fd) noexcept
            : fd{fd}
        {
            std::setvbuf(fd, nullptr, _IONBF, 0);
        }

        std_source(const std_source&) = delete;
        std_source(std_source&&) = delete;

        std_source& operator=(const std_source&) = delete;
        std_source& operator=(std_source&&) = delete;

        inline opt_char get()
        {
            char ch;
            if (read(&ch, 1))
            {
                return ch;
            }

            return {};
        }

        inline std::size_t read(char* const buf, const std::size_t n)
        {
            const auto result {std::fread(buf, 1, n, fd)};
            if (!result && std::ferror(fd))
            {
                throw std::runtime_error{"std_source: read failed"};
            }

            return result;
        }

        inline std::size_t read(const std::span<char> buf)
        {
            return read(buf.data(), buf.size());
        }
    private:
        FILE* const fd;
    };
    static_assert(source<std_source>);

    dyn_sink init_cout() noexcept
    {
        static std_sink stdcout {stdout};
        return stdcout;
    }
    
    dyn_sink init_cerr() noexcept
    {
        static std_sink stdcerr {stderr};
        return stdcerr;
    }

    dyn_peekable_source init_cin() noexcept
    {
        static buffered_source<std_source, 4096> stdcin {stdin};
        return stdcin;
    }
}