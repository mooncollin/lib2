export module lib2.io:istream;

import std;

import lib2.concepts;
import lib2.strings;
import lib2.meta;

import :character;
import :stream_traits;

namespace lib2
{
    export
    template<class T>
    class istream_iterator;

    export
    template<class T>
    class basic_istream
    {
    public:
        using value_type    = T;
        using size_type     = std::size_t;
        using ssize_type    = std::make_signed_t<size_type>;
        using traits_type   = stream_traits<T>;
        using opt_type      = typename stream_traits<T>::opt_type;

        virtual ~basic_istream() noexcept = default;

        [[nodiscard]] constexpr size_type read_available() const noexcept
        {
            return static_cast<size_type>(gend_ - gcur_);
        }

        constexpr opt_type get()
        {
            if (read_available())
            {
                return *gcur_;
            }

            return underflow();
        }

        constexpr opt_type bump()
        {
            if (read_available())
            {
                return *gcur_++;
            }

            return uflow();
        }

        constexpr opt_type next()
        {
            if (bump())
            {
                return get();
            }

            return {};
        }

        constexpr virtual size_type read(T* buf, size_type count)
        {
            const auto save {count};
            while (count)
            {
                if (const auto avail {std::min(count, read_available())})
                {
                    buf = std::copy_n(gcur_, avail, buf);
                    gcur_ += avail;
                    count -= avail;
                }
                else if (auto val {uflow()})
                {
                    *buf++ = std::move(*val);
                    --count;
                }
                else
                {
                    break;
                }
            }

            return save - count;
        }

        constexpr virtual size_type read(T* buf, size_type count, const T& delim)
        {
            const auto save {count};
            while (count)
            {
                if (const auto avail {std::min(count, read_available())})
                {
                    const auto end {gcur_ + avail};
                    const auto x {std::find(gcur_, end, delim)};
                    const auto read_count {x - gcur_};
                    buf = std::copy(gcur_, x, buf);
                    gcur_  = x;
                    count -= read_count;
                    if (x != end)
                    {
                        break;
                    }
                }
                else if (auto val {underflow()})
                {
                    if (val == delim)
                    {
                        break;
                    }
                    *buf++ = std::move(*val);
                    gcur_ += read_available();
                    --count;
                }
                else
                {
                    break;
                }
            }

            return save - count;
        }

        constexpr virtual size_type ignore(size_type count)
        {
            if (count == std::numeric_limits<size_type>::max())
            {
                return ignore_unlimited();
            }

            const auto save {count};
            while (count)
            {
                if (const auto avail {std::min(count, read_available())})
                {
                    gcur_ += avail;
                    count -= avail;
                }
                else if (uflow())
                {
                    --count;
                }
                else
                {
                    break;
                }
            }

            return save - count;
        }

