export module lib2.io:spanstream;

import std;

import :ostream;
import :istream;

namespace lib2
{
    export
    template<class T>
    class basic_ospanstream : public basic_ostream<T>
    {
    public:
        using value_type = typename basic_ostream<T>::value_type;
        using size_type  = typename basic_ostream<T>::size_type;
        using ssize_type = typename basic_ostream<T>::ssize_type;

        constexpr basic_ospanstream() noexcept = default;

        explicit basic_ospanstream(const std::span<T> buf) noexcept
        {
            span(buf);
        }

        basic_ospanstream(const basic_ospanstream&) = delete;
        basic_ospanstream(basic_ospanstream&&) noexcept = default;

        basic_ospanstream& operator=(const basic_ospanstream&) = delete;
        basic_ospanstream& operator=(basic_ospanstream&&) noexcept = default;

        constexpr std::span<T> span() const noexcept
        {
            return {this->pcur(), this->pend()};
        }

        constexpr void span(const std::span<T> s) noexcept
        {
            this->setp(s.data(), s.data() + s.size());
        }
        
        constexpr void write(const T* vals, size_type count) override
        {
            if (count > this->write_available())
            {
                throw std::out_of_range{"ospanstream::write"};
            }
            
            std::copy_n(vals, count, this->pcur());
            this->pbump(count);
        }

        constexpr void fill(const T& val, size_type count) override
        {
            if (count > this->write_available())
            {
                throw std::out_of_range{"ospanstream::write"};
            }
            
            std::fill_n(val, count, this->pcur());
            this->pbump(count);
        }

        constexpr void swap(basic_ospanstream& other) noexcept
        {
            basic_ostream<T>::swap(other);
        }
    };

    export
    using binary_ospanstream = basic_ospanstream<std::byte>;

    export
    using text_ospanstream = basic_ospanstream<char>;

    export
    template<class T>
    class basic_ispanstream : public basic_istream<T>
    {
    public:
        using value_type = typename basic_istream<T>::value_type;
        using size_type  = typename basic_istream<T>::size_type;
        using ssize_type = typename basic_istream<T>::ssize_type;

        constexpr basic_ispanstream() noexcept = default;

        explicit basic_ispanstream(const std::span<const T> buf) noexcept
        {
            span(buf);
        }

        basic_ispanstream(const basic_ispanstream&) = delete;
        basic_ispanstream(basic_ispanstream&&) noexcept = default;

        basic_ispanstream& operator=(const basic_ispanstream&) = delete;
        basic_ispanstream& operator=(basic_ispanstream&&) noexcept = default;

        constexpr std::span<const T> span() const noexcept
        {
            return {this->gcur(), this->gend()};
        }

        constexpr void span(const std::span<const T> s) noexcept
        {
            this->setg(s.data(), s.data(), s.data() + s.size());
        }

        constexpr size_type read(T* buf, size_type count) override
        {
            count = std::min(count, this->read_available());
            std::copy_n(this->gcur(), count, buf);
            return count;
        }

        constexpr void swap(basic_ispanstream& other) noexcept
        {
            basic_istream<T>::swap(other);
        }
    };

    export
    using binary_ispanstream = basic_ispanstream<std::byte>;

    export
    using text_ispanstream = basic_ispanstream<char>;
}