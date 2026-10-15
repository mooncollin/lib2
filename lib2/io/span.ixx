export module lib2.io:span;

import std;

import :sink;
import :source;

namespace lib2::io
{
    export
    class span_sink
    {
    public:
        constexpr span_sink() noexcept
            : pcur{nullptr}, pend{nullptr} {}

        constexpr span_sink(const std::span<char> buf) noexcept
            : pcur{buf.data()}, pend{pcur + buf.size()} {}

        constexpr span_sink(const span_sink&) noexcept = default;

        [[nodiscard]] constexpr std::span<const char> view() const noexcept
        {
            return {pcur, pend};
        }

        [[nodiscard]] constexpr std::size_t write_available() const noexcept
        {
            return pend - pcur;
        }

        constexpr void put(const char val)
        {
            if (!write_available())
            {
                throw std::out_of_range{"span_sink::put"};
            }

            *pcur++ = val;
        }

        constexpr void write(const char* const vals, const std::size_t count)
        {
            if (count > write_available())
            {
                throw std::out_of_range{"span_sink::write"};
            }

            pcur = std::copy_n(vals, count, pcur);
        }

        constexpr void write(const char* const cstr)
        {
            write(cstr, std::char_traits<char>::length(cstr));
        }

        constexpr void write(const std::span<const char> vals)
        {
            write(vals.data(), vals.size());
        }

        constexpr void fill(const char val, const std::size_t count)
        {
            if (count > write_available())
            {
                throw std::out_of_range{"span_sink::fill"};
            }

            pcur = std::fill_n(pcur, count, val);
        }

        constexpr void swap(span_sink& other) noexcept
        {
            std::swap(pcur, other.pcur);
            std::swap(pend, other.pend);
        }
    private:
        char* pcur;
        char* pend;
    };
    static_assert(sink<span_sink>);

    export
    class span_source
    {
    public:
        constexpr span_source() noexcept
            : pbeg{nullptr}, pcur{nullptr}, pend{nullptr} {}

        constexpr span_source(const std::span<const char> buf) noexcept
            : pbeg{buf.data()}, pcur{pbeg}, pend{pbeg + buf.size()} {}

        constexpr span_source(const span_source&) noexcept = default;

        constexpr opt_char get() noexcept
        {
            if (pcur == pend)
            {
                return {};
            }

            return *pcur++;
        }

        constexpr std::size_t read(char* const buf, std::size_t n) noexcept
        {
            n = std::min(n, peek().size());
            pcur = std::copy_n(pcur, n, buf);
            return n;
        }

        constexpr std::size_t read(const std::span<char> buf) noexcept
        {
            return read(buf.data(), buf.size());
        }

        constexpr std::span<const char> peek() const noexcept
        {
            return {pcur, pend};
        }

        constexpr void advance(const std::size_t n) noexcept
        {
            pcur += n;
        }
        
        constexpr void rewind(const std::ptrdiff_t n) noexcept
        {
            pcur -= n;
        }

        constexpr std::size_t tell() const noexcept
        {
            return pcur - pbeg;
        }

        constexpr char operator[](const std::size_t n) const noexcept
        {
            return pcur[n];
        }

        constexpr void swap(span_source& other) noexcept
        {
            std::swap(pbeg, other.pbeg);
            std::swap(pcur, other.pcur);
            std::swap(pend, other.pend);
        }
    private:
        const char* pbeg;
        const char* pcur;
        const char* pend;
    };
    static_assert(contiguous_source<span_source>);

    constexpr void swap(span_sink& lhs, span_sink& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    constexpr void swap(span_source& lhs, span_source& rhs) noexcept
    {
        lhs.swap(rhs);
    }
}