        constexpr virtual size_type ignore(size_type count, const T& delim)
        {
            if (count == std::numeric_limits<size_type>::max())
            {
                return ignore_unlimited(delim);
            }

            const auto save {count};
            while (count)
            {
                if (const auto avail {std::min(count, read_available())})
                {
                    const auto end {gcur_ + avail};
                    const auto x {std::find(gcur_, end, delim)};
                    count -= x - gcur_;
                    gcur_  = x;
                    
                    if (x != end)
                    {
                        --count;
                        ++gcur_;
                        break; 
                    }
                }
                else if (auto val {uflow()})
                {
                    --count;
                    if (val == delim)
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            return save - count;
        }

        constexpr virtual void sync() {}

        basic_istream& operator>>(basic_istream& (*func)(basic_istream&))
        {
            func(*this);
            return *this;
        }

        basic_istream& operator>>(basic_istream& (*func)(basic_istream&) noexcept) noexcept
        {
            func(*this);
            return *this;
        }

        istream_iterator<T> begin();
        static constexpr constexpr_value<std::default_sentinel> end{};

        template<class F>
            requires(std::is_invocable_r_v<const T*, F, const T*, const T*>)
        constexpr size_type consume(F&& f) noexcept(std::is_nothrow_invocable_v<F, const T*, const T*>)
        {
            const auto old_gcur {std::exchange(gcur_, std::invoke(std::forward<F>(f), gcur_, gend_))};
            return static_cast<size_type>(gcur_ - old_gcur);
        }
    protected:
        constexpr basic_istream() noexcept
            : gbeg_{nullptr}
            , gcur_{nullptr}
            , gend_{nullptr} {}

        constexpr basic_istream(const basic_istream&) noexcept = default;

        [[nodiscard]] constexpr T* gbeg() const noexcept
        {
            return gbeg_;
        }

        [[nodiscard]] constexpr T* gcur() const noexcept
        {
            return gcur_;
        }

        [[nodiscard]] constexpr T* gend() const noexcept
        {
            return gend_;
        }

        constexpr virtual opt_type underflow()
        {
            return {};
        }

        constexpr virtual opt_type uflow()
        {
            const auto val {underflow()};
            gcur_ += static_cast<bool>(read_available());
            return val;
        }

        constexpr void gbump(const ssize_type count) noexcept
        {
            gcur_ += count;
        }

        constexpr void setg(T* const gb, T* const gc, T* const ge) noexcept
        {
            gbeg_ = gb;
            gcur_ = gc;
            gend_ = ge;
        }

        constexpr void swap(basic_istream& other) noexcept
        {
            std::swap(gbeg_, other.gbeg_);
            std::swap(gcur_, other.gcur_);
            std::swap(gend_, other.gend_);
        }
    private:
        T* gbeg_;
        T* gcur_;
        T* gend_;

        constexpr size_type ignore_unlimited()
        {
            size_type count {0};
            do
            {
                count += read_available();
                gcur_ = gend_;
            }
            while (uflow() && ++count);

            return count;
        }

        constexpr size_type ignore_unlimited(const T& delim)
        {
            size_type count {0};
            while (true)
            {
                if (read_available())
                {
                    const auto x {std::find(gcur_, gend_, delim)};
                    count += x - gcur_;
                    gcur_ = x;
                    if (gcur_ != gend_)
                    {
                        ++gcur_;
                        ++count;
                        break;
                    }
                }
                else if (const auto val {uflow()})
                {
                    ++count;
                    if (val == delim)
                    {
                        break;
                    }
                }
            }

            return count;
        }
    };

    export
    using binary_istream = basic_istream<std::byte>;

    export
    using text_istream = basic_istream<char>;

    template<class T>
    class istream_iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
        using istream_type = basic_istream<T>;

        constexpr istream_iterator(const std::default_sentinel_t = {}) noexcept(std::is_nothrow_default_constructible_v<T>)
            : value{}, stream{nullptr} {}

        constexpr istream_iterator(istream_type& stream)
            : value{}, stream{std::addressof(stream)}
        {
            if (auto val {stream.get()})
            {
                value = std::move(*val);
            }
            else
            {
                this->stream = nullptr;
            }
        }

        constexpr istream_iterator(const istream_iterator& other) noexcept = default;

        constexpr const T& operator*() const
        {
            return value;
        }

        constexpr const T* operator->() const
        {
            return std::addressof(value);
        }

        constexpr istream_iterator& operator++()
        {
            if (auto val {stream->next()})
            {
                value = std::move(*val);
            }
            else
            {
                stream = nullptr;
            }

            return *this;
        }

        constexpr istream_iterator operator++(int)
        {
            auto temp {*this};
            ++*this;
            return temp;
        }

        constexpr bool operator==(const istream_iterator& rhs) const noexcept
        {
            return stream == rhs.stream;
        }
        constexpr bool operator!=(const istream_iterator&) const noexcept = default;

        constexpr bool operator==(const std::default_sentinel_t) const noexcept
        {
            return !stream;
        }
    private:
        T value;
        istream_type* stream;
    };

    template<class T>
    istream_iterator<T> basic_istream<T>::begin()
    {
        return {*this};
    }

    export
    text_istream& ws(text_istream& is)
    {
        while (is.get())
        {
            is.consume([](const auto begin, const auto end) {
                return std::find_if_not(begin, end, is_ascii_space);
            });
            if (is.read_available())
            {
                break;
            }
        }

        return is;
    }

    export
    template<class T>
    bool operator>>(basic_istream<T>& is, T& t)
    {
        if (auto val {is.get()})
        {
            t = std::move(*val);
            is.bump();
            return true;
        }

        return false;
    }

    export
    bool operator>>(text_istream& is, std::string& str)
    {
        str.clear();
        is >> ws;

        while (is.get())
        {
            is.consume([&](const auto begin, auto end) {
                end = std::find_if(begin, end, is_ascii_space);
                str.append(begin, end);
                return end;
            });
            if (is.read_available())
            {
                break;
            }
        }

        return !str.empty();
    }
}