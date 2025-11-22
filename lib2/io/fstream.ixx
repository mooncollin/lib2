module;

#include <cstdio>

export module lib2.io:fstream;

import std;

import lib2.compact_optional;
import lib2.memory;

import :character;
import :ostream;
import :istream;

namespace lib2
{
    export
    enum class openmode : int
    {
        in          = 0x1,
        out         = 0x2,
        app         = 0x4,
        trunc       = 0x8,
        noreplace   = 0x10
    };

    export
    enum class seek_mode : int
    {
        set = SEEK_SET,
        cur = SEEK_CUR,
        end = SEEK_END
    };

    export
    constexpr bool operator&(const openmode lhs, const openmode rhs) noexcept
    {
        return std::to_underlying(lhs) & std::to_underlying(rhs);
    }

    export
    constexpr openmode operator|(const openmode lhs, const openmode rhs) noexcept
    {
        return static_cast<openmode>(std::to_underlying(lhs) | std::to_underlying(rhs));
    }

    export
    class ofstream final : public basic_ostream<char>
    {
    public:
        using size_type  = typename basic_ostream<char>::size_type;
        using ssize_type = typename basic_ostream<char>::ssize_type;

        ofstream() noexcept
            : handle{nullptr} {}

        explicit ofstream(const std::filesystem::path::string_type::value_type* const filename, const openmode mode = openmode::out)
            : ofstream{}
        {
            open(filename, mode);
        }

        explicit ofstream(const std::filesystem::path::string_type& filename, const openmode mode = openmode::out)
            : ofstream{filename.c_str(), mode} {}

        explicit ofstream(const std::filesystem::path& filename, const openmode mode = openmode::out)
            : ofstream{filename.native(), mode} {}

        ofstream(const ofstream&) = delete;
        ofstream(ofstream&& other) noexcept
            : basic_ostream<char>{std::move(other)}
            , buf{std::exchange(other.buf, nullptr)}
            , handle{std::exchange(other.handle, nullptr)} {}

        ofstream& operator=(const ofstream&) = delete;
        ofstream& operator=(ofstream&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                try
                {
                    close();
                }
                catch (...) {}

                basic_ostream<char>::operator=(std::move(other));
                buf = std::exchange(other.buf, nullptr);
                handle = std::exchange(other.handle, nullptr);
            }
            return *this;
        }

        virtual ~ofstream() noexcept
        {
            try
            {
                close();
            }
            catch (...) {}
            
            if (buf)
            {
                delete[] buf.ptr();
            }
        }

        void swap(ofstream& other) noexcept
        {
            using std::swap;
            basic_ostream<char>::swap(other);
            swap(buf, other.buf);
            swap(handle, other.handle);
        }
        
        [[nodiscard]] bool is_open() const noexcept
        {
            return static_cast<bool>(handle);
        }

        void open(const std::filesystem::path::string_type::value_type* const filename, const openmode mode = openmode::out);

        void open(const std::filesystem::path::string_type& filename, const openmode mode = openmode::out)
        {
            return open(filename.c_str(), mode);
        }

        void open(const std::filesystem::path& path, const openmode mode = openmode::out)
        {
            return open(path.native(), mode);
        }

        void close();

        void setbuf(char* s, size_type size)
        {
            if (s)
            {
                if (buf)
                {
                    delete[] buf.ptr();
                    buf = nullptr;
                }
            }
            else
            {
                if (size == 0)
                {
                    if (buf)
                    {
                        delete[] buf.ptr();
                        buf = nullptr;
                    }
                }
                else if (buf)
                {
                    if ((this->pend() - this->pbeg()) < size)
                    {
                        delete[] buf.ptr();
                        buf = new char[size];
                    }
                }
                else
                {
                    buf = new char[size];
                }
                s = buf.ptr();
            }

            this->setp(s, s + size);
            buf.tag(true);
        }
        
