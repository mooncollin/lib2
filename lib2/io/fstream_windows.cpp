module;

#include <cstddef>
#include <windows.h>
#include <minwinbase.h>

module lib2.io;

import std;

import lib2.utility;

import :fstream;

namespace lib2
{
    constexpr std::size_t DWORDS_PER_SIZE_T {(sizeof(std::size_t) * 8 + sizeof(DWORD) * 8 - 1) / (sizeof(DWORD) * 8)};
    constexpr std::size_t default_buffer_size {8192};

    template<class CharT>
    struct async_io
    {
    private:
        async_io(const std::size_t buf_size) noexcept
            : next{nullptr}, capacity{buf_size} {}
    public:
        async_io* next;
        std::size_t capacity;
        OVERLAPPED ov[DWORDS_PER_SIZE_T] {};
        CharT buffer[1];
        
        static async_io* create(const std::size_t buf_size)
        {
            const auto total_size {sizeof(async_io) + (buf_size - 1) * sizeof(CharT)};
            async_io* mem {static_cast<async_io*>(::operator new(total_size))};
            return new (mem) async_io{buf_size};
        }

        static void destroy(async_io* const aio) noexcept
        {
            if (aio)
            {
                destroy(aio->next);
                aio->~async_io();
                ::operator delete(aio);
            }
        }
    };

    static bool is_console(const HANDLE handle) noexcept
    {
        const auto type {GetFileType(handle)};
        if (type != FILE_TYPE_CHAR) return false; // Cheap pre-check

        DWORD mode;
        return GetConsoleMode(handle, &mode) != 0;
    }

    static void io_write_console(const HANDLE handle, const char* data, std::size_t size)
    {
        while (size)
        {
            const auto chunk {static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()))};
            DWORD written {0};
            if (!WriteConsoleA(handle, data, chunk, &written, nullptr))
            {
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Write console failed"};
            }

