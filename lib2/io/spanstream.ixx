export module lib2.io:spanstream;

import std;

import :ostream;
import :istream;

namespace lib2
{
    export
    class ospanstream : public ostream
    {
    public:
        using value_type = ostream::value_type;
        using size_type  = ostream::size_type;
        using ssize_type = ostream::ssize_type;

        constexpr ospanstream() noexcept = default;

        explicit ospanstream(const std::span<std::byte> buf) noexcept
        {
            span(buf);
        }

        ospanstream(const ospanstream&) = delete;
        ospanstream(ospanstream&&) noexcept = default;

        ospanstream& operator=(const ospanstream&) = delete;
        ospanstream& operator=(ospanstream&&) noexcept = default;

        constexpr std::span<std::byte> span() const noexcept
        {
            return {this->pcur(), this->pend()};
        }

        constexpr void span(const std::span<std::byte> s) noexcept
        {
            this->setp(s.data(), s.data() + s.size());
        }
        
        constexpr void write(const std::byte* const vals, const size_type count) override
        {
            if (count > this->write_available())
            {
                throw std::out_of_range{"ospanstream::write"};
            }
            
            std::copy_n(vals, count, this->pcur());
            this->pbump(count);
        }

        constexpr void fill(const std::byte val, const size_type count) override
        {
            if (count > this->write_available())
            {
                throw std::out_of_range{"ospanstream::fill"};
            }
            
            std::fill_n(this->pcur(), count, val);
            this->pbump(count);
        }

        constexpr void swap(ospanstream& other) noexcept
        {
            ostream::swap(other);
        }
    };

    export
    class ispanstream : public istream
    {
    public:
        using value_type = istream::value_type;
        using size_type  = istream::size_type;
        using ssize_type = istream::ssize_type;
        using opt_type   = istream::opt_type;

        constexpr ispanstream() noexcept = default;

        explicit ispanstream(const std::span<const std::byte> buf) noexcept
        {
            span(buf);
        }

        ispanstream(const ispanstream&) = delete;
        ispanstream(ispanstream&&) noexcept = default;

        ispanstream& operator=(const ispanstream&) = delete;
        ispanstream& operator=(ispanstream&&) noexcept = default;

        constexpr std::span<const std::byte> span() const noexcept
        {
            return {this->gcur(), this->gend()};
        }

        constexpr void span(const std::span<const std::byte> s) noexcept
        {
            this->setg(const_cast<std::byte*>(s.data()), const_cast<std::byte*>(s.data()), const_cast<std::byte*>(s.data() + s.size()));
        }

        constexpr size_type read(ostream& os, size_type count) override
        {
            count = std::min(count, this->read_available());
            os.write(this->gcur(), count);
            this->gbump(count);
            return count;
        }

        constexpr size_type read(ostream& os, size_type count, const std::byte delim) override
        {
            count = std::min(count, this->read_available());
            const auto x {std::find(this->gcur(), this->gcur() + count, delim)};
            const auto read_count {static_cast<size_type>(x - this->gcur())};
            os.write(this->gcur(), read_count);
            this->gbump(read_count);
            return read_count;
        }

        constexpr size_type ignore(size_type count) override
        {
            count = std::min(count, this->read_available());
            this->gbump(count);
            return count;
        }

        constexpr size_type ignore(size_type count, const std::byte delim) override
        {
            count = std::min(count, this->read_available());
            const auto x {std::find(this->gcur(), this->gcur() + count, delim)};
            const auto read_count {static_cast<size_type>(x - this->gcur())};
            this->gbump(read_count);
            return read_count;
        }

        constexpr void swap(ispanstream& other) noexcept
        {
            istream::swap(other);
        }
    };
}