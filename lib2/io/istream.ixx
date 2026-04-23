export module lib2.io:istream;

import std;

import lib2.concepts;
import lib2.strings;
import lib2.meta;
import lib2.compact_optional;

import :ostream;

namespace lib2
{
    export
    class istream_iterator;

    export
    class text_istream_iterator;

    export
    class opt_byte : private compact_optional<int, -1>
    {
        using base = compact_optional<int, -1>;
        friend class opt_char;
    public:
        constexpr opt_byte() noexcept = default;
        constexpr opt_byte(std::nullopt_t) noexcept
            : base{std::nullopt} {}

        constexpr opt_byte(const std::byte value) noexcept
            : base{static_cast<int>(value)} {}

        constexpr opt_byte(const opt_byte&) noexcept = default;

        using base::has_value;
        
        constexpr explicit operator bool() const noexcept
        {
            return has_value();
        }

        constexpr std::byte operator*() const noexcept
        {
            return static_cast<std::byte>(base::operator*());
        }

        constexpr bool operator==(const opt_byte&) const noexcept = default;
        constexpr bool operator!=(const opt_byte&) const noexcept = default;

        constexpr bool operator==(const std::byte value) const noexcept
        {
            return static_cast<int>(value) == base::operator*();
        }

        constexpr bool operator!=(const std::byte value) const noexcept
        {
            return !(*this == value);
        }
    };

    export
    class opt_char : private compact_optional<int, -1>
    {
        using base = compact_optional<int, -1>;
    public:
        constexpr opt_char() noexcept = default;
        constexpr opt_char(std::nullopt_t) noexcept
            : base{std::nullopt} {}

        constexpr opt_char(const opt_byte value) noexcept
            : base{value} {}

        constexpr opt_char(const char value) noexcept
            : base{static_cast<int>(value)} {}

        constexpr opt_char(const opt_char&) noexcept = default;

        using base::has_value;
        
        constexpr explicit operator bool() const noexcept
        {
            return has_value();
        }

        constexpr char operator*() const noexcept
        {
            return static_cast<char>(base::operator*());
        }

        constexpr bool operator==(const opt_char&) const noexcept = default;
        constexpr bool operator!=(const opt_char&) const noexcept = default;

        constexpr bool operator==(const char value) const noexcept
        {
            return static_cast<int>(value) == base::operator*();
        }

        constexpr bool operator!=(const char value) const noexcept
        {
            return !(*this == value);
        }
    };

    export
    class istream
    {
    public:
        using value_type    = std::byte;
        using size_type     = std::size_t;
        using ssize_type    = std::make_signed_t<size_type>;
        using opt_type      = opt_byte;

        virtual ~istream() noexcept = default;

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

        constexpr virtual size_type read(ostream& os, size_type count)
        {
            const auto save {count};
            while (count)
            {
                if (const auto avail {std::min(count, read_available())})
                {
                    os.write(gcur_, avail);
                    gcur_ += avail;
                    count -= avail;
                }
                else if (const auto val {uflow()})
                {
                    os.put(*val);
                    --count;
                }
                else
                {
                    break;
                }
            }

            return save - count;
        }

