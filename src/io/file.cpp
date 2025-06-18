module;

#include <cerrno>
#include <cstdio>

module lib2.io;

import std;

import :file;

namespace lib2
{
    [[noreturn]] void throw_io()
    {
        if (!errno)
        {
            errno = EIO;
        }
        throw std::system_error{errno, std::generic_category()};
    }

    file_ref::file_ref(std::FILE& handle) noexcept
        : handle_{std::addressof(handle)} {}

    void file_ref::flush()
    {
        errno = 0;
        if (std::fflush(handle_) == EOF)
        {
            throw_io();
        }
    }

    void file_ref::setbuf(const std::span<char> buffer, const buffer_mode mode)
    {
        errno = 0;
        if (std::setvbuf(handle_, buffer.data(), std::to_underlying(mode), buffer.size()))
        {
            throw_io();
        }
    }

    void file_ref::setbuf()
    {
        setbuf({}, buffer_mode::none);
    }

    bool file_ref::get(char& c)
    {
        errno = 0;
        const auto meta {std::fgetc(handle_)};
        if (meta == EOF)
        {
            if (std::ferror(handle_))
            {
                throw_io();
            }

            return false;
        }

        c = static_cast<char>(meta);
        return true;
    }

    bool file_ref::get(wchar_t& c)
    {
        errno = 0;
        const auto meta {std::fgetwc(handle_)};
        if (meta == WEOF)
        {
            if (std::ferror(handle_))
            {
                throw_io();
            }
            
            return false;
        }

        return static_cast<wchar_t>(meta);
    }

    void file_ref::put(const char ch)
    {
        errno = 0;
        if (std::fputc(ch, handle_) == EOF)
        {
            throw_io();
        }
    }

    void file_ref::put(const wchar_t ch)
    {
        errno = 0;
        if (std::fputwc(ch, handle_) == WEOF)
        {
            throw_io();
        }
    }

    void file_ref::unget(const char ch)
    {
        errno = 0;
        if (std::ungetc(ch, handle_) == EOF)
        {
            throw_io();
        }
    }

    void file_ref::unget(const wchar_t ch)
    {
        errno = 0;
        if (std::ungetwc(ch, handle_) == WEOF)
        {
            throw_io();
        }
    }

    long file_ref::tell() const
    {
        errno = 0;
        const auto result {std::ftell(handle_)};
        if (result == -1)
        {
            throw_io();
        }

        return result;
    }


    std::fpos_t file_ref::getpos() const
    {
        std::fpos_t result;
        errno = 0;
        if (std::fgetpos(handle_, &result))
        {
            throw_io();
        }

        return result;
    }

    void file_ref::setpos(const std::fpos_t& pos)
    {
        errno = 0;
        if (std::fsetpos(handle_, &pos))
        {
            throw_io();
        }
    }

    void file_ref::seek(const long offset, const seek_mode origin)
    {
        errno = 0;
        if (std::fseek(handle_, offset, std::to_underlying(origin)))
        {
            throw_io();
        }
    }

    wide_orientation file_ref::wide(const wide_orientation mode)
    {
        const auto result {std::fwide(handle_, std::to_underlying(mode))};
        if (result < 0)
        {
            return wide_orientation::narrow;
        }

        if (result > 0)
        {
            return wide_orientation::wide;
        }

        return wide_orientation::none;
    }

    void file_ref::rewind() noexcept
    {
        std::rewind(handle_);
    }

    void file_ref::clearerr() noexcept
    {
        std::clearerr(handle_);
    }

    bool file_ref::eof() const noexcept
    {
        return static_cast<bool>(std::feof(handle_));
    }

    file::file() noexcept
        : handle_{nullptr} {}

    file::file(native_handle_type&& handle) noexcept
        : handle_{std::exchange(handle, nullptr)} {}

    file::file(file&& other) noexcept
        : handle_{std::exchange(other.handle_, nullptr)} {}

    file& file::operator=(file&& other) noexcept
    {
        handle_ = std::exchange(other.handle_, nullptr);
        return *this;
    }

    file::~file() noexcept
    {
        try
        {
            close();
        }
        catch (...) {}
    }

    std::expected<file_ref, std::error_code> file::ref() const noexcept
    {
        if (!is_open())
        {
            return std::unexpected<std::error_code>{std::make_error_code(std::errc::bad_file_descriptor)};
        }

        return *handle_;
    }

    void file::reset(native_handle_type handle)
    {
        close();
        handle_ = handle;
    }

    std::expected<file_ref, std::error_code> file::open(const std::filesystem::path::string_type::value_type* name, const openmode mode)
    {
        close();

        #if _WIN32
        const auto err {_wfopen_s(&handle_, name, mode.wstr())};
        #else
        const auto err {std::fopen_s(&handle_, name, mode.str())};
        #endif

        if (err)
        {
            return std::unexpected<std::error_code>{std::make_error_code(std::errc{errno})};
        }

        return *handle_;
    }

    std::expected<file_ref, std::error_code> file::open(const std::filesystem::path::string_type& name, const openmode mode)
    {
        return open(name.c_str(), mode);
    }

    std::expected<file_ref, std::error_code> file::open(const std::filesystem::path& name, const openmode mode)
    {
        return open(name.native(), mode);
    }

    std::expected<file_ref, std::error_code> file::reopen(const std::filesystem::path::string_type::value_type* name, const openmode mode)
    {
        if (!is_open())
        {
            return std::unexpected<std::error_code>{std::make_error_code(std::errc::bad_file_descriptor)};
        }

        #if _WIN32
        const auto err {_wfreopen_s(&handle_, name, mode.wstr(), handle_)};
        #else
        const auto err {std::freopen_s(&handle, name, mode.str(), handle_)};
        #endif
        
        if (err)
        {
            return std::unexpected<std::error_code>{std::make_error_code(std::errc{err})};
        }

        return *handle_;
    }

    std::expected<file_ref, std::error_code> file::reopen(const std::filesystem::path::string_type& name, const openmode mode)
    {
        return reopen(name.c_str(), mode);
    }

    std::expected<file_ref, std::error_code> file::reopen(const std::filesystem::path& name, const openmode mode)
    {
        return reopen(name.native(), mode);
    }

    void file::close()
    {
        if (is_open())
        {
            errno = 0;
            const auto err {std::fclose(handle_)};
            handle_ = nullptr;
            if (err == EOF)
            {
                throw_io();
            }
        }
    }
}