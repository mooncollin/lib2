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
        binary      = 0x10,
        noreplace   = 0x20
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
    template<class CharT>
    class basic_ofstream final : public basic_ostream<CharT>
    {
        static_assert(io_character<CharT> || std::same_as<CharT, std::byte>, "Invalid character type");
    public:
        using char_type  = CharT;
        using size_type  = typename basic_ostream<CharT>::size_type;
        using ssize_type = typename basic_ostream<CharT>::ssize_type;

        basic_ofstream() noexcept
            : handle{nullptr} {}

        explicit basic_ofstream(const std::filesystem::path::string_type::value_type* const filename, const openmode mode = openmode::out)
            : basic_ofstream{}
        {
            open(filename, mode);
        }

        explicit basic_ofstream(const std::filesystem::path::string_type& filename, const openmode mode = openmode::out)
            : basic_ofstream{filename.c_str(), mode} {}

        explicit basic_ofstream(const std::filesystem::path& filename, const openmode mode = openmode::out)
            : basic_ofstream{filename.native(), mode} {}

        basic_ofstream(const basic_ofstream&) = delete;
        basic_ofstream(basic_ofstream&& other) noexcept
            : basic_ostream<CharT>{std::move(other)}
            , buf{std::exchange(other.buf, nullptr)}
            , handle{std::exchange(other.handle, nullptr)} {}

        basic_ofstream& operator=(const basic_ofstream&) = delete;
        basic_ofstream& operator=(basic_ofstream&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                try
                {
                    close();
                }
                catch (...) {}

                basic_ostream<CharT>::operator=(std::move(other));
                buf = std::exchange(other.buf, nullptr);
                handle = std::exchange(other.handle, nullptr);
            }
            return *this;
        }

        virtual ~basic_ofstream() noexcept
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

        void swap(basic_ofstream& other) noexcept
        {
            using std::swap;
            basic_ostream<CharT>::swap(other);
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

        void setbuf(CharT* const s, const size_type size)
        {
            if (!s)
            {
                if (buf)
                {
                    delete[] buf.ptr();
                }

                if (size == 0)
                {
                    this->setp(nullptr, nullptr);
                    buf = nullptr;
                }
                else
                {
                    buf = new CharT[size];
                    this->setp(buf.ptr(), buf.ptr() + size);
                }
            }
            else
            {
                this->setp(s, s + size);
                if (buf)
                {
                    delete[] buf.ptr();
                }
                buf = nullptr;
            }
            
            buf.tag(true);
        }
        
        void flush() override;
        void write(const CharT* s, size_type count) override;
        void fill(const CharT& ch, size_type count) override;
    private:
        tagged_pointer<CharT, 1> buf;
        void* handle;
    };

    export
    using ofstream  = basic_ofstream<char>;

    export
    using wofstream = basic_ofstream<wchar_t>;

    export
    using bofstream = basic_ofstream<std::byte>;

    export
    template<class CharT>
    class basic_ifstream final : public basic_istream<CharT>
    {
        static_assert(io_character<CharT> || std::same_as<CharT, std::byte>, "Invalid character type");
    public:
        using char_type          = CharT;
        using size_type          = typename basic_istream<CharT>::size_type;
        using ssize_type         = typename basic_istream<CharT>::ssize_type;
        using traits_type        = typename basic_istream<CharT>::traits_type;
        using opt_type           = typename basic_istream<CharT>::opt_type;

        basic_ifstream() noexcept
            : handle{nullptr} {}

        explicit basic_ifstream(const std::filesystem::path::string_type::value_type* const filename, const openmode mode = openmode::in)
            : basic_ifstream{}
        {
            open(filename, mode);
        }

        explicit basic_ifstream(const std::filesystem::path::string_type& filename, const openmode mode = openmode::in)
            : basic_ifstream{filename.c_str(), mode} {}

        explicit basic_ifstream(const std::filesystem::path& filename, const openmode mode = openmode::in)
            : basic_ifstream{filename.native(), mode} {}

        basic_ifstream(const basic_ifstream&) = delete;
        basic_ifstream(basic_ifstream&& other) noexcept
            : basic_istream<CharT>{std::move(other)}
            , buf{std::move(other.buf)}
            , buf_capacity{std::exchange(other.buf_capacity, std::nullopt)}
            , handle{std::exchange(other.handle, nullptr)} {}

        basic_ifstream& operator=(const basic_ifstream&) = delete;
        basic_ifstream& operator=(basic_ifstream&& other) noexcept
        {
            basic_istream<CharT>::operator=(std::move(other));
            buf = std::move(other.buf);
            buf_capacity = std::exchange(other.buf_capacity, std::nullopt);
            handle = std::exchange(other.handle, nullptr);
            return *this;
        }

        void swap(basic_ifstream& other) noexcept
        {
            using std::swap;

            basic_istream<CharT>::swap(other);
            swap(buf, other.buf);
            swap(buf_capacity, other.buf_capacity);
            swap(handle, other.handle);
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

        void setbuf(CharT* const s, const size_type size)
        {
            if (s)
            {
                buf.reset();
                this->setg(s, s, s);
            }
            else if (size)
            {
                buf = std::make_unique<CharT[]>(size);
                this->setg(buf.get(), buf.get(), buf.get());
            }
            else
            {
                buf.reset();
                this->setg(nullptr, nullptr, nullptr);
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

        size_type read(CharT* s, size_type count) override;
    protected:
        opt_type underflow() override;
    private:
        std::unique_ptr<CharT[]> buf;
        optional_size buf_capacity;
        void* handle;
    };

    export
    using ifstream  = basic_ifstream<char>;

    export
    using wifstream = basic_ifstream<wchar_t>;

    template<class CharT>
    struct async_io;

    export
    template<class CharT>
    class basic_async_ofstream final : public basic_ostream<CharT>
    {
        static_assert(io_character<CharT> || std::same_as<CharT, std::byte>, "Invalid character type");
    public:
        using char_type  = CharT;
        using size_type  = typename basic_ostream<CharT>::size_type;
        using ssize_type = typename basic_ostream<CharT>::ssize_type;

        basic_async_ofstream() noexcept;

        explicit basic_async_ofstream(const std::filesystem::path::string_type::value_type* const filename, const openmode mode = openmode::out)
            : basic_async_ofstream{}
        {
            open(filename, mode);
        }

        explicit basic_async_ofstream(const std::filesystem::path::string_type& filename, const openmode mode = openmode::out)
            : basic_async_ofstream{filename.c_str(), mode} {}

        explicit basic_async_ofstream(const std::filesystem::path& filename, const openmode mode = openmode::out)
            : basic_async_ofstream{filename.native(), mode} {}

        basic_async_ofstream(const basic_async_ofstream&) = delete;
        basic_async_ofstream(basic_async_ofstream&& other) noexcept
            : basic_ostream<CharT>{std::move(other)}
            , pending_ios{std::move(other.pending_ios)}
            , free_ios{std::move(other.free_ios)}
            , offset{std::exchange(other.offset, 0)}
            , handle{std::exchange(other.handle, nullptr)} {}

        basic_async_ofstream& operator=(const basic_async_ofstream&) = delete;
        basic_async_ofstream& operator=(basic_async_ofstream&& other) noexcept
        {
            if (this != &other)
            {
                try
                {
                    close();
                }
                catch (...) {}

                basic_ostream<CharT>::operator=(std::move(other));
                pending_ios = std::move(other.pending_ios);
                free_ios = std::move(other.free_ios);
                offset = std::exchange(other.offset, 0);
                handle = std::exchange(other.handle, nullptr);
            }
            return *this;
        }

        virtual ~basic_async_ofstream() noexcept;

        void swap(basic_async_ofstream& other) noexcept
        {
            using std::swap;
            basic_ostream<CharT>::swap(other);
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
        void write(const CharT* s, size_type count) override;
        void fill(const CharT& ch, size_type count) override;
    private:
        async_io<CharT>* free_ios;
        async_io<CharT>* pending_ios;
        std::size_t offset;
        std::size_t default_buf_cap;
        void* handle;
    };

    export
    using async_ofstream  = basic_async_ofstream<char>;

    export
    using async_wofstream = basic_async_ofstream<wchar_t>;

    export
    using async_bofstream = basic_async_ofstream<std::byte>;

    export
    extern inline text_ostream& cout;
    
    export
    extern inline text_wostream& wcout;
    
    export
    extern inline text_ostream& cerr;

    export
    extern inline text_wostream& wcerr;
}