        void flush() override;
        void write(const char* s, size_type count) override;
        void fill(const char& ch, size_type count) override;
    protected:
        void overflow(char) override;
    private:
        tagged_pointer<char, 1> buf;
        void* handle;
    };

    export
    class ifstream final : public basic_istream<char>
    {
    public:
        using size_type          = typename basic_istream<char>::size_type;
        using ssize_type         = typename basic_istream<char>::ssize_type;
        using traits_type        = typename basic_istream<char>::traits_type;
        using opt_type           = typename basic_istream<char>::opt_type;

        ifstream() noexcept
            : handle{nullptr} {}

        explicit ifstream(const std::filesystem::path::string_type::value_type* const filename, const openmode mode = openmode::in)
            : ifstream{}
        {
            open(filename, mode);
        }

        explicit ifstream(const std::filesystem::path::string_type& filename, const openmode mode = openmode::in)
            : ifstream{filename.c_str(), mode} {}

        explicit ifstream(const std::filesystem::path& filename, const openmode mode = openmode::in)
            : ifstream{filename.native(), mode} {}

        ifstream(const ifstream&) = delete;
        ifstream(ifstream&& other) noexcept
            : basic_istream<char>{std::move(other)}
            , buf{std::move(other.buf)}
            , buf_capacity{std::exchange(other.buf_capacity, std::nullopt)}
            , handle{std::exchange(other.handle, nullptr)} {}

        ifstream& operator=(const ifstream&) = delete;
        ifstream& operator=(ifstream&& other) noexcept
        {
            close();
            basic_istream<char>::operator=(std::move(other));

            buf = std::move(other.buf);
            buf_capacity = std::exchange(other.buf_capacity, std::nullopt);
            handle = std::exchange(other.handle, nullptr);

            return *this;
        }

        void swap(ifstream& other) noexcept
        {
            using std::swap;

            basic_istream<char>::swap(other);
            swap(buf, other.buf);
            swap(handle, other.handle);
            swap(buf_capacity, other.buf_capacity);
        }
        
        [[nodiscard]] bool is_open() const noexcept
        {
            return static_cast<bool>(handle);
        }

        void open(const std::filesystem::path::string_type::value_type* const filename, const openmode mode = openmode::in);

        void open(const std::filesystem::path::string_type& filename, const openmode mode = openmode::out)
        {
            open(filename.c_str(), mode);
        }

        void open(const std::filesystem::path& path, const openmode mode = openmode::out)
        {
            open(path.native(), mode);
        }

        void close();

        void setbuf(char* const s, const size_type size)
        {
            if (s)
            {
                buf.reset();
                this->setg(s, s, s);
            }
            else
            {
                if (!buf_capacity || *buf_capacity < size)
                {
                    buf = std::make_unique<char[]>(std::max(size_type{1}, size));
                }
                this->setg(buf.get(), buf.get(), buf.get());
            }

            buf_capacity = size;
        }

        void seek(std::int64_t offset, seek_mode origin);
        
        void sync() override
        {
            const auto amount_read {this->gcur() - this->gbeg()};
            if (is_open() && amount_read)
            {
                seek(-amount_read, seek_mode::cur);
                this->setg(this->gbeg(), this->gbeg(), this->gbeg());
            }
        }

        size_type read(char* s, size_type count) override;
    protected:
        opt_type underflow() override;
    private:
        std::unique_ptr<char[]> buf;
        optional_size buf_capacity;
        void* handle;
    };

    struct async_io;

    export
    class async_ofstream final : public basic_ostream<char>
    {
    public:
        using size_type  = typename basic_ostream<char>::size_type;
        using ssize_type = typename basic_ostream<char>::ssize_type;

        async_ofstream() noexcept;

        explicit async_ofstream(const std::filesystem::path::string_type::value_type* const filename, const openmode mode = openmode::out)
            : async_ofstream{}
        {
            open(filename, mode);
        }

        explicit async_ofstream(const std::filesystem::path::string_type& filename, const openmode mode = openmode::out)
            : async_ofstream{filename.c_str(), mode} {}

        explicit async_ofstream(const std::filesystem::path& filename, const openmode mode = openmode::out)
            : async_ofstream{filename.native(), mode} {}

        async_ofstream(const async_ofstream&) = delete;
        async_ofstream(async_ofstream&& other) noexcept
            : basic_ostream<char>{std::move(other)}
            , pending_ios{std::move(other.pending_ios)}
            , free_ios{std::move(other.free_ios)}
            , offset{std::exchange(other.offset, 0)}
            , handle{std::exchange(other.handle, nullptr)} {}

        async_ofstream& operator=(const async_ofstream&) = delete;
        async_ofstream& operator=(async_ofstream&& other) noexcept
        {
            if (this != &other)
            {
                try
                {
                    close();
                }
                catch (...) {}

                basic_ostream<char>::operator=(std::move(other));
                pending_ios = std::move(other.pending_ios);
                free_ios = std::move(other.free_ios);
                offset = std::exchange(other.offset, 0);
                handle = std::exchange(other.handle, nullptr);
            }
            return *this;
        }

        virtual ~async_ofstream() noexcept;

        void swap(async_ofstream& other) noexcept
        {
            using std::swap;
            basic_ostream<char>::swap(other);
            swap(pending_ios, other.pending_ios);
            swap(free_ios, other.free_ios);
            swap(offset, other.offset);
            swap(handle, other.handle);
        }
        
        [[nodiscard]] bool is_open() const noexcept
        {
            return static_cast<bool>(handle);
        }

        void open(const std::filesystem::path::string_type::value_type* const filename, const openmode mode = openmode::out);

        void open(const std::filesystem::path::string_type& filename, const openmode mode = openmode::out)
        {
            return open(filename.c_str(), mode);
        }

        void open(const std::filesystem::path& path, const openmode mode = openmode::out)
        {
            return open(path.native(), mode);
        }

        void close();

        void setbuf(const std::size_t buf_size) noexcept
        {
            default_buf_cap = buf_size;
        }
        
        void flush() override;
        void write(const char* s, size_type count) override;
        void fill(const char& ch, size_type count) override;
    protected:
        void overflow(char) override;
    private:
        async_io* free_ios;
        async_io* pending_ios;
        std::size_t offset;
        std::size_t default_buf_cap;
        void* handle;
    };
    
    struct io_init
    {
        io_init() noexcept;

        lib2::text_ostream* cout_;
        lib2::text_ostream* cerr_;
        lib2::text_istream* cin_;
    };

    io_init& io_initializer() noexcept
    {
        static io_init instance;
        return instance;
    };

    export
    text_ostream& cout {*io_initializer().cout_};
    
    export
    text_ostream& cerr {*io_initializer().cerr_};

    export
    text_istream& cin {*io_initializer().cin_};
}