        constexpr virtual size_type read(ostream& os, size_type count, const std::byte delim)
        {
            const auto save {count};
            while (count)
            {
                if (const auto avail {std::min(count, read_available())})
                {
                    const auto end {gcur_ + avail};
                    const auto x {std::find(gcur_, end, delim)};
                    const auto read_count {static_cast<size_type>(x - gcur_)};
                    os.write(gcur_, read_count);
                    gcur_  = x;
                    count -= read_count;
                    if (x != end)
                    {
                        break;
                    }
                }
                else if (const auto val {underflow()})
                {
                    if (val == delim)
                    {
                        break;
                    }
                    os.put(*val);
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

        constexpr virtual size_type ignore(size_type count, const std::byte delim)
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
                else if (const auto val {uflow()})
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

        size_type read(std::byte* buf, size_type count);
        size_type read(std::byte* buf, size_type count, std::byte delim);

        constexpr virtual void sync() {}

        istream_iterator begin();
        static constexpr inline constexpr_value<std::default_sentinel> end{};

        template<class F>
            requires(std::is_invocable_r_v<const std::byte*, F, const std::byte*, const std::byte*>)
        constexpr size_type consume(F&& f) noexcept(std::is_nothrow_invocable_v<F, const std::byte*, const std::byte*>)
        {
            const auto new_gcur {std::invoke(std::forward<F>(f), gcur_, gend_)};
            const auto amount {static_cast<size_type>(new_gcur - gcur_)};
            gcur_ += amount;
            return amount;
        }
    protected:
        constexpr istream() noexcept
            : gbeg_{nullptr}
            , gcur_{nullptr}
            , gend_{nullptr} {}

        constexpr istream(const istream&) noexcept = default;

        [[nodiscard]] constexpr std::byte* gbeg() const noexcept
        {
            return gbeg_;
        }

        [[nodiscard]] constexpr std::byte* gcur() const noexcept
        {
            return gcur_;
        }

        [[nodiscard]] constexpr std::byte* gend() const noexcept
        {
            return gend_;
        }

        constexpr virtual opt_type underflow()
        {
            return {};
        }

        constexpr opt_type uflow()
        {
            const auto val {underflow()};
            gcur_ += static_cast<bool>(read_available());
            return val;
        }

        constexpr void gbump(const ssize_type count) noexcept
        {
            gcur_ += count;
        }

        constexpr void setg(std::byte* const gb, std::byte* const gc, std::byte* const ge) noexcept
        {
            gbeg_ = gb;
            gcur_ = gc;
            gend_ = ge;
        }

        constexpr void swap(istream& other) noexcept
        {
            std::swap(gbeg_, other.gbeg_);
            std::swap(gcur_, other.gcur_);
            std::swap(gend_, other.gend_);
        }
    private:
        std::byte* gbeg_;
        std::byte* gcur_;
        std::byte* gend_;

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

        constexpr size_type ignore_unlimited(const std::byte delim)
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

    class istream_iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::byte;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        constexpr istream_iterator(const std::default_sentinel_t = {}) noexcept
            : value{}, stream{nullptr} {}

        constexpr istream_iterator(istream& stream)
            : value{}, stream{std::addressof(stream)}
        {
            if (const auto val {stream.get()})
            {
                value = *val;
            }
            else
            {
                this->stream = nullptr;
            }
        }

        constexpr istream_iterator(const istream_iterator& other) noexcept = default;

        constexpr const std::byte operator*() const
        {
            return value;
        }

        constexpr istream_iterator& operator++()
        {
            if (const auto val {stream->next()})
            {
                value = *val;
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
        std::byte value;
        istream* stream;
    };

    export
    struct text_istream
    {
        istream& stream;

        constexpr text_istream(istream& stream) noexcept
            : stream{stream} {}

        [[nodiscard]] constexpr istream::size_type read_available() const noexcept
        {
            return stream.read_available();
        }

        constexpr inline opt_char get()
        {
            return stream.get();
        }

        constexpr inline opt_char bump()
        {
            return stream.bump();
        }

        constexpr inline opt_char next()
        {
            return stream.next();
        }

        constexpr inline istream::size_type read(text_ostream os, const istream::size_type count)
        {
            return stream.read(os.stream, count);
        }

        constexpr inline istream::size_type read(text_ostream os, const istream::size_type count, const char delim)
        {
            return stream.read(os.stream, count, std::byte(delim));
        }

        constexpr inline istream::size_type ignore(const istream::size_type count)
        {
            return stream.ignore(count);
        }

        constexpr inline istream::size_type ignore(const istream::size_type count, const char delim)
        {
            return stream.ignore(count, std::byte(delim));
        }

        istream::size_type read(char* buf, istream::size_type count);
        istream::size_type read(char* buf, istream::size_type count, char delim);

        template<class F>
            requires(std::is_invocable_r_v<const char*, F, const char*, const char*>)
        constexpr inline istream::size_type consume(F&& f) noexcept(std::is_nothrow_invocable_v<F, const char*, const char*>)
        {
            return stream.consume([&](const auto beg, const auto end) {
                return reinterpret_cast<const std::byte*>(std::invoke(std::forward<F>(f), reinterpret_cast<const char*>(beg), reinterpret_cast<const char*>(end)));
            });
        }

        text_istream_iterator begin();
        static constexpr inline constexpr_value<std::default_sentinel> end{};
    };

    class text_istream_iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = char;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        constexpr text_istream_iterator(const std::default_sentinel_t = {}) noexcept
            : it{} {}

        constexpr text_istream_iterator(text_istream is)
            : it{is.stream} {}

        constexpr text_istream_iterator(const text_istream_iterator& other) noexcept = default;

        constexpr const char operator*() const
        {
            return static_cast<char>(*it);
        }

        constexpr text_istream_iterator& operator++()
        {
            ++it;
            return *this;
        }

        constexpr text_istream_iterator operator++(int)
        {
            auto temp {*this};
            ++*this;
            return temp;
        }

        constexpr bool operator==(const text_istream_iterator&) const noexcept = default;
        constexpr bool operator!=(const text_istream_iterator&) const noexcept = default;

        constexpr bool operator==(const std::default_sentinel_t) const noexcept
        {
            return it == std::default_sentinel;
        }
    private:
        istream_iterator it;
    };

    istream_iterator istream::begin()
    {
        return {*this};
    }

    text_istream_iterator text_istream::begin()
    {
        return {*this};
    }

    // export
    // text_istream& ws(text_istream& is)
    // {
    //     while (is.get())
    //     {
    //         is.consume([](const auto begin, const auto end) {
    //             return std::find_if_not(begin, end, isspace_ascii);
    //         });
    //         if (is.read_available())
    //         {
    //             break;
    //         }
    //     }

    //     return is;
    // }

    // export
    // template<class T>
    // bool operator>>(basic_istream<T>& is, T& t)
    // {
    //     if (auto val {is.get()})
    //     {
    //         t = std::move(*val);
    //         is.bump();
    //         return true;
    //     }

    //     return false;
    // }

    // export
    // bool operator>>(text_istream& is, std::string& str)
    // {
    //     str.clear();
    //     is >> ws;

    //     while (is.get())
    //     {
    //         is.consume([&](const auto begin, auto end) {
    //             end = std::find_if(begin, end, isspace_ascii);
    //             str.append(begin, end);
    //             return end;
    //         });

    //         if (is.read_available())
    //         {
    //             break;
    //         }
    //     }

    //     return !str.empty();
    // }

    export
    bool getline(text_istream& is, std::string& str, const char delim)
    {
        str.clear();

        while (is.get())
        {
            is.consume([&](const auto begin, auto end) {
                end = std::find(begin, end, delim);
                str.append(begin, end);
                return end;
            });

            if (is.read_available())
            {
                is.bump();
                return true;
            }
        }

        return !str.empty();
    }

    export
    bool getline(text_istream& is, std::string& str)
    {
        return getline(is, str, '\n');
    }
}