export module lib2.io:buffered;

import std;

import lib2.meta;

import :sink;
import :source;

namespace lib2::io
{
    template<std::size_t N>
        requires(N != 0)
    class stack_storage
    {
    public:
        constexpr stack_storage() noexcept = default;

        [[nodiscard]] constexpr char* data() noexcept
        {
            return buf;
        }

        [[nodiscard]] constexpr const char* data() const noexcept
        {
            return buf;
        }

        static constexpr constexpr_value<N> capacity {};
    private:
        char buf[N];
    };

    class heap_storage
    {
    public:
        heap_storage(const std::size_t cap)
            : cap{cap}
        {
            if (!cap)
            {
                throw std::invalid_argument{"heap_storage: Capacity must not be 0"};
            }

            buf = std::make_unique<char[]>(cap);
        }

        [[nodiscard]] char* data() noexcept
        {
            return buf.get();
        }

        [[nodiscard]] const char* data() const noexcept
        {
            return buf.get();
        }

        [[nodiscard]] std::size_t capacity() const noexcept
        {
            return cap;
        }

        void resize(const std::size_t n)
        {
            buf = std::make_unique<char[]>(n);
            cap = n;
        }
    private:
        std::size_t cap;
        std::unique_ptr<char[]> buf;
    };

    template<sink S, typename Storage>
    class buffered_sink_impl
    {
    public:
        constexpr ~buffered_sink_impl() noexcept
        {
            try
            {
                flush();
            }
            catch (...) {}
        }

        [[nodiscard]] constexpr std::size_t write_available() const noexcept
        {
            return storage.capacity() - pos;
        }

        [[nodiscard]] constexpr std::size_t amount_written() const noexcept
        {
            return pos;
        }

        [[nodiscard]] constexpr std::span<const char> view() const noexcept
        {
            return {storage.data(), pos};
        }

        constexpr void discard() noexcept
        {
            pos = 0;
        }

        constexpr void put(const char val)
        {
            if (!write_available())
            {
                sink.write(storage.data(), pos);
                pos = 0;
            }

            storage.data()[pos++] = val;
        }

        constexpr void write(const char* vals, std::size_t count)
        {
            if (const auto amount {std::min(count, write_available())})
            {
                std::copy_n(vals, amount, storage.data() + pos);
                count -= amount;
                vals += amount;
                pos += amount;
            }

            if (count)
            {
                flush();
                if (count <= write_available())
                {
                    std::copy_n(vals, count, storage.data() + pos);
                    pos += count;
                }
                else
                {
                    sink.write(vals, count);
                }
            }
        }

        constexpr void write(const char* const buf)
        {
            write(buf, std::char_traits<char>::length(buf));
        }

        constexpr void write(const std::span<const char> buf)
        {
            write(buf.data(), buf.size());
        }

        constexpr void fill(const char val, std::size_t count)
        {
            while (count)
            {
                if (const auto amount {std::min(count, write_available())})
                {
                    std::fill_n(storage.data() + pos, amount, val);
                    count -= amount;
                    pos += amount;
                }

                if (count)
                {
                    flush();
                }
            }
        }

        constexpr void flush()
        {
            if (pos)
            {
                sink.write(storage.data(), pos);
                pos = 0;
            }
        }

        S sink;
    protected:
        constexpr buffered_sink_impl() noexcept(std::is_nothrow_default_constructible_v<Storage> && std::is_nothrow_default_constructible_v<S>) = default;

        template<class... StorageArgs, class... Args>
            requires(std::constructible_from<S, Args...>)
        constexpr buffered_sink_impl(std::tuple<StorageArgs...> stargs, Args&&... args) noexcept(std::is_nothrow_constructible_v<Storage, StorageArgs...> && std::is_nothrow_constructible_v<S, Args...>)
            : sink{std::forward<Args>(args)...}
            , storage{std::apply([](auto&&... stargs) { return Storage{std::forward<decltype(stargs)>(stargs)...}; }, std::move(stargs))}
            , pos{0} {}

        Storage storage;
        std::size_t pos;
    };

