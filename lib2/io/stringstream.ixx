export module lib2.io:stringstream;

import std;

import lib2.strings;

import :ostream;
import :istream;
import :iostream;
import :spanstream;

namespace lib2
{
    export
    template<class Allocator = std::allocator<char>>
    class basic_istringstream : public istream
    {
    public:
        using value_type     = istream::value_type;
        using size_type      = istream::size_type;
        using ssize_type     = istream::ssize_type;
        using char_type      = char;
        using traits_type    = std::char_traits<char>;
        using pos_type       = typename traits_type::pos_type;
        using off_type       = typename traits_type::off_type;
        using allocator_type = Allocator;

        constexpr basic_istringstream() noexcept
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_istringstream(const std::basic_string<char_type, traits_type, Allocator>& s)
            : buf{s}
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_istringstream(std::basic_string<char_type, traits_type, Allocator>&& s) noexcept
            : buf{std::move(s)}
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_istringstream(const Allocator& a)
            : buf{a}
        {
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr explicit basic_istringstream(const std::basic_string<char_type, traits_type, SAlloc>& s)
            : buf{s.begin(), s.end()}
        {
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr basic_istringstream(const std::basic_string<char_type, traits_type, SAlloc>& s, const Allocator& a)
            : buf{s.begin(), s.end(), a}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::string_view> StringViewLike>
        constexpr explicit basic_istringstream(const StringViewLike& sv)
            : buf{sv}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::string_view> StringViewLike>
        constexpr basic_istringstream(const StringViewLike& sv, const Allocator& a)
            : buf{sv, a}
        {
            init_buf_ptrs();
        }

        constexpr basic_istringstream(basic_istringstream&& rhs) noexcept
            : buf{std::move(rhs.buf)}
        {
            rhs.buf.clear();
            rhs.init_buf_ptrs();
            init_buf_ptrs();
        }

        constexpr basic_istringstream(basic_istringstream&& rhs, const Allocator& a)
            : buf{std::move(rhs.buf), a}
        {
            rhs.buf.clear();
            rhs.init_buf_ptrs();
            init_buf_ptrs();
        }

        constexpr basic_istringstream(const basic_istringstream&) = delete;

        [[nodiscard]] constexpr std::string_view view() const noexcept
        {
            return buf;
        }

        constexpr std::basic_string<char_type, traits_type, Allocator> str() const&
        {
            return buf;
        }

        template<class SAlloc>
        constexpr std::basic_string<char_type, traits_type, SAlloc> str(const SAlloc& a) const
        {
            return {buf, a};
        }

        constexpr std::basic_string<char_type, traits_type, Allocator> str() && noexcept
        {
            auto temp {std::move(buf)};
            buf.clear();
            init_buf_ptrs();

            return std::move(temp);
        }

        constexpr void str(const std::basic_string<char_type, traits_type, Allocator>& s)
        {
            buf = s;
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr void str(const std::basic_string<char_type, traits_type, SAlloc>& s)
        {
            buf.assign(s.begin(), s.end());
            init_buf_ptrs();
        }

        constexpr void str(std::basic_string<char_type, traits_type, Allocator>&& s) noexcept
        {
            buf = std::move(s);
            init_buf_ptrs();
        }

        template<std::convertible_to<std::string_view> StringViewLike>
        constexpr void str(const StringViewLike& t)
        {
            buf = t;
            init_buf_ptrs();
        }

        constexpr inline opt_char get()
        {
            return istream::get();
        }

        constexpr inline opt_char bump()
        {
            return istream::bump();
        }

        constexpr inline opt_char next()
        {
            return istream::next();
        }

        constexpr inline size_type read(text_ostream os, const size_type count)
        {
            return istream::read(os.stream, count);
        }

        constexpr inline size_type read(text_ostream os, const size_type count, const char delim)
        {
            return istream::read(os.stream, count, std::byte(delim));
        }

        size_type read(char* const buf, const size_type count)
        {
            ospanstream ss {{reinterpret_cast<std::byte*>(buf), count}};
            return read(ss, count);
        }

        size_type read(char* const buf, const size_type count, const char delim)
        {
            ospanstream ss {{reinterpret_cast<std::byte*>(buf), count}};
            return read(ss, count, delim);
        }

        constexpr inline istream::size_type ignore(const istream::size_type count, const char delim)
        {
            return istream::ignore(count, std::byte(delim));
        }

        template<class F>
            requires(std::is_invocable_r_v<const char*, F, const char*, const char*>)
        constexpr inline istream::size_type consume(F&& f) noexcept(std::is_nothrow_invocable_v<F, const char*, const char*>)
        {
            return istream::consume([&](const auto beg, const auto end) {
                return reinterpret_cast<const std::byte*>(std::invoke(std::forward<F>(f), reinterpret_cast<const char*>(beg), reinterpret_cast<const char*>(end)));
            });
        }
    private:
        std::basic_string<char_type, traits_type, Allocator> buf;

        constexpr void init_buf_ptrs() noexcept
        {
            this->setg(reinterpret_cast<std::byte*>(buf.data()), reinterpret_cast<std::byte*>(buf.data()), reinterpret_cast<std::byte*>(buf.data() + buf.size()));
        }
    };

    export
    template<class Allocator = std::allocator<char>>
    class basic_ostringstream : public ostream
    {
    public:
        using value_type     = ostream::value_type;
        using size_type      = ostream::size_type;
        using ssize_type     = ostream::ssize_type;
        using char_type      = char;
        using traits_type    = std::char_traits<char>;
        using pos_type       = typename traits_type::pos_type;
        using off_type       = typename traits_type::off_type;
        using allocator_type = Allocator;

        constexpr basic_ostringstream() noexcept
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_ostringstream(const std::basic_string<char_type, traits_type, Allocator>& s)
            : buf{s}
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_ostringstream(std::basic_string<char_type, traits_type, Allocator>&& s) noexcept
            : buf{std::move(s)}
        {
            init_buf_ptrs();
        }

        constexpr basic_ostringstream(const Allocator& a)
            : buf{a}
        {
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr explicit basic_ostringstream(const std::basic_string<char_type, traits_type, SAlloc>& s)
            : buf{s.begin(), s.end()}
        {
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr basic_ostringstream(const std::basic_string<char_type, traits_type, SAlloc>& s, const Allocator& a)
            : buf{s.begin(), s.end(), a}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::string_view> StringViewLike>
        constexpr explicit basic_ostringstream(const StringViewLike& sv)
            : buf{sv}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::string_view> StringViewLike>
        constexpr basic_ostringstream(const StringViewLike& sv, const Allocator& a)
            : buf{sv, a}
        {
            init_buf_ptrs();
        }

        constexpr basic_ostringstream(basic_ostringstream&& rhs) noexcept
            : buf{std::move(rhs.buf)}
        {
            rhs.buf.clear();
            rhs.init_buf_ptrs();
            init_buf_ptrs();
        }

        constexpr basic_ostringstream(basic_ostringstream&& rhs, const Allocator& a)
            : buf{std::move(rhs.buf), a}
        {
            rhs.buf.clear();
            rhs.init_buf_ptrs();
            init_buf_ptrs();
        }

        basic_ostringstream(const basic_ostringstream&) = delete;

        constexpr std::string_view view() const noexcept
        {
            return {reinterpret_cast<const char*>(this->pbeg()), reinterpret_cast<const char*>(this->pcur())};
        }

        constexpr std::basic_string<char_type, traits_type, Allocator> str() const&
        {
            return std::basic_string<char_type, traits_type, Allocator>{view()};
        }

        template<class SAlloc>
        constexpr std::basic_string<char_type, traits_type, SAlloc> str(const SAlloc& a) const
        {
            return std::basic_string<char_type, traits_type, SAlloc>{view(), a};
        }

        constexpr std::basic_string<char_type, traits_type, Allocator> str() && noexcept
        {
            // Turn capacity into size. Make the data "real"
            const auto written {this->pcur() - this->pbeg()};
            buf.resize_and_overwrite(buf.capacity(), [&](const auto, const auto) {
                return written;
            });

            const auto temp {std::move(buf)};
            buf.clear();
            init_buf_ptrs();

            return temp;
        }

        constexpr void str(const std::basic_string<char_type, traits_type, Allocator>& s)
        {
            buf = s;
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr void str(const std::basic_string<char_type, traits_type, SAlloc>& s)
        {
            buf.assign(s.begin(), s.end());
            init_buf_ptrs();
        }

        constexpr void str(std::basic_string<char_type, traits_type, Allocator>&& s) noexcept
        {
            buf = std::move(s);
            init_buf_ptrs();
        }

        template<std::convertible_to<std::string_view> StringViewLike>
        constexpr void str(const StringViewLike& t)
        {
            buf = t;
            init_buf_ptrs();
        }

        constexpr void put(const char ch)
        {
            ostream::put(std::byte(ch));
        }

        constexpr void write(const std::byte* const s, const size_type count) override
        {
            if ((this->amount_written() + count) > buf.capacity())
            {
                resize_str(count);
            }

            std::copy_n(s, count, this->pcur());
            this->pbump(count);
        }

        constexpr void write(const char_type* const s, const size_type count)
        {
            write(reinterpret_cast<const std::byte*>(s), count);
        }

        constexpr void write(const std::string_view str)
        {
            write(reinterpret_cast<const std::byte*>(str.data()), str.size());
        }

        constexpr void fill(const std::byte ch, size_type count) override
        {
            if ((this->amount_written() + count) > buf.capacity())
            {
                resize_str(count);
            }

            std::fill_n(this->pcur(), count, ch);
            this->pbump(count);
        }

        constexpr void fill(const char_type ch, size_type count)
        {
            fill(std::byte(ch), count);
        }

        constexpr void swap(basic_ostringstream& other) noexcept
        {
            std::swap(buf, other.buf);
            init_buf_ptrs();
            other.init_buf_ptrs();
        }
    protected:
        constexpr void overflow(const std::byte ch) override
        {
            write(&ch, 1);
        }
    private:
        std::basic_string<char_type, traits_type, Allocator> buf;

        constexpr void init_buf_ptrs() noexcept
        {
            this->setp(reinterpret_cast<std::byte*>(buf.data()), reinterpret_cast<std::byte*>(buf.data() + buf.capacity()));
            this->pbump(buf.size());
        }

        constexpr void resize_str(const size_type count)
        {
            // Turn capacity into size. Make the data "real"
            const auto written {this->amount_written()};
            buf.resize_and_overwrite(buf.capacity(), [&](const auto, const auto) {
                return written;
            });

            // Grow faster than normal
            size_type res_amt {buf.capacity() + count};
            if (res_amt < (std::numeric_limits<size_type>::max() / 4))
            {
                res_amt <<= 2;
                try
                {
                    buf.resize_and_overwrite(res_amt, [&](const auto, const auto) {
                        return written;
                    });
                }
                catch (const std::bad_alloc&)
                {
                    buf.resize_and_overwrite(buf.capacity() + count, [&](const auto, const auto) {
                        return written;
                    });
                }
            }
            else
            {
                buf.resize_and_overwrite(res_amt, [&](const auto, const auto) {
                    return written;
                });
            }
            init_buf_ptrs();
        }
    };

    export
    template<class Allocator = std::allocator<char>>
    class basic_stringstream : public iostream
    {
    public:
        using value_type     = istream::value_type;
        using size_type      = istream::size_type;
        using ssize_type     = istream::ssize_type;
        using char_type      = char;
        using traits_type    = std::char_traits<char>;
        using pos_type       = typename traits_type::pos_type;
        using off_type       = typename traits_type::off_type;
        using allocator_type = Allocator;

        constexpr basic_stringstream() noexcept
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_stringstream(const std::basic_string<char_type, traits_type, Allocator>& s)
            : buf{s}
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_stringstream(std::basic_string<char_type, traits_type, Allocator>&& s) noexcept
            : buf{std::move(s)}
        {
            init_buf_ptrs();
        }

        constexpr basic_stringstream(const Allocator& a)
            : buf{a}
        {
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr explicit basic_stringstream(const std::basic_string<char_type, traits_type, SAlloc>& s)
            : buf{s.begin(), s.end()}
        {
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr basic_stringstream(const std::basic_string<char_type, traits_type, SAlloc>& s, const Allocator& a)
            : buf{s.begin(), s.end(), a}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::string_view> StringViewLike>
        constexpr explicit basic_stringstream(const StringViewLike& sv)
            : buf{sv}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::string_view> StringViewLike>
        constexpr basic_stringstream(const StringViewLike& sv, const Allocator& a)
            : buf{sv, a}
        {
            init_buf_ptrs();
        }

        constexpr basic_stringstream(basic_stringstream&& rhs) noexcept
            : buf{std::move(rhs.buf)}
        {
            rhs.buf.clear();
            rhs.init_buf_ptrs();
            init_buf_ptrs();
        }

        constexpr basic_stringstream(basic_stringstream&& rhs, const Allocator& a)
            : buf{std::move(rhs.buf), a}
        {
            rhs.buf.clear();
            rhs.init_buf_ptrs();
            init_buf_ptrs();
        }

        basic_stringstream(const basic_stringstream&) = delete;

        [[nodiscard]] constexpr std::string_view view() const noexcept
        {
            return {reinterpret_cast<const char*>(this->pbeg()), reinterpret_cast<const char*>(this->pcur())};
        }

        constexpr std::basic_string<char_type, traits_type, Allocator> str() const&
        {
            return std::basic_string<char_type, traits_type, Allocator>{view()};
        }

        template<class SAlloc>
        constexpr std::basic_string<char_type, traits_type, SAlloc> str(const SAlloc& a) const
        {
            return std::basic_string<char_type, traits_type, SAlloc>{view(), a};
        }

        constexpr std::basic_string<char_type, traits_type, Allocator> str() && noexcept
        {
            // Turn capacity into size. Make the data "real"
            buf.resize_and_overwrite(buf.capacity(), [this](const auto, const auto) {
                return this->amount_written();
            });

            const auto temp {std::move(buf)};
            buf.clear();
            init_buf_ptrs();

            return temp;
        }

        constexpr void str(const std::basic_string<char_type, traits_type, Allocator>& s)
        {
            buf = s;
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr void str(const std::basic_string<char_type, traits_type, SAlloc>& s)
        {
            buf.assign(s.begin(), s.end());
            init_buf_ptrs();
        }

        constexpr void str(std::basic_string<char_type, traits_type, Allocator>&& s) noexcept
        {
            buf = std::move(s);
            init_buf_ptrs();
        }

        template<std::convertible_to<std::string_view> StringViewLike>
        constexpr void str(const StringViewLike& t)
        {
            buf = t;
            init_buf_ptrs();
        }

        constexpr void put(const char ch)
        {
            ostream::put(std::byte(ch));
        }

        constexpr void write(const std::byte* const s, const size_type count) override
        {
            if ((this->amount_written() + count) > buf.capacity())
            {
                resize_str(count);
            }

            std::copy_n(s, count, this->pcur());
            this->pbump(count);
        }

        constexpr void write(const char_type* const s, const size_type count)
        {
            write(reinterpret_cast<const std::byte*>(s), count);
        }

        constexpr void fill(const std::byte ch, size_type count) override
        {
            if ((this->amount_written() + count) > buf.capacity())
            {
                resize_str(count);
            }

            std::fill_n(this->pcur(), count, ch);
            this->pbump(count);
        }

        constexpr void fill(const char_type ch, size_type count)
        {
            fill(std::byte(ch), count);
        }

        constexpr void swap(basic_stringstream& other) noexcept
        {
            std::swap(buf, other.buf);
            init_buf_ptrs();
            other.init_buf_ptrs();
        }
    private:
        std::basic_string<char_type, traits_type, Allocator> buf;

        constexpr void init_buf_ptrs() noexcept
        {
            this->setg(reinterpret_cast<std::byte*>(buf.data()), reinterpret_cast<std::byte*>(buf.data()), reinterpret_cast<std::byte*>(buf.data() + buf.size()));
            this->setp(reinterpret_cast<std::byte*>(buf.data()), reinterpret_cast<std::byte*>(buf.data() + buf.capacity()));
            this->pbump(buf.size());
        }

        constexpr void resize_str(const size_type count)
        {
            // Turn capacity into size. Make the data "real"
            const auto written {this->pcur() - this->pbeg()};
            buf.resize_and_overwrite(buf.capacity(), [&](const auto, const auto) {
                return written;
            });

            // Grow faster than normal
            size_type res_amt {buf.capacity() + count};
            if (res_amt < (std::numeric_limits<size_type>::max() / 4))
            {
                res_amt <<= 2;
                try
                {
                    buf.resize_and_overwrite(res_amt, [&](const auto, const auto) {
                        return written;
                    });
                }
                catch (const std::bad_alloc&)
                {
                    buf.resize_and_overwrite(buf.capacity() + count, [&](const auto, const auto) {
                        return written;
                    });
                }
            }
            else
            {
                buf.resize_and_overwrite(res_amt, [&](const auto, const auto) {
                    return written;
                });
            }
            init_buf_ptrs();
        }
    };

    export
    using ostringstream = basic_ostringstream<>;

    export
    using istringstream = basic_istringstream<>;
    
    export
    using stringstream  = basic_stringstream<>;

    export
    template<class Alloc>
    void swap(basic_istringstream<Alloc>& lhs, basic_istringstream<Alloc>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    export
    template<class Alloc>
    void swap(basic_ostringstream<Alloc>& lhs, basic_ostringstream<Alloc>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    export
    template<class Alloc>
    void swap(basic_stringstream<Alloc>& lhs, basic_stringstream<Alloc>& rhs) noexcept
    {
        lhs.swap(rhs);
    }
}