            data += written;
            size -= written;
        }
    }

    static void io_write_console(const HANDLE handle, const wchar_t* data, std::size_t size)
    {
        while (size)
        {
            const auto chunk {static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()))};
            DWORD written {0};
            if (!WriteConsoleW(handle, data, chunk, &written, nullptr))
            {
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Write console failed"};
            }

            data += written;
            size -= written;
        }
    }

    static void io_write_console(const HANDLE handle, const std::byte* data, std::size_t size)
    {
        return io_write_console(handle, reinterpret_cast<const char*>(data), size);   
    }

    template<class CharT>
    static void io_write(const HANDLE handle, const CharT* data, std::size_t size)
    {
        if (is_console(handle))
        {
            io_write_console(handle, data, size);
            return;
        }
        
        size *= sizeof(CharT);
        while (size)
        {
            const auto chunk {static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()))};
            DWORD written {0};
            if (!WriteFile(handle, data, chunk, &written, nullptr) || (written != chunk))
            {
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Write failed"};
            }

            data += written / sizeof(CharT);
            size -= written;
        }
    }

    static std::size_t io_read_console(const HANDLE handle, char* data, std::size_t size)
    {
        std::size_t amount_read {0};
        while (size)
        {
            const auto chunk {static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()))};
            DWORD read {0};
            if (!ReadConsoleA(handle, data, chunk, &read, nullptr))
            {
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Write console failed"};
            }

            data += read;
            size -= read;
            amount_read += read;
        }

        return amount_read;
    }

    static std::size_t io_read_console(const HANDLE handle, wchar_t* data, std::size_t size)
    {
        std::size_t amount_read {0};
        while (size)
        {
            const auto chunk {static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()))};
            DWORD read {0};
            if (!ReadConsoleW(handle, data, chunk, &read, nullptr))
            {
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Write console failed"};
            }

            data += read;
            size -= read;
            amount_read += read;
        }

        return amount_read;
    }

    static std::size_t io_read_console(const HANDLE handle, std::byte* const data, const std::size_t size)
    {
        return io_read_console(handle, reinterpret_cast<char*>(data), size);
    }

    template<class CharT>
    static std::size_t io_read(const HANDLE handle, CharT* data, std::size_t size)
    {
        if (is_console(handle))
        {
            return io_read_console(handle, data, size);
        }

        size *= sizeof(CharT);
        std::size_t total_read {0};
        while (size)
        {
            const auto chunk {static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()))};
            DWORD read {0};
            if (!ReadFile(handle, data, chunk, &read, nullptr))
            {
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Read failed"};
            }

            if (!read)
            {
                break;
            }

            data += read / sizeof(CharT);
            size -= read;
            total_read += read;
        }

        return total_read / sizeof(CharT);
    }

    static void io_seek(const HANDLE handle, const std::int64_t offset, const seek_mode origin)
    {
        LARGE_INTEGER li;
        li.QuadPart = offset;
        if (SetFilePointerEx(static_cast<HANDLE*>(handle), li, nullptr, static_cast<DWORD>(origin)) == INVALID_SET_FILE_POINTER)
        {
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Seek failed"};
        }
    }

    static HANDLE io_open(const std::filesystem::path::string_type::value_type* const filename, const DWORD access, const DWORD flags = FILE_ATTRIBUTE_NORMAL)
    {
        const auto handle {
            CreateFileW(
            filename,
            access,
            0,
            nullptr,
            CREATE_ALWAYS,
            flags,
            nullptr
        )};

        if (handle == INVALID_HANDLE_VALUE)
        {
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Open failed"};
        }
        
        return handle;
    }

    static void io_close(const HANDLE handle)
    {
        if (!CloseHandle(handle))
        {
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Close failed"};
        }
    }

    template<class CharT>
    static void io_async_write(const HANDLE handle, async_io<CharT>& aio, std::size_t size, std::size_t offset)
    {
        auto data {aio.buffer};
        size *= sizeof(CharT);
        std::size_t chunk_idx {0};

        while (size)
        {
            const auto chunk {static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()))};
            auto& overlap {aio.ov[chunk_idx++]};
            overlap = {};
            overlap.Offset     = static_cast<DWORD>(offset & std::numeric_limits<DWORD>::max());
            overlap.OffsetHigh = static_cast<DWORD>((offset >> 32) & std::numeric_limits<DWORD>::max());
            if (!WriteFile(handle, data, chunk, nullptr, &overlap) && GetLastError() != ERROR_IO_PENDING)
            {
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Write async failed"};
            }

            data += chunk / sizeof(CharT);
            size -= chunk;
            offset += chunk;
        }
    }

    template<class CharT>
    static bool io_async_done(const HANDLE handle, async_io<CharT>& aio, const bool wait)
    {
        for (auto& ov : aio.ov)
        {
            DWORD bytesTransferred {0};
            if (!GetOverlappedResult(handle, &ov, &bytesTransferred, wait))
            {
                if (GetLastError() == ERROR_IO_INCOMPLETE)
                {
                    return false;
                }

                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Write async failed"};
            }
        }

        return true;
    }

    template<class CharT>
    static void io_async_add_pending(async_io<CharT>*& free_ios, async_io<CharT>*& pending_ios) noexcept
    {
        const auto temp {std::exchange(free_ios, free_ios->next)};
        temp->next = pending_ios;
        pending_ios = temp;
    }

    template<class CharT>
    static async_io<CharT>& io_async_get(const HANDLE handle, const std::size_t size, async_io<CharT>*& free_ios, async_io<CharT>*& pending_ios)
    {
        // First, clean up any completed IOs
        for (auto it {pending_ios}, prev {(async_io<CharT>*) nullptr}; it;)
        {
            const auto next {it->next};
            if (io_async_done(handle, *it, false))
            {
                if (prev) prev->next = next;
                else      pending_ios = next;
                
                it->next = free_ios;
                free_ios = it;
            }
            else
            {
                prev = it;
            }
            it = next;
        }

        // Next, see if we have a free IO that is large enough
        for (auto it {free_ios}, prev {(async_io<CharT>*) nullptr}; it; prev = it, it = it->next)
        {
            if (it->capacity >= size)
            {
                if (prev)
                {
                    prev->next = it->next;
                    it->next = free_ios;
                    free_ios = it;
                }
                return *free_ios;
            }
        }

        // Finally, allocate a new IO
        const auto aio {async_io<CharT>::create(size)};
        aio->next = free_ios;
        free_ios = aio;
        return *aio;
    }

    template<class CharT>
    void basic_ofstream<CharT>::open(const std::filesystem::path::string_type::value_type* const filename, const openmode mode)
    {
        close();

        if (mode & openmode::in)
        {
            throw std::invalid_argument{"File must be opened with out mode"};
        }

        handle = io_open(filename, GENERIC_WRITE);
    }

    template<class CharT>
    void basic_ofstream<CharT>::close()
    {
        if (is_open())
        {
            flush();
            io_close(handle);
            handle = nullptr;
        }
    }

    template<class CharT>
    void basic_ofstream<CharT>::flush()
    {
        if (is_open())
        {
            if (const auto written {this->amount_written()}; written)
            {
                io_write(handle, this->pbeg(), written);
                this->setp(this->pbeg(), this->pend());
            }
        }
    }

    template<class CharT>
    void basic_ofstream<CharT>::write(const CharT* s, size_type count)
    {
        if (!is_open())
        {
            throw std::logic_error{"File not open"};
        }

        if (!buf.tag())
        {
            setbuf(nullptr, default_buffer_size);
        }

        if (this->is_buffered())
        {
            // If we have stuff in our buffer, write
            // what we can to that buffer and flush.
            if (this->amount_written())
            {
                const auto amount {std::min(count, this->write_available())};
                std::copy_n(s, amount, this->pcur());
                this->pbump(amount);
                count -= amount;
                s += amount;
                if (count)
                {
                    flush();
                }
            }

            // Check to see if remaining data can fit in the buffer.
            if (count && count <= this->write_available())
            {
                std::copy_n(s, count, this->pcur());
                this->pbump(count);
                return;
            }
        }

        if (count)
        {
            io_write(handle, s, count);
        }
    }

    template<class CharT>
    void basic_ofstream<CharT>::fill(const CharT& ch, size_type count)
    {
        if (!is_open())
        {
            throw std::logic_error{"File not open"};
        }

        if (!buf.tag())
        {
            setbuf(nullptr, default_buffer_size);
        }

        if (this->is_buffered())
        {
            while (count)
            {
                // If we have stuff in our buffer, write
                // what we can to that buffer and flush.
                if (auto amount {this->write_available()}; amount)
                {
                    amount = std::min(count, amount);
                    std::fill_n(this->pcur(), amount, ch);
                    this->pbump(amount);
                    count -= amount;
                }
                
                if (count)
                {
                    flush();
                }
            }
        }
        else
        {
            while (count--)
            {
                io_write(handle, &ch, 1);
            }
        }
    }

    template<class CharT>
    void basic_ofstream<CharT>::overflow(const CharT ch)
    {
        if (!is_open())
        {
            throw std::logic_error{"File not open"};
        }

        if (!buf.tag())
        {
            setbuf(nullptr, default_buffer_size);
        }

        if (this->is_buffered())
        {
            if (!this->write_available())
            {
                flush();
            }

            *this->pcur() = ch;
            this->pbump(1);
        }
        else
        {
            io_write(handle, &ch, 1);
        }
    }

    template<class CharT>
    void basic_ifstream<CharT>::open(const std::filesystem::path::string_type::value_type* const filename, const openmode mode)
    {
        if (mode & openmode::out)
        {
            throw std::invalid_argument{"File must be opened with in mode"};
        }

        handle = io_open(filename, GENERIC_READ);
    }

    template<class CharT>
    void basic_ifstream<CharT>::close()
    {
        if (is_open())
        {
            io_close(handle);
            handle = nullptr;
            this->setg(this->gbeg(), this->gbeg(), this->gbeg());
        }
    }

    template<class CharT>
    void basic_ifstream<CharT>::seek(const std::int64_t offset, const seek_mode origin)
    {
        io_seek(handle, offset, origin);
    }

    template<class CharT>
    basic_ifstream<CharT>::size_type basic_ifstream<CharT>::read(CharT* s, size_type count)
    {
        if (!is_open())
        {
            throw std::logic_error{"File not open"};
        }
        
        if (!buf_capacity)
        {
            setbuf(nullptr, default_buffer_size);
        }

        if (!this->is_buffered())
        {
            return io_read(handle, s, count);
        }

        std::size_t read_count {0};

        if (const auto avail {this->read_available()}; avail)
        {
            const auto amount {std::min(count, avail)};
            std::copy_n(this->gcur(), amount, s);
            this->gbump(amount);
            count -= amount;
            s += amount;
            read_count += amount;
        }

        // If the request is larger than our buffer capacity,
        // just read directly into the user buffer.
        if (count > buf_capacity)
        {
            read_count += io_read(handle, s, count);
        }
        else if (count)
        {
            underflow();
            std::copy_n(s, count, this->gcur());
            this->gbump(count);
            read_count += count;
        }

        return read_count;
    }

    template<class CharT>
    basic_ifstream<CharT>::opt_type basic_ifstream<CharT>::underflow()
    {
        if (!buf_capacity)
        {
            setbuf(nullptr, default_buffer_size);
        }

        if (!this->is_buffered())
        {
            CharT ch;
            if (io_read(handle, &ch, 1))
            {
                return ch;
            }
            return {};
        }

        const auto amt_read {io_read(handle, this->gbeg(), *buf_capacity)};
        this->setg(this->gbeg(), this->gbeg(), this->gbeg() + amt_read);
        if (amt_read)
        {
            return *(this->gcur());
        }
        
        return {};
    }

    template<class CharT>
    basic_async_ofstream<CharT>::basic_async_ofstream() noexcept
        : free_ios{nullptr}, pending_ios{nullptr}, offset{0}, default_buf_cap{default_buffer_size}, handle{nullptr} {}

    template<class CharT>
    basic_async_ofstream<CharT>::~basic_async_ofstream() noexcept
    {
        try
        {
            close();
        }
        catch (...) {}
        
        async_io<CharT>::destroy(free_ios);
    }

    template<class CharT>
    void basic_async_ofstream<CharT>::open(const std::filesystem::path::string_type::value_type* const filename, const openmode mode)
    {
        close();

        if (mode & openmode::in)
        {
            throw std::invalid_argument{"File must be opened with out mode"};
        }

        handle = io_open(filename, GENERIC_WRITE, FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL);
    }

    
    template<class CharT>
    void basic_async_ofstream<CharT>::close()
    {
        if (is_open())
        {
            flush();
            io_close(handle);
            handle = nullptr;
        }
    }

    template<class CharT>
    void basic_async_ofstream<CharT>::flush()
    {
        if (is_open())
        {
            if (const auto written {this->amount_written()}; written)
            {
                io_async_add_pending(free_ios, pending_ios);
                io_async_write(handle, *pending_ios, written, offset);
                offset += written;
                this->setp(nullptr, nullptr);
            }
            for (auto aio {pending_ios}; aio; aio = aio->next)
            {
                io_async_done(handle, *aio, true);
            }
            free_ios = std::exchange(pending_ios, nullptr);
        }
    }

    template<class CharT>
    void basic_async_ofstream<CharT>::write(const CharT* s, size_type count)
    {
        if (!is_open())
        {
            throw std::logic_error{"File not open"};
        }

        // If we have stuff in our buffer, write
        // what we can to that buffer and flush.
        if (this->amount_written())
        {
            const auto amount {std::min(count, this->write_available())};
            std::copy_n(s, amount, this->pcur());
            this->pbump(amount);
            count -= amount;
            s += amount;
            if (count)
            {
                io_async_add_pending(free_ios, pending_ios);
                io_async_write(handle, *pending_ios, pending_ios->capacity, offset);
                offset += pending_ios->capacity;
                this->setp(nullptr, nullptr);
            }
            else
            {
                return;
            }
        }

        auto& aio {io_async_get(handle, std::max(default_buf_cap, count), free_ios, pending_ios)};

        // Check to see if remaining data can fit in the buffer.
        if (count != aio.capacity)
        {
            this->setp(aio.buffer, aio.buffer + aio.capacity);
            std::copy_n(s, count, this->pcur());
            this->pbump(count);
        }
        else
        {
            std::copy_n(s, aio.capacity, aio.buffer);
            io_async_add_pending(free_ios, pending_ios);
            io_async_write(handle, aio, aio.capacity, offset);
            offset += aio.capacity;
        }
    }

    template<class CharT>
    void basic_async_ofstream<CharT>::fill(const CharT& ch, size_type count)
    {
        if (!is_open())
        {
            throw std::logic_error{"File not open"};
        }

        // If we have stuff in our buffer, write
        // what we can to that buffer and flush.
        if (this->amount_written())
        {
            const auto amount {std::min(count, this->write_available())};
            std::fill_n(this->pcur(), amount, ch);
            this->pbump(amount);
            count -= amount;
            if (count)
            {
                io_async_add_pending(free_ios, pending_ios);
                io_async_write(handle, *pending_ios, pending_ios->capacity, offset);
                offset += pending_ios->capacity;
                this->setp(nullptr, nullptr);
            }
            else
            {
                return;
            }
        }

        auto& aio {io_async_get(handle, std::max(default_buf_cap, count), free_ios, pending_ios)};

        // Check to see if remaining data can fit in the buffer.
        if (count != aio.capacity)
        {
            this->setp(aio.buffer, aio.buffer + aio.capacity);
            std::fill_n(this->pcur(), count, ch);
            this->pbump(count);
        }
        else
        {
            std::fill_n(aio.buffer, count, ch);
            io_async_add_pending(free_ios, pending_ios);
            io_async_write(handle, aio, aio.capacity, offset);
            offset += aio.capacity;
        }
    }

    template<class CharT>
    void basic_async_ofstream<CharT>::overflow(const CharT ch)
    {
        if (!this->write_available())
        {
            if (this->amount_written())
            {
                io_async_add_pending(free_ios, pending_ios);
                io_async_write(handle, *pending_ios, pending_ios->capacity, offset);
                offset += pending_ios->capacity;
                this->setp(nullptr, nullptr);
            }

            auto& aio {io_async_get(handle, default_buf_cap, free_ios, pending_ios)};
            this->setp(aio.buffer, aio.buffer + aio.capacity);
        }
        
        *this->pcur() = ch;
        this->pbump(1);
    }

    template<class CharT>
    class basic_stdostream final : public basic_ostream<CharT>
    {
    public:
        using size_type = typename basic_ostream<CharT>::size_type;

        basic_stdostream(const HANDLE handle) noexcept
            : handle{handle}, cur{buf} {}

        ~basic_stdostream() noexcept
        {
            try
            {
                flush();
            }
            catch (...) {}
        }

        void write(const CharT* s, size_type count) override
        {
            const auto og_s {s};
            const auto og_count {count};

            // If we have stuff in our buffer, write
            // what we can to that buffer and flush.
            if (amount_written())
            {
                const auto amount {std::min(count, write_available())};
                cur = std::copy_n(s, amount, cur);
                count -= amount;
                s += amount;
                if (count)
                {
                    flush();
                }
            }

            if (count)
            {
                if (count <= write_available())
                {
                    cur = std::copy_n(s, count, cur);
                }
                else
                {
                    io_write(handle, s, count);
                }
            }

            if (std::char_traits<CharT>::find(og_s, og_count, '\n'))
            {
                flush();
            }
        }

        void fill(const CharT& ch, size_type count) override
        {
            while (count)
            {
                // If we have stuff in our buffer, write
                // what we can to that buffer and flush.
                if (auto amount {this->write_available()}; amount)
                {
                    amount = std::min(count, amount);
                    cur = std::fill_n(cur, amount, ch);
                    count -= amount;
                }
                
                if (count)
                {
                    flush();
                }
            }
            
            if (ch == '\n')
            {
                flush();
            }
        }

        void flush() override
        {
            if (const auto written {amount_written()}; written)
            {
                io_write(handle, buf, written);
                cur = buf;
            }
        }
        
        void overflow(const CharT ch) override
        {
            if (!this->write_available())
            {
                flush();
            }

            *cur++ = ch;
            if (ch == '\n')
            {
                flush();
            }
        }

        [[nodiscard]] constexpr size_type write_available() const noexcept
        {
            return static_cast<size_type>(std::end(buf) - cur);
        }

        [[nodiscard]] constexpr size_type amount_written() const noexcept
        {
            return static_cast<size_type>(cur - buf);
        }
    private:
        CharT buf[4096];
        HANDLE handle;
        CharT* cur;
    };

    template<class CharT>
    class basic_stdistream final : public basic_istream<CharT>
    {
    public:
        using opt_type = typename basic_istream<CharT>::opt_type;

        basic_stdistream(const HANDLE handle) noexcept
            : handle{handle}
        {
            this->setg(buf, buf, buf + sizeof(buf));
        }

        std::size_t read(CharT* s, std::size_t count) override
        {
            std::size_t read_count {std::min(count, this->read_available())};

            if (read_count)
            {
                std::copy_n(this->gcur(), read_count, s);
                this->gbump(read_count);
                count -= read_count;
                s += read_count;
            }

            // If the request is larger than our buffer capacity,
            // just read directly into the user buffer.

            if (count)
            {
                if (count >= sizeof(buf))
                {
                    read_count += io_read(handle, s, count);
                }
                else
                {
                    underflow();
                    std::copy_n(s, count, this->gcur());
                    this->gbump(count);
                    read_count += count;
                }
            }

            return read_count;
        }
    protected:
        opt_type underflow() override
        {
            const auto amount {io_read(handle, buf, sizeof(buf))};
            if (amount)
            {
                this->setg(buf, buf, buf + amount);
                return buf[0];
            }

            return {};
        }
    private:
        CharT buf[4096];
        HANDLE handle;
    };

    // For when GetStdHandle gives NULL or an invalid handle
    template<class CharT>
    class basic_null_stdostream final : public basic_ostream<CharT>
    {
    public:
        basic_null_stdostream(std::error_code ec) noexcept
            : ec{std::move(ec)} {}
    protected:
        void overflow(const CharT) override
        {
            throw std::system_error{ec};
        }
    private:
        std::error_code ec;
    };

    template<class CharT>
    class basic_null_stdistream final : public basic_istream<CharT>
    {
    public:
        using opt_type = typename basic_istream<CharT>::opt_type;

        basic_null_stdistream(std::error_code ec) noexcept
            : ec{std::move(ec)} {}
    protected:
        opt_type underflow() override
        {
            throw std::system_error{ec};
        }
    private:
        std::error_code ec;
    };

    struct io_init
    {
        io_init() noexcept
        {
            const auto null_err {std::make_error_code(std::errc::no_such_device)};
            
            const auto out_handle {GetStdHandle(STD_OUTPUT_HANDLE)};

            if (!out_handle)
            {
                cout_.emplace<basic_null_stdostream<char>>(null_err);
                wcout_.emplace<basic_null_stdostream<wchar_t>>(null_err);
            }
            else if (out_handle == INVALID_HANDLE_VALUE)
            {
                const auto win_err {static_cast<int>(GetLastError())};
                cout_.emplace<basic_null_stdostream<char>>(std::error_code(win_err, std::system_category()));
                wcout_.emplace<basic_null_stdostream<wchar_t>>(std::error_code(win_err, std::system_category()));
            }
            else
            {
                cout_.emplace<basic_stdostream<char>>(out_handle);
                wcout_.emplace<basic_stdostream<wchar_t>>(out_handle);
            }

            const auto err_handle {GetStdHandle(STD_ERROR_HANDLE)};

            if (!err_handle)
            {
                cerr_.emplace<basic_null_stdostream<char>>(null_err);
                wcerr_.emplace<basic_null_stdostream<wchar_t>>(null_err);
            }
            else if (err_handle == INVALID_HANDLE_VALUE)
            {
                const auto win_err {static_cast<int>(GetLastError())};
                cerr_.emplace<basic_null_stdostream<char>>(std::error_code(win_err, std::system_category()));
                wcerr_.emplace<basic_null_stdostream<wchar_t>>(std::error_code(win_err, std::system_category()));
            }
            else
            {
                cerr_.emplace<basic_stdostream<char>>(err_handle);
                wcerr_.emplace<basic_stdostream<wchar_t>>(err_handle);
            }

            const auto in_handle {GetStdHandle(STD_INPUT_HANDLE)};

            if (!in_handle)
            {
                cin_.emplace<basic_null_stdistream<char>>(null_err);
                wcin_.emplace<basic_null_stdistream<wchar_t>>(null_err);
            }
            else if (in_handle == INVALID_HANDLE_VALUE)
            {
                const auto win_err {static_cast<int>(GetLastError())};
                cin_.emplace<basic_null_stdistream<char>>(std::error_code(win_err, std::system_category()));
                wcin_.emplace<basic_null_stdistream<wchar_t>>(std::error_code(win_err, std::system_category()));
            }
            else
            {
                cin_.emplace<basic_stdistream<char>>(in_handle);
                wcin_.emplace<basic_stdistream<wchar_t>>(in_handle);
            }
        }

        text_ostream& get_cout() noexcept
        {
            return *std::visit(overloaded {
                [](text_ostream& os) -> text_ostream* {
                    return &os;
                },
                [](std::monostate) -> text_ostream* {
                    return nullptr;
                }
            }, cout_);
        }

        wtext_ostream& get_wcout() noexcept
        {
            return *std::visit(overloaded {
                [](wtext_ostream& os) -> wtext_ostream* {
                    return &os;
                },
                [](std::monostate) -> wtext_ostream* {
                    return nullptr;
                }
            }, wcout_);
        }

        text_ostream& get_cerr() noexcept
        {
            return *std::visit(overloaded {
                [](text_ostream& os) -> text_ostream* {
                    return &os;
                },
                [](std::monostate) -> text_ostream* {
                    return nullptr;
                }
            }, cerr_);
        }

        wtext_ostream& get_wcerr() noexcept
        {
            return *std::visit(overloaded {
                [](wtext_ostream& os) -> wtext_ostream* {
                    return &os;
                },
                [](std::monostate) -> wtext_ostream* {
                    return nullptr;
                }
            }, wcerr_);
        }

        text_istream& get_cin() noexcept
        {
            return *std::visit(overloaded {
                [](text_istream& os) -> text_istream* {
                    return &os;
                },
                [](std::monostate) -> text_istream* {
                    return nullptr;
                }
            }, cin_);
        }

        wtext_istream& get_wcin() noexcept
        {
            return *std::visit(overloaded {
                [](wtext_istream& os) -> wtext_istream* {
                    return &os;
                },
                [](std::monostate) -> wtext_istream* {
                    return nullptr;
                }
            }, wcin_);
        }

        std::variant<
            std::monostate,
            basic_stdostream<char>,
            basic_null_stdostream<char>
        > cout_;

        std::variant<
            std::monostate,
            basic_stdostream<wchar_t>,
            basic_null_stdostream<wchar_t>
        > wcout_;

        std::variant<
            std::monostate,
            basic_stdostream<char>,
            basic_null_stdostream<char>
        > cerr_;

        std::variant<
            std::monostate,
            basic_stdostream<wchar_t>,
            basic_null_stdostream<wchar_t>
        > wcerr_;

        std::variant<
            std::monostate,
            basic_stdistream<char>,
            basic_null_stdistream<char>
        > cin_;

        std::variant<
            std::monostate,
            basic_stdistream<wchar_t>,
            basic_null_stdistream<wchar_t>
        > wcin_;
    };

    inline io_init init_;

    text_ostream& cout   {init_.get_cout()};
    wtext_ostream& wcout {init_.get_wcout()};

    text_ostream& cerr   {init_.get_cerr()};
    wtext_ostream& wcerr {init_.get_wcerr()};

    text_istream& cin   {init_.get_cin()};
    wtext_istream& wcin {init_.get_wcin()};

    template basic_ofstream<char>;
    template basic_ofstream<wchar_t>;
    template basic_ofstream<std::byte>;

    template basic_ifstream<char>;
    template basic_ifstream<wchar_t>;
    template basic_ifstream<std::byte>;

    template basic_async_ofstream<char>;
    template basic_async_ofstream<wchar_t>;
    template basic_async_ofstream<std::byte>;
}