    export
    template<sink S, std::size_t N>
    class buffered_sink : public buffered_sink_impl<S, stack_storage<N>>
    {
    public:
        template<class... Args>
            requires(std::constructible_from<S, Args...>)
        constexpr buffered_sink(Args&&... args) noexcept(std::is_nothrow_constructible_v<S, Args...>)
            : buffered_sink_impl<S, stack_storage<N>>{{}, std::forward<Args>(args)...} {}
    };
    static_assert(sink<buffered_sink<null_sink, 1>>);

    export
    template<sink S>
    class heap_buffered_sink : public buffered_sink_impl<S, heap_storage>
    {
    public:
        template<class... Args>
            requires(std::constructible_from<S, Args...>)
        constexpr heap_buffered_sink(const std::size_t cap, Args&&... args)
            : buffered_sink_impl<S, heap_storage>{std::tuple{cap}, std::forward<Args>(args)...} {}

        void resize(const std::size_t cap)
        {
            this->storage.resize(cap);
            this->pos = 0;
        }
    };
    static_assert(sink<heap_buffered_sink<null_sink>>);

    export
    template<std::size_t N>
    struct fixed_sink : public buffered_sink_impl<throwing_sink, stack_storage<N>>
    {
    public:
        constexpr fixed_sink() noexcept = default;
    };
    static_assert(sink<fixed_sink<1>>);


    template<source S, typename Storage>
    class buffered_source_impl
    {
    public:
        constexpr opt_char get()
        {
            if (pos == size)
            {
                if (!refill())
                {
                    return {};
                }
            }

            return storage.data()[pos++];
        }

        constexpr std::size_t read(char* buf, std::size_t n)
        {
            std::size_t read_amount {std::min(n, peek().size())};
            if (read_amount)
            {
                std::copy_n(storage.data() + pos, read_amount, buf);
                n -= read_amount;
                buf += read_amount;
                pos += read_amount;
            }

            if (n)
            {
                if (n < storage.capacity())
                {
                    n = std::min(refill(), n);
                    std::copy_n(storage.data() + pos, n, buf);
                    pos += n;
                    read_amount += n;
                }
                else
                {
                    read_amount += source.read(buf, n);
                }
            }

            return read_amount;
        }

        constexpr std::size_t read(const std::span<char> buf)
        {
            return read(buf.data(), buf.size());
        }

        [[nodiscard]] constexpr std::span<const char> peek()
        {
            if (pos == size)
            {
                refill();
            }

            return {storage.data() + pos, storage.data() + size};
        }

        constexpr void advance(const std::size_t n) noexcept
        {
            pos += n;
        }

        S source;
    protected:
        constexpr buffered_source_impl() noexcept(std::is_nothrow_default_constructible_v<Storage> && std::is_nothrow_default_constructible_v<S>) = default;

        template<class... StorageArgs, class... Args>
            requires(std::constructible_from<S, Args...>)
        constexpr buffered_source_impl(std::tuple<StorageArgs...> stargs, Args&&... args) noexcept(std::is_nothrow_constructible_v<Storage, StorageArgs...> && std::is_nothrow_constructible_v<S, Args...>)
            : source{std::forward<Args>(args)...}
            , storage{std::apply([](auto&&... stargs) { return Storage{std::forward<decltype(stargs)>(stargs)...}; }, std::move(stargs))}
            , pos{0}
            , size{0} {}

        constexpr std::size_t refill()
        {
            size = source.read(storage.data(), storage.capacity());
            pos = 0;
            return size;
        }

        Storage storage;
        std::size_t pos;
        std::size_t size;
    };

    export
    template<source S, std::size_t N>
    class buffered_source : public buffered_source_impl<S, stack_storage<N>>
    {
    public:
        template<class... Args>
            requires(std::constructible_from<S, Args...>)
        constexpr buffered_source(Args&&... args) noexcept(std::is_nothrow_constructible_v<S, Args...>)
            : buffered_source_impl<S, stack_storage<N>>{{}, std::forward<Args>(args)...} {}
    };

    export
    template<source S>
    class heap_buffered_source : public buffered_source_impl<S, heap_storage>
    {
    public:
        template<class... Args>
            requires(std::constructible_from<S, Args...>)
        constexpr heap_buffered_source(const std::size_t cap, Args&&... args)
            : buffered_source_impl<S, heap_storage>{std::tuple{cap}, std::forward<Args>(args)...} {}

        void resize(const std::size_t cap)
        {
            this->storage.resize(cap);
            this->pos = 0;
            this->size = 0;
        }
    };
}