module;

#include <cstdio>

export module lib2.io:fstream;

import std;

import :character;
import :ostream;
import :file;

namespace lib2
{
    export
    template<class CharT>
    class basic_ofstream : public basic_ostream<CharT>
    {
        static_assert(io_character<CharT> || std::same_as<CharT, std::byte>, "Invalid character type");
    public:
        using char_type = CharT;
        using size_type = typename basic_ostream<CharT>::size_type;
        using ssize_type = typename basic_ostream<CharT>::ssize_type;
        using native_handle_type = file::native_handle_type;

        basic_ofstream() noexcept
            : buf_set{false} {}

        explicit basic_ofstream(const std::filesystem::path::string_type::value_type* filename, openmode mode = openmode::out)
            : basic_ofstream{}
        {
            const auto err {open(filename, mode)};
            if (err)
            {
                throw std::system_error{err};
            }
        }

        explicit basic_ofstream(const std::filesystem::path::string_type& filename, openmode mode = openmode::out)
            : basic_ofstream{filename.c_str(), mode} {}

        explicit basic_ofstream(const std::filesystem::path& filename, openmode mode = openmode::out)
            : basic_ofstream{filename.native(), mode} {}

        basic_ofstream(const basic_ofstream&) = delete;
        basic_ofstream(basic_ofstream&& other) noexcept
            : basic_ostream<CharT>{std::move(other)}
            , buf{std::move(other.buf)}
            , buf_set{std::exchange(other.buf_set, false)}
            , f{std::move(other.f)} {}

        basic_ofstream& operator=(const basic_ofstream&) = delete;
        basic_ofstream& operator=(basic_ofstream&& other) noexcept
        {
            basic_ostream<CharT>::operator=(std::move(other));
            buf = std::move(other.buf);
            buf_set = std::exchange(other.buf_set, false);
            f = std::move(other.f);
            return *this;
        }

        virtual ~basic_ofstream() noexcept
        {
            try
            {
                flush();
            }
            catch (...) {}
        }

        void swap(basic_ofstream& other) noexcept
        {
            using std::swap;
            basic_ostream<CharT>::swap(other);
            swap(buf, other.buf);
            swap(buf_set, other.buf_set);
            swap(f, other.f);
        }

        [[nodiscard]] native_handle_type native_handle() const noexcept
        {
            return f.native_handle();
        }
        
        [[nodiscard]] bool is_open() const noexcept
        {
            return f.is_open();
        }

        std::error_code open(const std::filesystem::path::string_type::value_type* filename, openmode mode = openmode::out)
        {
            if (mode & openmode::in)
            {
                return std::make_error_code(std::errc::invalid_argument);
            }

            auto result {f.open(filename, mode | openmode::out)};
            if (!result)
            {
                return result.error();
            }

            result->setbuf();
            return {};
        }

        std::error_code open(const std::filesystem::path::string_type& filename, openmode mode = openmode::out)
        {
            return open(filename.c_str(), mode);
        }

        std::error_code open(const std::filesystem::path& path, openmode mode = openmode::out)
        {
            return open(path.native(), mode);
        }

        void close()
        {
            f.close();
        }

        void setbuf(CharT* s, size_type size)
        {
            if (s == nullptr)
            {
                if (size == 0)
                {
                    this->setp(nullptr, nullptr);
                    buf.reset();
                }
                else
                {
                    buf = std::make_unique<CharT[]>(size);
                    this->setp(buf.get(), buf.get() + size);
                }
            }
            else
            {
                this->setp(s, s + size);
                buf.reset();
            }

            buf_set = true;
        }
        
        void flush() override
        {
            if (is_open() && (this->pcur() != this->pbeg()))
            {
                unwrap().write(this->pbeg(), static_cast<std::size_t>(this->pcur() - this->pbeg()));
                this->setp(this->pbeg(), this->pend());
            }
        }

        void write(const CharT* s, size_type count) override
        {
            auto fref {unwrap()};
            ensure_buf();

            if (is_unbuffered())
            {
                fref.write(s, count);
            }
            else
            {
                // If we have stuff in our buffer, write
                // what we can to that buffer and flush.
                if (this->pcur() != this->pbeg())
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

                // Check to see if remaining data can fit
                // in the buffer. If not, do a direct write.
                if (count > this->write_available())
                {
                    fref.write(s, count);
                }
                else if (count)
                {
                    std::copy_n(s, count, this->pcur());
                    this->pbump(count);
                }
            }
        }
    protected:
        void overflow(CharT ch) override
        {
            ensure_buf();

            if (is_unbuffered())
            {
                unwrap().put(ch);
            }
            else
            {
                flush();
                this->put(ch);
            }
        }
    private:
        std::unique_ptr<CharT[]> buf;
        bool buf_set;
        file f;

        [[nodiscard]] bool is_unbuffered() const noexcept
        {
            return this->pbeg() == nullptr;
        }

        void ensure_buf()
        {
            if (!buf_set)
            {
                setbuf(nullptr, 4096);
            }
        }

        file_ref unwrap()
        {
            auto fr {f.ref()};
            if (!fr)
            {
                throw std::system_error{fr.error()};
            }

            return *fr;
        }
    };

    export
    using ofstream  = basic_ofstream<char>;

    export
    using wofstream = basic_ofstream<wchar_t>;

    export
    using bofstream = basic_ofstream<std::byte>;

    template<class CharT>
    class basic_stdofstream : public basic_ostream<CharT>
    {
    public:
        using size_type = typename basic_ostream<CharT>::size_type;
    
        basic_stdofstream(FILE* f) noexcept
            : f{*f} {}

        void write(const CharT* s, size_type count) override
        {
            f.write(s, count);
        }

        ~basic_stdofstream() noexcept
        {
            try
            {
                f.flush();
            }
            catch (...) {}
        }
    protected:
        void overflow(CharT ch) override
        {
            f.put(ch);
        }
    private:
        file_ref f;
    };

    basic_stdofstream<char> cout_impl {stdout};
    basic_stdofstream<wchar_t> wcout_impl {stdout};

    basic_stdofstream<char> cerr_impl {stderr};
    basic_stdofstream<wchar_t> wcerr_impl {stderr};

    export
    text_ostream& cout {cout_impl};
    
    export
    wtext_ostream& wcout {wcout_impl};
    
    export
    text_ostream& cerr {cerr_impl};

    export
    wtext_ostream& wcerr {wcerr_impl};
}