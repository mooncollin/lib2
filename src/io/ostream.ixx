export module lib2.io:ostream;

import std;

import :character;

namespace lib2
{
    export
    template<class T>
    class basic_ostream
    {
    public:
        using value_type = T;
        using size_type  = std::size_t;
        using ssize_type = std::make_signed_t<size_type>;

        constexpr virtual ~basic_ostream() noexcept = default;

        constexpr void put(T val)
        {
            if (write_available())
            {
                *pcur_ = std::move(val);
                ++pcur_;
            }
            else
            {
                overflow(std::move(val));
            }
        }

        virtual constexpr void write(const T* vals, size_type count)
        {
            while (count > 0)
            {
                if (!write_available())
                {
                    overflow(*vals);
                    --count;
                    ++vals;
                    continue;
                }

                const auto amount {std::min(count, write_available())};
                std::copy_n(vals, amount, pcur_);
                pbump(amount);
                count -= amount;
                vals += amount;
            }
        }

        virtual constexpr void fill(const T& val, size_type count)
        {
            while (count > 0)
            {
                if (!write_available())
                {
                    overflow(val);
                    --count;
                    continue;
                }

                const auto amount {std::min(count, write_available())};
                std::fill_n(pcur_, amount, val);
                pbump(amount);
                count -= amount;
            }
        }

        virtual constexpr void flush() {}
    protected:
        constexpr basic_ostream() noexcept
            : pbeg_{nullptr}
            , pcur_{nullptr}
            , pend_{nullptr} {}

        constexpr basic_ostream(const basic_ostream&) noexcept = default;

        [[nodiscard]] constexpr T* pbeg() const noexcept
        {
            return pbeg_;
        }

        [[nodiscard]] constexpr T* pcur() const noexcept
        {
            return pcur_;
        }

        [[nodiscard]] constexpr T* pend() const noexcept
        {
            return pend_;
        }
        
        constexpr virtual void overflow(T)
        {
            throw std::out_of_range{"ostream::overflow"};
        }

        constexpr void pbump(ssize_type count) noexcept
        {
            pcur_ += count;
        }

        constexpr void setp(T* pb, T* pe) noexcept
        {
            pbeg_ = pb;
            pcur_ = pb;
            pend_ = pe;
        }

        constexpr void swap(basic_ostream& other) noexcept
        {
            std::swap(pbeg_, other.pbeg_);
            std::swap(pcur_, other.pcur_);
            std::swap(pend_, other.pend_);
        }
        
        [[nodiscard]] constexpr size_type write_available() const noexcept
        {
            return static_cast<size_type>(pend_ - pcur_);
        }
    private:
        T* pbeg_;
        T* pcur_;
        T* pend_;
    };

    export
    using byte_ostream = basic_ostream<std::byte>;

    export
    template<io_character CharT>
    using basic_text_ostream = basic_ostream<CharT>;

    export
    using text_ostream = basic_text_ostream<char>;

    export
    using wtext_ostream = basic_text_ostream<wchar_t>;

    export
	template<class T, class S = text_ostream>
	concept stream_writable =
		requires(const T& t, S& s)
	{
		{ s << t };
	};

    export
    template<class T>
    basic_ostream<T>& operator<<(basic_ostream<T>& os, T val)
    {
        os.put(std::move(val));
        return os;
    }

    export
    template<class CharT>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const CharT* s)
    {
        os.write(s, std::char_traits<CharT>::length(s));
        return os;
    }

    export
    template<class CharT, class Traits>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const std::basic_string_view<CharT, Traits> s)
    {
        os.write(s.data(), s.size());
        return os;
    }

    export
    template<class CharT, class Traits, class Allocator>
    basic_text_ostream<CharT>& operator<<(basic_text_ostream<CharT>& os, const std::basic_string<CharT, Traits, Allocator>& s)
    {
        os.write(s.data(), s.size());
        return os;
    }

    export
    byte_ostream& operator<<(byte_ostream&, const std::byte*) = delete;
    
    export
    template<class T, class R>
    R operator<<(basic_ostream<T>& os, R(*func)(basic_ostream<T>&))
    {
        return func(os);
    }

    export
    template<class T>
    basic_ostream<T>& flush(basic_ostream<T>& os)
    {
        os.flush();
        return os;
    }

    template<class T>
    struct io_fill_t
    {
        T fill;
        std::size_t count;
    };

    export
    template<class T>
    [[nodiscard]] constexpr io_fill_t<T> io_fill(T val, std::size_t count) noexcept
    {
        return {std::move(val), count};
    }

    export
    template<class T>
    basic_ostream<T>& operator<<(basic_ostream<T>& os, const io_fill_t<T>& op)
    {
        os.fill(op.fill, op.count);
        return os;
    }

    export
    template<class T>
    class ostream_iterator
    {
    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;

        constexpr ostream_iterator(basic_ostream<T>& s) noexcept
            : stream_{std::addressof(s)} {}

        ostream_iterator& operator=(T value)
        {
            stream_->put(std::move(value));
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

        [[nodiscard]] basic_ostream<T>& stream() noexcept
        {
            return *stream_;
        }
    private:
        basic_ostream<T>* stream_;
    };

    export
    template<class T>
    class size_ostream : public basic_ostream<T>
    {
    public:
        using value_type = typename basic_ostream<T>::value_type;
        using size_type  = typename basic_ostream<T>::size_type;
        using ssize_type = typename basic_ostream<T>::ssize_type;

        size_ostream() noexcept
            : size_{0} {}
        
        [[nodiscard]] std::size_t size() const noexcept
        {
            return size_;
        }

        void write(const T*, size_type count) override
        {
            size_ += count;
        }

        void fill(const T&, size_type count) override
        {
            size_ += count;
        }
    protected:
        void overflow(T) override
        {
            ++size_;
        }
    private:
        std::size_t size_;
    };
}