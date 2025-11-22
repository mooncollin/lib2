module;

#include <cassert>

export module lib2.io:ostream;

import std;

import lib2.strings;

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
        using ssize_type = std::ptrdiff_t;

        constexpr virtual ~basic_ostream() noexcept = default;

        [[nodiscard]] constexpr size_type write_available() const noexcept
        {
            return static_cast<size_type>(pend_ - pcur_);
        }

        [[nodiscard]] constexpr size_type amount_written() const noexcept
        {
            return static_cast<size_type>(pcur_ - pbeg_);
        }

        constexpr void put(T val)
        {
            if (write_available())
            {
                *pcur_++ = std::move(val);
            }
            else
            {
                overflow(std::move(val));
            }
        }

        template<class F>
            requires(std::is_invocable_r_v<T*, Op, T*, T*>)
        constexpr size_type produce(F&& f)
        {
            const auto new_cur {std::invoke(std::forward<F>(f), pcur_, pend_)};
            const auto amount {static_cast<size_type>(new_cur - pcur_)};
            pcur_ = new_cur;
            return amount;
        }

        virtual constexpr void write(const T* vals, size_type count)
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

        virtual constexpr void fill(const T& val, size_type count)
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

        basic_ostream& operator<<(basic_ostream&(*func)(basic_ostream&))
        {
            return func(*this);
        }
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

        [[nodiscard]] constexpr bool is_buffered() const noexcept
        {
            return pbeg_ != pend_;
        }

        constexpr void pbump(const ssize_type count) noexcept
        {
            assert(write_available() >= count);
            pcur_ += count;
        }

        constexpr void setp(T* const pb, T* const pe) noexcept
        {
            assert(pe >= pb);
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

        virtual constexpr void overflow(T val)
        {
            throw std::logic_error{"Overflow not supported"};
        }
    private:
        T* pbeg_;
        T* pcur_;
        T* pend_;
    };

    export
    using byte_ostream = basic_ostream<std::byte>;

    export
    using text_ostream = basic_ostream<char>;

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
    template<std::ranges::contiguous_range R>
        requires(!character<std::remove_const_t<std::remove_pointer_t<std::decay_t<R>>>>)
    basic_ostream<std::ranges::range_value_t<R>>& operator<<(basic_ostream<std::ranges::range_value_t<R>>& os, R&& r)
    {
        os.write(std::ranges::data(r), std::ranges::size(r));
        return os;
    }

    export
    template<character CharT>
    basic_ostream<CharT>& operator<<(basic_ostream<CharT>& os, const CharT* const s)
    {
        os.write(s, std::char_traits<CharT>::length(s));
        return os;
    }

    export
    byte_ostream& operator<<(byte_ostream&, const std::byte*) = delete;

    export
    template<class T>
    basic_ostream<T>& flush(basic_ostream<T>& os)
    {
        os.flush();
        return os;
    }

    export
    template<class T>
    struct io_fill
    {
        T fill;
        typename basic_ostream<T>::size_type count;
    };

    export
    template<class T>
    io_fill(T, typename basic_ostream<T>::size_type) -> io_fill<T>;

    export
    template<class T>
    basic_ostream<T>& operator<<(basic_ostream<T>& os, const io_fill<T>& op)
    {
        os.fill(op.fill, op.count);
        return os;
    }

    template<character CharT>
    struct io_quoted
    {
        std::basic_string_view<CharT> str;
        CharT delim {'"'};
        CharT escape {'\\'};
        bool do_delims {true};
    };

    export
    template<character CharT>
    io_quoted(std::basic_string_view<CharT>, CharT = '"', CharT = '\\', bool = true) -> io_quoted<CharT>;

    export
    template<character CharT>
    io_quoted(const CharT*, CharT = '"', CharT = '\\', bool = true) -> io_quoted<CharT>;

    export
    template<character CharT>
    basic_ostream<CharT>& operator<<(basic_ostream<CharT>& os, io_quoted<CharT> q)
    {
        const CharT finds[] {q.escape, q.delim};

        if (q.do_delims)
        {
            os << q.delim;
        }

        std::size_t idx {q.str.find_first_of(finds, 0, sizeof(finds))};
        while (idx != std::string_view::npos)
        {
            os << q.str.substr(0, idx) << q.escape;
            q.str.remove_prefix(idx);
            idx = q.str.find_first_of(finds, 0, sizeof(finds));
        }
        
        os << q.str;
        
        if (q.do_delims)
        {
            os << q.delim;
        }

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
    class size_ostream final : public basic_ostream<T>
    {
    public:
        using value_type = typename basic_ostream<T>::value_type;
        using size_type  = typename basic_ostream<T>::size_type;
        using ssize_type = typename basic_ostream<T>::ssize_type;

        constexpr size_ostream() noexcept
            : size_{0} {}
        
        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return size_;
        }

        constexpr void write(const T* const, const size_type count) noexcept override
        {
            size_ += count;
        }

        constexpr void fill(const T&, const size_type count) noexcept override
        {
            size_ += count;
        }
    protected:
        constexpr void overflow(const T) override
        {
            ++size_;
        }
    private:
        std::size_t size_;
    };
}