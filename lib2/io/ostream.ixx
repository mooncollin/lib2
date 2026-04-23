module;

#include <cassert>

export module lib2.io:ostream;

import std;

import lib2.strings;

namespace lib2
{
    export
    class ostream
    {
    public:
        using value_type = std::byte;
        using size_type  = std::size_t;
        using ssize_type = std::ptrdiff_t;

        constexpr virtual ~ostream() noexcept = default;

        [[nodiscard]] constexpr size_type write_available() const noexcept
        {
            return static_cast<size_type>(pend_ - pcur_);
        }

        [[nodiscard]] constexpr size_type amount_written() const noexcept
        {
            return static_cast<size_type>(pcur_ - pbeg_);
        }

        constexpr void put(const std::byte val)
        {
            if (write_available())
            {
                *pcur_++ = val;
            }
            else
            {
                overflow(val);
            }
        }

        virtual constexpr void write(const std::byte* vals, size_type count)
        {
            while (count)
            {
                if (const auto avail {std::min(count, write_available())})
                {
                    pcur_ = std::copy_n(vals, avail, pcur_);
                    vals += avail;
                    count -= avail;
                }
                else
                {
                    overflow(*vals);
                    ++vals;
                    --count;
                }
            }
        }

        virtual constexpr void fill(const std::byte val, size_type count)
        {
            while (count)
            {
                if (const auto avail {std::min(count, write_available())})
                {
                    pcur_ = std::fill_n(pcur_, avail, val);
                    count -= avail;
                }
                else
                {
                    overflow(val);
                    --count;
                }
            }
        }

        virtual constexpr void flush() {}

        template<class F>
            requires(std::is_invocable_r_v<std::byte*, F, std::byte*, std::byte*>)
        constexpr size_type produce(F&& f) noexcept(std::is_nothrow_invocable_v<F, std::byte*, std::byte*>)
        {
            const auto old_pcur {std::exchange(pcur_, std::invoke(std::forward<F>(f), pcur_, pend_))};
            return static_cast<size_type>(pcur_ - old_pcur);
        }
    protected:
        constexpr ostream() noexcept
            : pbeg_{nullptr}
            , pcur_{nullptr}
            , pend_{nullptr} {}

        constexpr ostream(const ostream&) noexcept = default;

        [[nodiscard]] constexpr std::byte* pbeg() const noexcept
        {
            return pbeg_;
        }

        [[nodiscard]] constexpr std::byte* pcur() const noexcept
        {
            return pcur_;
        }

        [[nodiscard]] constexpr std::byte* pend() const noexcept
        {
            return pend_;
        }

        [[nodiscard]] constexpr bool is_buffered() const noexcept
        {
            return pbeg_ != pend_;
        }

        constexpr void pbump(const ssize_type count) noexcept
        {
            pcur_ += count;
        }

        constexpr void setp(std::byte* const pb, std::byte* const pe) noexcept
        {
            assert(pe >= pb);
            pbeg_ = pb;
            pcur_ = pb;
            pend_ = pe;
        }

        constexpr void swap(ostream& other) noexcept
        {
            std::swap(pbeg_, other.pbeg_);
            std::swap(pcur_, other.pcur_);
            std::swap(pend_, other.pend_);
        }

        virtual void overflow(std::byte)
        {
            throw std::logic_error{"Overflow not supported"};
        }
    private:
        std::byte* pbeg_;
        std::byte* pcur_;
        std::byte* pend_;
    };

    export
    class ostream_iterator
    {
    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;

        constexpr ostream_iterator(ostream& s) noexcept
            : stream_{std::addressof(s)} {}

        ostream_iterator& operator=(const std::byte value)
        {
            stream_->put(value);
            return *this;
        }

        ostream_iterator& operator*() noexcept
        {
            return *this;
        }

        ostream_iterator& operator++() noexcept
        {
            return *this;
        }

        ostream_iterator& operator++(int) noexcept
        {
            return *this;
        }

        [[nodiscard]] ostream& stream() noexcept
        {
            return *stream_;
        }
    private:
        ostream* stream_;
    };

    export
    class size_ostream final : public ostream
    {
    public:
        using value_type = ostream::value_type;
        using size_type  = ostream::size_type;
        using ssize_type = ostream::ssize_type;

        constexpr size_ostream() noexcept
            : size_{0} {}
        
        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return size_;
        }

        constexpr void write(const std::byte* const, const size_type count) noexcept override
        {
            size_ += count;
        }

        constexpr void fill(const std::byte, const size_type count) noexcept override
        {
            size_ += count;
        }
    protected:
        constexpr void overflow(const std::byte) override
        {
            ++size_;
        }
    private:
        std::size_t size_;
    };

    export
    struct text_ostream
    {
        ostream& stream;

        constexpr text_ostream(ostream& stream) noexcept
            : stream{stream} {}

        constexpr inline void put(const char ch)
        {
            stream.put(std::byte(ch));
        }

        constexpr inline void write(const char* const data, const ostream::size_type count)
        {
            stream.write(reinterpret_cast<const std::byte*>(data), count);
        }

        constexpr inline void write(const std::string_view str)
        {
            write(str.data(), str.size());
        }

        constexpr inline void fill(const char ch, const ostream::size_type count)
        {
            stream.fill(std::byte(ch), count);
        }

        template<class F>
            requires(std::is_invocable_r_v<char*, F, char*, char*>)
        constexpr inline ostream::size_type produce(F&& f) noexcept(std::is_nothrow_invocable_v<F, char*, char*>)
        {
            return stream.produce([&](const auto beg, const auto end) {
                return reinterpret_cast<char*>(std::invoke(std::forward<F>(f), reinterpret_cast<char*>(beg), reinterpret_cast<char*>(end)));
            });
        }
    };

    export
    class text_ostream_iterator
    {
    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;

        constexpr text_ostream_iterator(text_ostream s) noexcept
            : stream_{&(s.stream)} {}

        text_ostream_iterator& operator=(const char value)
        {
            stream_->put(std::byte(value));
            return *this;
        }

        text_ostream_iterator& operator*() noexcept
        {
            return *this;
        }

        text_ostream_iterator& operator++() noexcept
        {
            return *this;
        }

        text_ostream_iterator& operator++(int) noexcept
        {
            return *this;
        }
    private:
        ostream* stream_;
    };
}