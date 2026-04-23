export module lib2.io:iostream;

import :ostream;
import :istream;

namespace lib2
{
    export
    class iostream : public ostream, public istream
    {
    public:
        virtual ~iostream() noexcept = default;
    protected:
        iostream() noexcept = default;
        iostream(const iostream&) noexcept = default;

        void swap(iostream& other) noexcept
        {
            istream::swap(other);
            ostream::swap(other);
        }
    };

    export
    struct text_iostream
    {
        iostream& stream;

        constexpr text_iostream(iostream& stream) noexcept
            : stream{stream} {}

        constexpr inline void put(const char ch)
        {
            stream.put(std::byte(ch));
        }

        constexpr inline void write(const std::string_view str)
        {
            stream.write(reinterpret_cast<const std::byte*>(str.data()), str.size());
        }

        template<class F>
            requires(std::is_invocable_r_v<char*, F, char*, char*>)
        constexpr inline ostream::size_type produce(F&& f) noexcept(std::is_nothrow_invocable_v<F, char*, char*>)
        {
            return stream.produce([&](const auto beg, const auto end) {
                return reinterpret_cast<char*>(std::invoke(std::forward<F>(f), reinterpret_cast<char*>(beg), reinterpret_cast<char*>(end)));
            });
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

        template<class F>
            requires(std::is_invocable_r_v<const char*, F, const char*, const char*>)
        constexpr inline istream::size_type consume(F&& f) noexcept(std::is_nothrow_invocable_v<F, const char*, const char*>)
        {
            return stream.consume([&](const auto beg, const auto end) {
                return reinterpret_cast<const std::byte*>(std::invoke(std::forward<F>(f), reinterpret_cast<const char*>(beg), reinterpret_cast<const char*>(end)));
            });
        }

        constexpr inline operator text_ostream() noexcept
        {
            return stream;
        }

        constexpr inline operator text_istream() noexcept
        {
            return stream;
        }
    };
}