module;

#include <cstddef>
#include <windows.h>
#include <minwinbase.h>

module lib2.io;

import std;

import lib2.strings;
import lib2.utility;
import lib2.err;

import :fstream;

namespace lib2
{
    constexpr std::size_t DWORDS_PER_SIZE_T {(sizeof(std::size_t) * 8 + sizeof(DWORD) * 8 - 1) / (sizeof(DWORD) * 8)};
    constexpr std::size_t default_buffer_size {8192};

    struct async_io
    {
    private:
        async_io(const std::size_t buf_size) noexcept
            : next{nullptr}, capacity{buf_size} {}
    public:
        async_io* next;
        std::size_t capacity;
        OVERLAPPED ov[DWORDS_PER_SIZE_T] {};
        char buffer[1];
        
        static async_io* create(const std::size_t buf_size)
        {
            const auto total_size {sizeof(async_io) + (buf_size - 1)};
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

    static void io_write_console(const HANDLE handle, const wchar_t* data, std::size_t size)
    {
        while (size)
        {
            const auto chunk {static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()))};
            DWORD written {0};
            if (!WriteConsoleW(handle, data, chunk, &written, nullptr))
            {
                throw os_error{"Write console failed"};
            }

            data += written;
            size -= written;
        }
    }

    static void io_write(const HANDLE handle, const char* data, std::size_t size)
    {
        while (size)
        {
            const auto chunk {static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()))};
            DWORD written {0};
            if (!WriteFile(handle, data, chunk, &written, nullptr) || (written != chunk))
            {
                throw os_error{"Write failed"};
            }

