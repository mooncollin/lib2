export module lib2.io:stringstream;

import std;

import :ostream;
import :istream;
import :iostream;

namespace lib2
{
    export
    template<io_character CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
    class basic_istringstream : public basic_istream<CharT>
    {
    public:
        using value_type     = typename basic_istream<CharT>::value_type;
        using size_type      = typename basic_istream<CharT>::size_type;
        using ssize_type     = typename basic_istream<CharT>::ssize_type;
        using char_type      = CharT;
        using traits_type    = Traits;
        using pos_type       = typename traits_type::pos_type;
        using off_type       = typename traits_type::off_type;
        using allocator_type = Allocator;

        constexpr basic_istringstream() noexcept
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_istringstream(const std::basic_string<CharT, Traits, Allocator>& s)
            : buf{s}
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_istringstream(std::basic_string<CharT, Traits, Allocator>&& s) noexcept
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
        constexpr explicit basic_istringstream(const std::basic_string<CharT, Traits, SAlloc>& s)
            : buf{s.begin(), s.end()}
        {
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr basic_istringstream(const std::basic_string<CharT, Traits, SAlloc>& s, const Allocator& a)
            : buf{s.begin(), s.end(), a}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::basic_string_view<CharT, Traits>> StringViewLike>
        constexpr explicit basic_istringstream(const StringViewLike& sv)
            : buf{sv}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::basic_string_view<CharT, Traits>> StringViewLike>
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

        constexpr std::basic_string_view<CharT, Traits> view() const noexcept
        {
            return buf;
        }

        constexpr std::basic_string<CharT, Traits, Allocator> str() const&
        {
            return buf;
        }

        template<class SAlloc>
        constexpr std::basic_string<CharT, Traits, SAlloc> str(const SAlloc& a) const
        {
            return {buf, a};
        }

        constexpr std::basic_string<CharT, Traits, Allocator> str() && noexcept
        {
            auto temp {std::move(buf)};
            buf.clear();
            init_buf_ptrs();

            return std::move(temp);
        }

        constexpr void str(const std::basic_string<CharT, Traits, Allocator>& s)
        {
            buf = s;
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr void str(const std::basic_string<CharT, Traits, SAlloc>& s)
        {
            buf.assign(s.begin(), s.end());
            init_buf_ptrs();
        }

        constexpr void str(std::basic_string<CharT, Traits, Allocator>&& s) noexcept
        {
            buf = std::move(s);
            init_buf_ptrs();
        }

        template<std::convertible_to<std::basic_string_view<CharT, Traits>> StringViewLike>
        constexpr void str(const StringViewLike& t)
        {
            buf = t;
            init_buf_ptrs();
        }
    private:
        std::basic_string<CharT, Traits, Allocator> buf;

        constexpr void init_buf_ptrs() noexcept
        {
            this->setg(buf.data(), buf.data(), buf.data() + buf.size());
        }
    };

    export
    template<io_character CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
    class basic_ostringstream : public basic_ostream<CharT>
    {
    public:
        using value_type     = typename basic_istream<CharT>::value_type;
        using size_type      = typename basic_istream<CharT>::size_type;
        using ssize_type     = typename basic_istream<CharT>::ssize_type;
        using char_type      = CharT;
        using traits_type    = Traits;
        using pos_type       = typename traits_type::pos_type;
        using off_type       = typename traits_type::off_type;
        using allocator_type = Allocator;

        constexpr basic_ostringstream() noexcept
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_ostringstream(const std::basic_string<CharT, Traits, Allocator>& s)
            : buf{s}
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_ostringstream(std::basic_string<CharT, Traits, Allocator>&& s) noexcept
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
        constexpr explicit basic_ostringstream(const std::basic_string<CharT, Traits, SAlloc>& s)
            : buf{s.begin(), s.end()}
        {
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr basic_ostringstream(const std::basic_string<CharT, Traits, SAlloc>& s, const Allocator& a)
            : buf{s.begin(), s.end(), a}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::basic_string_view<CharT, Traits>> StringViewLike>
        constexpr explicit basic_ostringstream(const StringViewLike& sv)
            : buf{sv}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::basic_string_view<CharT, Traits>> StringViewLike>
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

        constexpr std::basic_string_view<CharT, Traits> view() const noexcept
        {
            return {this->pbeg(), this->pcur()};
        }

        constexpr std::basic_string<CharT, Traits, Allocator> str() const&
        {
            return std::basic_string<CharT, Traits, Allocator>{view()};
        }

        template<class SAlloc>
        constexpr std::basic_string<CharT, Traits, SAlloc> str(const SAlloc& a) const
        {
            return std::basic_string<CharT, Traits, SAlloc>{view(), a};
        }

        constexpr std::basic_string<CharT, Traits, Allocator> str() && noexcept
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

        constexpr void str(const std::basic_string<CharT, Traits, Allocator>& s)
        {
            buf = s;
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr void str(const std::basic_string<CharT, Traits, SAlloc>& s)
        {
            buf.assign(s.begin(), s.end());
            init_buf_ptrs();
        }

        constexpr void str(std::basic_string<CharT, Traits, Allocator>&& s) noexcept
        {
            buf = std::move(s);
            init_buf_ptrs();
        }

        template<std::convertible_to<std::basic_string_view<CharT, Traits>> StringViewLike>
        constexpr void str(const StringViewLike& t)
        {
            buf = t;
            init_buf_ptrs();
        }

        constexpr void write(const char_type* s, size_type count) override
        {
            const auto written {this->pcur() - this->pbeg()};
            if ((written + count) > buf.capacity())
            {
                resize_str(count);
            }

            Traits::copy(this->pcur(), s, count);
            this->pbump(count);
        }

        constexpr void fill(const char_type& ch, size_type count) override
        {
            const auto written {this->pcur() - this->pbeg()};
            if ((written + count) > buf.capacity())
            {
                resize_str(count);
            }

            Traits::assign(this->pcur(), count, ch);
            this->pbump(count);
        }

        constexpr void swap(basic_ostringstream& other) noexcept
        {
            std::swap(buf, other.buf);
            basic_ostream<CharT>::swap(other);
        }
    private:
        std::basic_string<CharT, Traits, Allocator> buf;

        constexpr void init_buf_ptrs() noexcept
        {
            this->setp(buf.data(), buf.data() + buf.capacity());
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
    template<io_character CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
    class basic_stringstream : public basic_iostream<CharT>
    {
    public:
        using value_type     = typename basic_istream<CharT>::value_type;
        using size_type      = typename basic_istream<CharT>::size_type;
        using ssize_type     = typename basic_istream<CharT>::ssize_type;
        using char_type      = CharT;
        using traits_type    = Traits;
        using pos_type       = typename traits_type::pos_type;
        using off_type       = typename traits_type::off_type;
        using allocator_type = Allocator;

        constexpr basic_stringstream() noexcept
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_stringstream(const std::basic_string<CharT, Traits, Allocator>& s)
            : buf{s}
        {
            init_buf_ptrs();
        }

        constexpr explicit basic_stringstream(std::basic_string<CharT, Traits, Allocator>&& s) noexcept
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
        constexpr explicit basic_stringstream(const std::basic_string<CharT, Traits, SAlloc>& s)
            : buf{s.begin(), s.end()}
        {
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr basic_stringstream(const std::basic_string<CharT, Traits, SAlloc>& s, const Allocator& a)
            : buf{s.begin(), s.end(), a}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::basic_string_view<CharT, Traits>> StringViewLike>
        constexpr explicit basic_stringstream(const StringViewLike& sv)
            : buf{sv}
        {
            init_buf_ptrs();
        }

        template<std::convertible_to<std::basic_string_view<CharT, Traits>> StringViewLike>
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

        constexpr std::basic_string_view<CharT, Traits> view() const noexcept
        {
            return {this->pbeg(), this->pcur()};
        }

        constexpr std::basic_string<CharT, Traits, Allocator> str() const&
        {
            return std::basic_string<CharT, Traits, Allocator>{view()};
        }

        template<class SAlloc>
        constexpr std::basic_string<CharT, Traits, SAlloc> str(const SAlloc& a) const
        {
            return std::basic_string<CharT, Traits, SAlloc>{view(), a};
        }

        constexpr std::basic_string<CharT, Traits, Allocator> str() && noexcept
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

        constexpr void str(const std::basic_string<CharT, Traits, Allocator>& s)
        {
            buf = s;
            init_buf_ptrs();
        }

        template<class SAlloc>
        constexpr void str(const std::basic_string<CharT, Traits, SAlloc>& s)
        {
            buf.assign(s.begin(), s.end());
            init_buf_ptrs();
        }

        constexpr void str(std::basic_string<CharT, Traits, Allocator>&& s) noexcept
        {
            buf = std::move(s);
            init_buf_ptrs();
        }

        template<std::convertible_to<std::basic_string_view<CharT, Traits>> StringViewLike>
        constexpr void str(const StringViewLike& t)
        {
            buf = t;
            init_buf_ptrs();
        }

        constexpr void write(const char_type* s, size_type count) override
        {
            const auto written {this->pcur() - this->pbeg()};
            if ((written + count) > buf.capacity())
            {
                resize_str(count);
            }

            Traits::copy(this->pcur(), s, count);
            this->pbump(count);
        }

        constexpr void fill(const char_type& ch, size_type count) override
        {
            const auto written {this->pcur() - this->pbeg()};
            if ((written + count) > buf.capacity())
            {
                resize_str(count);
            }

            Traits::assign(this->pcur(), count, ch);
            this->pbump(count);
        }

        constexpr void swap(basic_stringstream& other) noexcept
        {
            std::swap(buf, other.buf);
            basic_iostream<CharT>::swap(other);
        }
    private:
        std::basic_string<CharT, Traits, Allocator> buf;

        constexpr void init_buf_ptrs() noexcept
        {
            this->setg(buf.data(), buf.data(), buf.data() + buf.size());
            this->setp(buf.data(), buf.data() + buf.capacity());
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
    using istringstream  = basic_istringstream<char>;
    
    export
    using wistringstream = basic_istringstream<wchar_t>;
    
    export
    using ostringstream  = basic_ostringstream<char>;
    
    export
    using wostringstream = basic_ostringstream<wchar_t>;
    
    export
    using stringstream   = basic_stringstream<char>;
    
    export
    using wstringstream  = basic_stringstream<char>;

    export
    template<class CharT, class Traits, class Alloc>
    void swap(basic_istringstream<CharT, Traits, Alloc>& lhs, basic_istringstream<CharT, Traits, Alloc>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    export
    template<class CharT, class Traits, class Alloc>
    void swap(basic_ostringstream<CharT, Traits, Alloc>& lhs, basic_ostringstream<CharT, Traits, Alloc>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    export
    template<class CharT, class Traits, class Alloc>
    void swap(basic_stringstream<CharT, Traits, Alloc>& lhs, basic_stringstream<CharT, Traits, Alloc>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    export
    template<class CharT, class Traits, class Alloc>
    basic_ostream<CharT>& operator<<(basic_ostream<CharT>& os, const basic_ostringstream<CharT, Traits, Alloc>& ss)
    {
        return os << ss.view();
    }

    export
    template<class CharT, class Traits, class Alloc>
    basic_ostream<CharT>& operator<<(basic_ostream<CharT>& os, basic_istringstream<CharT, Traits, Alloc>& ss)
    {
        os << ss.view();
        ss.str({});
        return os;
    }
}