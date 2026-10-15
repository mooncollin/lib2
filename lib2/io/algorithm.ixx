export module lib2.io:algorithm;

import std;

import lib2.strings;

import :sink;
import :source;

namespace lib2::io
{
    template<class F>
    concept reader_fn = std::is_invocable_r_v<std::span<const char>::iterator, F, std::span<const char>>;

    template<class F>
    concept reader_p_fn = std::is_invocable_r_v<bool, F, char>;

    export
    struct read_result
    {
        std::size_t count;
        bool found;
    };

    export
    template<peekable_source Source, sink Sink, reader_fn F>
    constexpr read_result read_to(Source&& src, Sink&& dst, F fn)
    {
        std::size_t total {0};
        for (auto w {src.peek()}; !w.empty(); w = src.peek())
        {
            const auto it {std::invoke(fn, w)};
            const std::size_t amount {static_cast<std::size_t>(it - w.begin())};

            dst.write(w.data(), amount);
            src.advance(amount);
            total += amount;

            if (it != w.end())
            {
                return {total, true};
            }
        }

        return {total, false};
    }

    export
    template<contiguous_source Source, sink Sink, reader_fn F>
    constexpr read_result read_to(Source&& src, Sink&& dst, F fn)
    {
        const auto w {src.peek()};
        const auto it {std::invoke(fn, w)};
        const auto amount {it - w.begin()};

        dst.write(w.begin(), amount);
        src.advance(amount);

        if (it != w.end())
        {
            return {amount, true};
        }

        return {amount, false};
    }

    export
    template<peekable_source Source, sink Sink, reader_p_fn F>
    constexpr read_result take_while(Source&& src, Sink&& dst, F fn)
    {
        return read_to(src, dst, [&](const auto w) {
            return std::ranges::find_if_not(w, fn);
        });
    }

    export
    template<peekable_source Source, reader_fn F>
    constexpr read_result skip_to(Source&& src, F fn)
    {
        null_sink dst;
        return read_to(src, dst, std::move(fn));
    }

    export
    template<peekable_source Source, reader_p_fn F>
    constexpr read_result skip_while(Source&& src, F fn)
    {
        null_sink dst;
        return take_while(src, dst, std::move(fn));
    }

    export
    template<peekable_source Source, reader_p_fn Predicate>
    constexpr opt_char expect(Source&& src, Predicate p)
    {
        const auto window {src.peek()};
        if (window.empty() || !std::invoke(p, window.front()))
        {
            return {};
        }

        const auto result {window.front()};
        src.advance(1);
        return result;
    }

    export
    template<peekable_source Source>
    constexpr bool expect(Source&& src, const char c)
    {
        return static_cast<bool>(expect(src, [c](const auto ch) { return c == ch; }));
    }

    export
    template<peekable_source Source, sink Sink>
    constexpr std::optional<std::size_t> getline(Source&& src, Sink&& dst, const char c = '\n')
    {
        const auto result {read_to(src, dst, [c](const auto w) {
            return std::ranges::find(w, c);
        })};

        if (result.found)
        {
            src.advance(1);
            return result.count;
        }

        if (!result.count)
        {
            return {};
        }

        return result.count;
    }

    export
    template<peekable_source Source>
    constexpr void skipws(Source&& src)
    {
        skip_while(src, isspace_ascii);
    }
}