            data += written;
            size -= written;
        }
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
                throw os_error{"Write console failed"};
            }

            data += read;
            size -= read;
            amount_read += read;

            if (std::char_traits<wchar_t>::find(data - read, read, L'\n'))
            {
                break;
            }
        }

        return amount_read;
    }

    static std::size_t io_read(const HANDLE handle, char* data, std::size_t size)
    {
        std::size_t total_read {0};
        while (size)
        {
            const auto chunk {static_cast<DWORD>(std::min<std::size_t>(size, std::numeric_limits<DWORD>::max()))};
            DWORD read {0};
            if (!ReadFile(handle, data, chunk, &read, nullptr))
            {
                throw os_error{"Read failed"};
            }

            if (!read)
            {
                break;
            }

            data += read;
            size -= read;
            total_read += read;
        }

        return total_read;
    }

    static void io_seek(const HANDLE handle, const std::int64_t offset, const seek_mode origin)
    {
        LARGE_INTEGER li;
        li.QuadPart = offset;
        if (SetFilePointerEx(static_cast<HANDLE*>(handle), li, nullptr, static_cast<DWORD>(origin)) == INVALID_SET_FILE_POINTER)
        {
            throw os_error{"Seek failed"};
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
            throw os_error{"Open failed"};
        }
        
        return handle;
    }

    static void io_close(const HANDLE handle)
    {
        if (!CloseHandle(handle))
        {
            throw os_error{"Close failed"};
        }
    }

    static void io_async_write(const HANDLE handle, async_io& aio, std::size_t size, std::size_t offset)
    {
        auto data {aio.buffer};
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
                throw os_error{"Write async failed"};
            }

            data += chunk;
            size -= chunk;
            offset += chunk;
        }
    }

    static bool io_async_done(const HANDLE handle, async_io& aio, const bool wait)
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

                throw os_error{"Write async failed"};
            }
        }

        return true;
    }

    static void io_async_add_pending(async_io*& free_ios, async_io*& pending_ios) noexcept
    {
        const auto temp {std::exchange(free_ios, free_ios->next)};
        temp->next = pending_ios;
        pending_ios = temp;
    }

    static async_io& io_async_get(const HANDLE handle, const std::size_t size, async_io*& free_ios, async_io*& pending_ios)
    {
        // First, clean up any completed IOs
        for (auto it {pending_ios}, prev {(async_io*) nullptr}; it;)
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
        for (auto it {free_ios}, prev {(async_io*) nullptr}; it; prev = it, it = it->next)
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
        const auto aio {async_io::create(size)};
        aio->next = free_ios;
        free_ios = aio;
        return *aio;
    }

    void ofstream::open(const std::filesystem::path::string_type::value_type* const filename, const openmode mode)
    {
        close();

        if (mode & openmode::in)
        {
            throw std::invalid_argument{"File must be opened with out mode"};
        }

        handle = io_open(filename, GENERIC_WRITE);
    }

    void ofstream::close()
    {
        if (is_open())
        {
            flush();
            io_close(handle);
            handle = nullptr;
        }
    }

    void ofstream::flush()
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

    void ofstream::write(const char* s, size_type count)
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

    void ofstream::fill(const char& ch, size_type count)
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

    void ofstream::overflow(const char ch)
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

    void ifstream::open(const std::filesystem::path::string_type::value_type* const filename, const openmode mode)
    {
        if (mode & openmode::out)
        {
            throw std::invalid_argument{"File must be opened with in mode"};
        }

        handle = io_open(filename, GENERIC_READ);
    }

    void ifstream::close()
    {
        if (is_open())
        {
            io_close(handle);
            handle = nullptr;
            this->setg(this->gbeg(), this->gbeg(), this->gbeg());
        }
    }

    void ifstream::seek(const std::int64_t offset, const seek_mode origin)
    {
        io_seek(handle, offset, origin);
    }

    ifstream::size_type ifstream::read(char* s, size_type count)
    {
        if (!is_open())
        {
            throw std::logic_error{"File not open"};
        }
        
        if (!buf_capacity)
        {
            setbuf(nullptr, default_buffer_size);
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

    ifstream::opt_type ifstream::underflow()
    {
        if (!buf_capacity)
        {
            setbuf(nullptr, default_buffer_size);
        }

        const auto amt_read {io_read(handle, this->gbeg(), *buf_capacity)};
        this->setg(this->gbeg(), this->gbeg(), this->gbeg() + amt_read);
        if (amt_read)
        {
            return *(this->gcur());
        }
        
        return {};
    }

    async_ofstream::async_ofstream() noexcept
        : free_ios{nullptr}, pending_ios{nullptr}, offset{0}, default_buf_cap{default_buffer_size}, handle{nullptr} {}

    async_ofstream::~async_ofstream() noexcept
    {
        try
        {
            close();
        }
        catch (...) {}
        
        async_io::destroy(free_ios);
    }

    void async_ofstream::open(const std::filesystem::path::string_type::value_type* const filename, const openmode mode)
    {
        close();

        if (mode & openmode::in)
        {
            throw std::invalid_argument{"File must be opened with out mode"};
        }

        handle = io_open(filename, GENERIC_WRITE, FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL);
    }

    void async_ofstream::close()
    {
        if (is_open())
        {
            flush();
            io_close(handle);
            handle = nullptr;
        }
    }

    void async_ofstream::flush()
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

    void async_ofstream::write(const char* s, size_type count)
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

    void async_ofstream::fill(const char& ch, size_type count)
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

    void async_ofstream::overflow(const char ch)
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
    class bare_out_stream final : public basic_ostream<CharT>
    {
    public:
        using size_type = typename basic_ostream<CharT>::size_type;

        bare_out_stream(const HANDLE handle) noexcept
            : handle{handle}
        {
            this->setp(buf, buf + sizeof(buf));
        }

        ~bare_out_stream() noexcept
        {
            try
            {
                flush();
            }
            catch (...) {}
        }

        void flush()
        {
            if (const auto written {this->amount_written()})
            {
                if constexpr (std::same_as<CharT, wchar_t>)
                {
                    io_write_console(handle, this->pbeg(), written);
                }
                else
                {
                    io_write(handle, this->pbeg(), written);
                }
                this->setp(buf, buf + sizeof(buf));
            }
        }

        void write(const CharT* s, size_type count) override
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

            if (count)
            {
                if (count <= this->write_available())
                {
                    std::copy_n(s, count, this->pcur());
                    this->pbump(count);
                }
                else
                {
                    if constexpr (std::same_as<CharT, wchar_t>)
                    {
                        io_write_console(handle, s, count);
                    }
                    else
                    {
                        io_write(handle, s, count);
                    }
                }
            }
        }

        void fill(const CharT& ch, size_type count) override
        {
            while (count)
            {
                // If we have stuff in our buffer, write
                // what we can to that buffer and flush.
                if (auto amount {this->write_available()})
                {
                    amount = std::min(count, amount);
                    std::fill_n(this->pcur(), amount, ch);
                    this->pbump(count);
                    count -= amount;
                }
                
                if (count)
                {
                    flush();
                }
            }
        }
    protected:
        void overflow(const CharT ch) override
        {
            flush();
            *this->pcur() = ch;
            this->pbump(1);
        }
    private:
        HANDLE handle;
        CharT buf[4096];
    };

    class out_console_stream final : public basic_ostream<char>
    {
    public:
        using size_type = basic_ostream<char>::size_type;

        out_console_stream(bare_out_stream<wchar_t>& stream) noexcept
            : console_stream{stream} {}

        void flush() override
        {
            console_stream.flush();
        }

        void write(const char* s, const size_type count) override
        {
            bool needs_flushing {false};
            const auto og_s {s};
            const auto end {s + count};
            auto pos {std::find(s, end, '\n')};
            while (pos != end)
            {
                needs_flushing = true;
                if (pos != og_s)
                {
                    if (*(s - 1) == '\r')
                    {
                        pos = std::find(pos + 1, end, '\n');
                        continue;
                    }
                }

                lib2::utf8_to_utf16(s, pos, lib2::ostream_iterator{console_stream});
                console_stream.write(L"\r\n", 2);
                s = pos + 1;
                pos = std::find(s, end, '\n');
            }

            if (s != end)
            {
                lib2::utf8_to_utf16(s, end, lib2::ostream_iterator{console_stream});
            }

            if (needs_flushing)
            {
                flush();
            }
        }

        void fill(const char& ch, size_type count) override
        {
            if (ch == '\n')
            {
                while (count--)
                {
                    console_stream.write(L"\r\n", 2);
                }
                flush();
            }
            else
            {
                console_stream.fill(ch, count);
            }
        }
    protected:
        void overflow(const char ch) override
        {
            if (ch == '\n')
            {
                console_stream.write(L"\r\n", 2);
                flush();
            }
            else
            {
                console_stream.put(ch);
            }
        }
    private:
        bare_out_stream<wchar_t>& console_stream;
    };

    template<class CharT>
    class bare_in_stream final : public basic_istream<CharT>
    {
    public:
        using opt_type = typename basic_istream<CharT>::opt_type;

        bare_in_stream(const HANDLE handle) noexcept
            : handle{handle} {}

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
                    if constexpr (std::same_as<CharT, wchar_t>)
                    {
                        read_count += io_read_console(handle, s, count);
                    }
                    else
                    {
                        read_count += io_read(handle, s, count);
                    }
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
            const auto amount {[this] {
                if constexpr (std::same_as<CharT, wchar_t>)
                {
                    return io_read_console(handle, buf, sizeof(buf));
                }
                else
                {
                    return io_read(handle, buf, sizeof(buf));
                }
            }()};

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

    class in_console_stream final : public basic_istream<char>
    {
    public:
        using opt_type  = typename basic_istream<char>::opt_type;
        using size_type = typename basic_istream<char>::size_type;

        in_console_stream(bare_in_stream<wchar_t>& stream) noexcept
            : console_stream{stream}
        {
            this->setg(buf, buf, buf);
        }

        size_type read(char* s, size_type count) override
        {
            const auto save {count};
            while (count)
            {
                if (const auto avail {this->read_available()})
                {
                    const auto amount {std::min(count, avail)};
                    s = std::copy_n(this->gcur(), amount, s);
                    this->gbump(amount);
                    count -= amount;
                }
                else if (const auto possible_code_points {count / 4})
                {
                    const auto limited_view {std::ranges::views::take(console_stream, possible_code_points)};
                    const auto og_s {s};
                    s = utf16_to_utf8(limited_view.begin(), limited_view.end(), s).out;
                    count -= s - og_s;
                }
                else
                {
                    underflow();
                }
            }

            return save - count;
        }
    protected:
        opt_type underflow() override
        {
            const auto ch {console_stream.bump()};
            if (!ch)
            {
                return {};
            }

            const wchar_t wch {*ch};

            const auto result {utf16_to_utf8(&wch, &wch + 1, buf)};
            this->setg(buf, buf, result.out);
            return buf[0];
        }
    private:
        char buf[4];
        bare_in_stream<wchar_t>& console_stream;
    };

    // For when GetStdHandle gives NULL or an invalid handle
    class null_stdostream final : public basic_ostream<char>
    {
    public:
        null_stdostream(std::error_code ec) noexcept
            : ec{std::move(ec)} {}
    protected:
        void overflow(const char) override
        {
            throw std::system_error{ec};
        }
    private:
        std::error_code ec;
    };

    class null_stdistream final : public basic_istream<char>
    {
    public:
        using opt_type = basic_istream<char>::opt_type;

        null_stdistream(std::error_code ec) noexcept
            : ec{std::move(ec)} {}
    protected:
        opt_type underflow() override
        {
            throw std::system_error{ec};
        }
    private:
        std::error_code ec;
    };

    io_init::io_init() noexcept
    {
        std::error_code err;
        
        const auto out_handle {GetStdHandle(STD_OUTPUT_HANDLE)};

        if (!out_handle)
        {
            err = std::make_error_code(std::errc::no_such_device);
        }
        else if (out_handle == INVALID_HANDLE_VALUE)
        {
            const auto win_err {static_cast<int>(GetLastError())};
            err = {win_err, std::system_category()};
        }
        
        if (err)
        {
            static null_stdostream null_cout {err};
            cout_  = &null_cout;
        }
        else if (is_console(out_handle))
        {
            static bare_out_stream<wchar_t> true_cout {out_handle};
            static out_console_stream c_cout {true_cout};
            cout_ = &c_cout;
        }
        else
        {
            static bare_out_stream<char> f_cout {out_handle};
            cout_ = &f_cout;
        }
        
        err = {};
        const auto err_handle {GetStdHandle(STD_ERROR_HANDLE)};

        if (!err_handle)
        {
            err = std::make_error_code(std::errc::no_such_device);
        }
        else if (err_handle == INVALID_HANDLE_VALUE)
        {
            const auto win_err {static_cast<int>(GetLastError())};
            err = {win_err, std::system_category()};
        }

        if (err)
        {
            static null_stdostream null_cerr {err};
            cerr_ = &null_cerr;
        }
        else if (is_console(err_handle))
        {
            static bare_out_stream<wchar_t> true_cerr {err_handle};
            static out_console_stream c_cerr {true_cerr};
            cerr_ = &c_cerr;
        }
        else
        {
            static bare_out_stream<char> f_cerr {err_handle};
            cerr_ = &f_cerr;
        }

        err = {};
        const auto in_handle {GetStdHandle(STD_INPUT_HANDLE)};

        if (!in_handle)
        {
            err = std::make_error_code(std::errc::no_such_device);
        }
        else if (in_handle == INVALID_HANDLE_VALUE)
        {
            const auto win_err {static_cast<int>(GetLastError())};
            err = {win_err, std::system_category()};
        }

        if (err)
        {
            static null_stdistream null_cin {err};
            cin_  = &null_cin;
        }
        else if (is_console(in_handle))
        {
            static bare_in_stream<wchar_t> true_in {in_handle};
            static in_console_stream c_cin {true_in};
            cin_  = &c_cin;
        }
        else
        {
            static bare_in_stream<char> f_cin {in_handle};
            cin_  = &f_cin;
        }
    }
}