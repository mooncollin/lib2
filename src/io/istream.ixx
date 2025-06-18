export module lib2.io:istream;

import std;

import :character;
import :stream_traits;

namespace lib2
{
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
            switch (gend_ - gcur_)
            {
            case 0:
                uflow();
                return get();
            case 1:
                return underflow();
            default:
                return ++*gcur_;
            }
        }

        constexpr virtual size_type read(T* buf, size_type count)
        {
            const auto save {count};
            while (count)
            {
                if (const auto avail {read_available()})
                {
                    const auto amount {std::min(count, avail)};
                    buf = std::copy_n(gcur_, amount, buf);
                    gbump(amount);
                    count -= amount;
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

        constexpr size_type read(std::span<T> buf)
        {
            return read(buf.data(), buf.size());
        }

        constexpr virtual size_type read(T* buf, size_type count, const T& delim)
        {
            const auto save {count};
            while (count)
            {
                if (const auto avail {read_available()})
                {
                    const auto to_read {std::min(count, avail)};
                    const auto end {gcur_ + to_read};
                    const auto x {std::find(gcur_, end, delim)};
                    const std::size_t read_count = x - gcur_;
                    buf = std::copy_n(gcur_, read_count, buf);
                    gbump(read_count);
                    count -= read_count;
                    if (x != end)
                    {
                        --count;
                        gbump(1);
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
                    *buf++ = std::move(*val);
                }
                else
                {
                    break;
                }
            }

            return save - count;
        }

        constexpr size_type read(std::span<T> buf, const T& delim)
        {
            return read(buf.data(), buf.size(), delim);
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
                if (const auto avail {read_available()})
                {
                    const auto to_read {std::min(count, avail)};
                    gbump(to_read);
                    count -= to_read;
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
                if (const auto avail {read_available()})
                {
                    const auto to_read {std::min(count, avail)};
                    const auto end {gcur_ + to_read};
                    const auto x {std::find(gcur_, end, delim)};
                    const std::size_t read_count = x - gcur_;
                    gbump(read_count);
                    count -= read_count;
                    
                    if (x != end)
                    {
                        --count;
                        gbump(1);
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
            auto val {underflow()};
            gcur_ += static_cast<bool>(read_available());
            return val;
        }

        constexpr void gbump(ssize_type count) noexcept
        {
            gcur_ += count;
        }

        constexpr void setg(T* gb, T* gc, T* ge) noexcept
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

        [[nodiscard]] constexpr size_type read_available() const noexcept
        {
            return static_cast<size_type>(gend_ - gcur_);
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
            while (underflow());

            return count;
        }

        constexpr size_type ignore_unlimited(const T& delim)
        {
            size_type count {0};
            do
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
            while (underflow());

            return count;
        }
    };

    export
    using binary_istream = basic_istream<std::byte>;

    export
    template<io_character CharT>
    using basic_text_istream = basic_istream<CharT>;

    export
    using text_istream = basic_text_istream<char>;

    export
    using wtext_istream = basic_text_istream<wchar_t>;
}