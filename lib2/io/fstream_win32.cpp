module;

#include <cstddef>
#include <windows.h>
#include <minwinbase.h>

module lib2.io;

import std;

import lib2.strings;
import lib2.utility;
import lib2.err;

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
        std::byte buffer[1];
        
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

    static void io_write(const HANDLE handle, const std::byte* data, std::size_t size)
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
                throw os_error{"Read console failed"};
            }

            data += read;
            size -= read;
            amount_read += read;

            if (std::find(data - read, data, L'\n'))
            {
                break;
            }
        }

        return amount_read;
    }

    static std::size_t io_read(const HANDLE handle, std::byte* data, std::size_t size)
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

    static HANDLE io_open(const std::filesystem::path::string_type::value_type* const filename, const DWORD access, const DWORD creation, const DWORD flags = FILE_ATTRIBUTE_NORMAL)
    {
        const auto handle {
            CreateFileW(
            filename,
            access,
            0,
            nullptr,
            creation,
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

        DWORD creation {CREATE_ALWAYS};

        handle = io_open(filename, GENERIC_WRITE, creation);
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
            if (const auto written {this->amount_written()})
            {
                io_write(handle, this->pbeg(), written);
                this->setp(this->pbeg(), this->pend());
            }
        }
    }

    void ofstream::write(const std::byte* s, size_type count)
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

    void ofstream::fill(const std::byte b, size_type count)
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
                    std::fill_n(this->pcur(), amount, b);
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
                io_write(handle, &b, 1);
            }
        }
    }

    void ofstream::overflow(const std::byte b)
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

            *this->pcur() = b;
            this->pbump(1);
        }
        else
        {
            io_write(handle, &b, 1);
        }
    }

    void ifstream::open(const std::filesystem::path::string_type::value_type* const filename, const openmode mode)
    {
        if (mode & openmode::out)
        {
            throw std::invalid_argument{"File must be opened with in mode"};
        }

        handle = io_open(filename, GENERIC_READ, OPEN_EXISTING);
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

    ifstream::size_type ifstream::read(ostream& os, size_type count)
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

        if (const auto avail {this->read_available()})
        {
            const auto amount {std::min(count, avail)};
            os.write(this->gcur(), amount);
            this->gbump(amount);
            count -= amount;
            read_count += amount;
        }

        // If the request is larger than our buffer capacity,
        // just read directly into the user buffer.
        if (count > buf_capacity)
        {
            // TODO
            // read_count += io_read(handle, s, count);
        }
        else if (count)
        {
            underflow();
            os.write(this->gcur(), count);
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

        DWORD creation {CREATE_NEW};

        if (mode & openmode::trunc)
        {
            creation = CREATE_ALWAYS;
        }

        handle = io_open(filename, GENERIC_WRITE, creation, FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL);
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

    void async_ofstream::write(const std::byte* s, size_type count)
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

    void async_ofstream::fill(const std::byte b, size_type count)
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
            std::fill_n(this->pcur(), amount, b);
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
            std::fill_n(this->pcur(), count, b);
            this->pbump(count);
        }
        else
        {
            std::fill_n(aio.buffer, count, b);
            io_async_add_pending(free_ios, pending_ios);
            io_async_write(handle, aio, aio.capacity, offset);
            offset += aio.capacity;
        }
    }

    void async_ofstream::overflow(const std::byte b)
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
        
        *this->pcur() = b;
        this->pbump(1);
    }

    class whandle_out_stream final : public ostream
    {
    public:
        using size_type = typename ostream::size_type;

        whandle_out_stream(const HANDLE handle) noexcept
            : handle{handle}
        {
            this->setp(buf, buf + sizeof(buf));
        }

        ~whandle_out_stream() noexcept
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
                io_write(handle, this->pbeg(), written);
                this->setp(buf, buf + sizeof(buf));
            }
        }

        void write(const std::byte* s, size_type count) override
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
                    io_write(handle, s, count);
                }
            }
        }

        void fill(const std::byte b, size_type count) override
        {
            while (count)
            {
                // If we have stuff in our buffer, write
                // what we can to that buffer and flush.
                if (auto amount {this->write_available()})
                {
                    amount = std::min(count, amount);
                    std::fill_n(this->pcur(), amount, b);
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
        void overflow(const std::byte ch) override
        {
            flush();
            *this->pcur() = ch;
            this->pbump(1);
        }
    private:
        HANDLE handle;
        std::byte buf[4096];
    };

    class out_console_stream final : public ostream
    {
    public:
        using size_type = ostream::size_type;

        out_console_stream(const HANDLE handle) noexcept
            : handle{handle}, cur_{buf}, expand{true} {}

        ~out_console_stream() noexcept
        {
            try
            {
                flush();
            }
            catch (...) {}
        }

        void flush() override
        {
            if (const auto written {cur_ - buf})
            {
                io_write_console(handle, buf, written);
                cur_ = buf;
            }
        }

        void write(const std::byte* s, const size_type count) override
        {
            bool needs_flushing {false};

            const auto emit_wchar {[&](const wchar_t wc) {
                if (cur_ == std::end(buf))
                {
                    io_write_console(handle, buf, std::size(buf));
                    cur_ = buf;
                    needs_flushing = false;
                }
                *cur_++ = wc;
            }};

            const auto write_byte {[&](const auto b) {
                // Fast path: ASCII
                if (utf8_len == 0 && b < std::byte{0x80})
                {
                    emit_wchar(static_cast<wchar_t>(b));
                    return;
                }

                // Start of sequence
                if (utf8_len == 0)
                {
                    utf8_buf[0] = static_cast<char8_t>(b);
                    utf8_len = 1;

                    if ((b & std::byte{0xE0}) == std::byte{0xC0}) utf8_expected = 2;
                    else if ((b & std::byte{0xF0}) == std::byte{0xE0}) utf8_expected = 3;
                    else if ((b & std::byte{0xF8}) == std::byte{0xF0}) utf8_expected = 4;
                    else
                    {
                        // invalid start, emit replacement char
                        utf8_len = 0;
                        utf8_expected = 0;
                        emit_wchar(L'?');
                    }
                    return;
                }

                // Continuation byte
                utf8_buf[utf8_len++] = static_cast<char8_t>(b);

                if (utf8_len < utf8_expected)
                    return;

                // Decode
                std::uint32_t cp {0};

                if (utf8_expected == 2)
                    cp = ((utf8_buf[0] & 0x1F) << 6) |
                        (utf8_buf[1] & 0x3F);
                else if (utf8_expected == 3)
                    cp = ((utf8_buf[0] & 0x0F) << 12) |
                        ((utf8_buf[1] & 0x3F) << 6) |
                        (utf8_buf[2] & 0x3F);
                else
                    cp = ((utf8_buf[0] & 0x07) << 18) |
                        ((utf8_buf[1] & 0x3F) << 12) |
                        ((utf8_buf[2] & 0x3F) << 6) |
                        (utf8_buf[3] & 0x3F);

                utf8_len = 0;
                utf8_expected = 0;

                if (cp <= 0xFFFF)
                {
                    emit_wchar(static_cast<wchar_t>(cp));
                }
                else
                {
                    cp -= 0x10000;
                    emit_wchar(static_cast<wchar_t>(0xD800 + (cp >> 10)));
                    emit_wchar(static_cast<wchar_t>(0xDC00 + (cp & 0x3FF)));
                }
            }};

            const auto end {s + count};
            for (; s != end; ++s)
            {
                if (*s == std::byte{'\r'})
                {
                    expand = false;
                }
                else if (*s == std::byte{'\n'})
                {
                    if (expand)
                    {
                        write_byte(std::byte{'\r'});
                    }
                    else
                    {
                        expand = true;
                    }
                    needs_flushing = true;
                }
                write_byte(*s);
            }

            if (needs_flushing)
            {
                flush();
            }
        }

        void fill(const std::byte b, size_type count) override
        {
            bool needs_flushing {false};
            const auto emit_wchar {[&](const wchar_t wc) {
                if (cur_ == std::end(buf))
                {
                    io_write_console(handle, buf, std::size(buf));
                    cur_ = buf;
                    needs_flushing = false;
                }
                *cur_++ = wc;
            }};

            // Resolve any expands
            if (count)
            {
                if (b == std::byte{'\r'})
                {
                    expand = false;
                }
                else if (b == std::byte{'\n'})
                {
                    if (expand)
                    {
                        emit_wchar(L'\r');
                    }
                    else
                    {
                        expand = true;
                    }
                    needs_flushing = true;
                }

                emit_wchar(static_cast<wchar_t>(b));
                --count;
            }

            if (b == std::byte{'\n'})
            {
                while (count--)
                {
                    emit_wchar(L'\r');
                    emit_wchar(L'\n');
                }
            }
            else
            {
                do
                {
                    if (cur_ == std::end(buf))
                    {
                        io_write_console(handle, buf, std::size(buf));
                        cur_ = buf;
                        needs_flushing = false;
                    }

                    if (const auto avail {std::min(count, static_cast<std::size_t>(std::end(buf) - cur_))})
                    {
                        cur_ = std::fill_n(cur_, avail, static_cast<wchar_t>(b));
                        count -= avail;
                    }

                    if (count)
                    {
                        io_write_console(handle, buf, std::size(buf));
                        cur_ = buf;
                    }
                }
                while (count);
            }

            utf8_len = 0;
            utf8_expected = 0;
        }
    protected:
        void overflow(const std::byte b) override
        {
            write(&b, 1);
        }
    private:
        HANDLE handle;
        wchar_t buf[4096];
        wchar_t* cur_;
        bool expand;

        char8_t utf8_buf[4];
        int utf8_len {0};
        int utf8_expected {0};
    };

    class whandle_in_stream final : public istream
    {
    public:
        using opt_type = typename istream::opt_type;

        whandle_in_stream(const HANDLE handle) noexcept
            : handle{handle} {}

        std::size_t read(ostream& os, std::size_t count) override
        {
            std::size_t read_count {std::min(count, this->read_available())};

            if (read_count)
            {
                os.write(this->gcur(), read_count);
                this->gbump(read_count);
                count -= read_count;
            }

            // If the request is larger than our buffer capacity,
            // just read directly into the user buffer.

            if (count)
            {
                if (count >= sizeof(buf))
                {
                    // TODO
                    // read_count += io_read(handle, s, count);
                }
                else
                {
                    underflow();
                    os.write(this->gcur(), count);
                    this->gbump(count);
                    read_count += count;
                }
            }

            return read_count;
        }
    protected:
        opt_type underflow() override
        {
            const auto amount {io_read(handle, buf, std::size(buf))};

            if (amount)
            {
                this->setg(buf, buf, buf + amount);
                return buf[0];
            }

            return {};
        }
    private:
        std::byte buf[4096];
        HANDLE handle;
    };

    class in_console_stream final : public istream
    {
    public:
        using opt_type  = istream::opt_type;
        using size_type = istream::size_type;

        in_console_stream(const HANDLE handle) noexcept
            : handle{handle}, cur{buf}, end{buf} {}

        size_type read(ostream& os, size_type count) override
        {
            size_type written {0};

            while (written < count)
            {
                // 1. Drain leftover UTF-8
                while (utf8_cur < utf8_len && written < count)
                {
                    os.put(std::byte{static_cast<unsigned char>(utf8_buf[utf8_cur++])});
                    ++written;
                }

                if (written == count)
                    break;

                utf8_cur = 0;
                utf8_len = 0;

                // 2. Need more UTF-16
                if (cur == end)
                {
                    const auto amount {io_read_console(handle, buf, std::size(buf))};
                    if (!amount)
                        break;

                    cur = buf;
                    end = buf + amount;
                }

                // 3. Decode one code point
                std::uint32_t cp {0};
                const auto wc {*cur++};

                if (pending_high)
                {
                    if (wc >= 0xDC00 && wc <= 0xDFFF)
                    {
                        cp = 0x10000 + (((pending_high - 0xD800) << 10) |
                                        (wc - 0xDC00));
                        pending_high = 0;
                    }
                    else
                    {
                        cp = 0xFFFD;
                        pending_high = 0;
                        --cur;
                    }
                }
                else if (wc >= 0xD800 && wc <= 0xDBFF)
                {
                    pending_high = wc;
                    continue;
                }
                else if (wc >= 0xDC00 && wc <= 0xDFFF)
                {
                    cp = 0xFFFD;
                }
                else
                {
                    cp = wc;
                }

                // 4. Encode into utf8_buf
                if (cp <= 0x7F)
                {
                    utf8_buf[0] = static_cast<char>(cp);
                    utf8_len = 1;
                }
                else if (cp <= 0x7FF)
                {
                    utf8_buf[0] = static_cast<char>(0xC0 | (cp >> 6));
                    utf8_buf[1] = static_cast<char>(0x80 | (cp & 0x3F));
                    utf8_len = 2;
                }
                else if (cp <= 0xFFFF)
                {
                    utf8_buf[0] = static_cast<char>(0xE0 | (cp >> 12));
                    utf8_buf[1] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                    utf8_buf[2] = static_cast<char>(0x80 | (cp & 0x3F));
                    utf8_len = 3;
                }
                else
                {
                    utf8_buf[0] = static_cast<char>(0xF0 | (cp >> 18));
                    utf8_buf[1] = static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
                    utf8_buf[2] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                    utf8_buf[3] = static_cast<char>(0x80 | (cp & 0x3F));
                    utf8_len = 4;
                }
            }

            return written;
        }
    protected:
        opt_type underflow() override
        {
            std::byte cb[1];
            ospanstream ss {cb};
            if (read(ss, 1))
            {
                return cb[0];
            }
            
            return {};
        }
    private:
        HANDLE handle;
        wchar_t buf[4096];
        wchar_t* cur;
        wchar_t* end;

        char8_t utf8_buf[8];
        std::size_t utf8_cur {0};
        std::size_t utf8_len {0};

        wchar_t pending_high {0};
    };

    // For when GetStdHandle gives NULL or an invalid handle
    class null_stdostream final : public ostream
    {
    public:
        null_stdostream(std::error_code ec) noexcept
            : ec{std::move(ec)} {}
    protected:
        void overflow(const std::byte) override
        {
            throw std::system_error{ec};
        }
    private:
        std::error_code ec;
    };

    class null_stdistream final : public istream
    {
    public:
        using opt_type = istream::opt_type;

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

    text_ostream init_cout() noexcept
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
            return null_cout;
        }
        
        if (is_console(out_handle))
        {
            static out_console_stream c_cout {out_handle};
            return c_cout;
        }

        static whandle_out_stream f_cout {out_handle};
        return f_cout;
    }

    text_ostream init_cerr() noexcept
    {
        std::error_code err;

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
            return null_cerr;
        }
        
        if (is_console(err_handle))
        {
            static out_console_stream c_cerr {err_handle};
            return c_cerr;
        }

        static whandle_out_stream f_cerr {err_handle};
        return f_cerr;
    }

    text_istream init_cin() noexcept
    {
        std::error_code err;

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
            return null_cin;
        }
       
        if (is_console(in_handle))
        {
            static in_console_stream c_cin {in_handle};
            return c_cin;
        }

        static whandle_in_stream f_cin {in_handle};
        return f_cin;
    }
}