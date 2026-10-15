export module lib2.io:sink;

import std;

import lib2.dyn;

namespace lib2::io
{
    export
    template<class T>
    concept sink =
        requires(T& t, char c, const char* buf, std::size_t n, std::span<const char> sp)
    {
        { t.put(c) }        -> std::same_as<void>;
        { t.write(buf, n) } -> std::same_as<void>;
        { t.write(buf) }    -> std::same_as<void>;
        { t.write(sp) }     -> std::same_as<void>;
        { t.fill(c, n) }    -> std::same_as<void>;
    };

    export
    struct sink_facet
    {
        struct vtable_type
        {
            void (*put)(void*, char);
            void (*write)(void*, const char*, std::size_t);
            void (*fill)(void*, char, std::size_t);
        };

        // template<sink S>
        // static inline constexpr vtable_type vtable_for {
        //     .put = [](void* const ptr, const char c) {
        //         static_cast<S*>(ptr)->put(c);
        //     },
        //     .write = [](void* const ptr, const char* const buf, const std::size_t count) {
        //         static_cast<S*>(ptr)->write(buf, count);
        //     },
        //     .fill = [](void* const ptr, const char c, const std::size_t count) {
        //         static_cast<S*>(ptr)->fill(c, count);
        //     }
        // };

        template<sink S>
        static constexpr const vtable_type& get_vtable() noexcept
        {
            static constexpr vtable_type vt {
                .put = [](void* const ptr, const char c) {
                    static_cast<S*>(ptr)->put(c);
                },
                .write = [](void* const ptr, const char* const buf, const std::size_t count) {
                    static_cast<S*>(ptr)->write(buf, count);
                },
                .fill = [](void* const ptr, const char c, const std::size_t count) {
                    static_cast<S*>(ptr)->fill(c, count);
                }
            };
            return vt;
        }

        template<class T>
        static constexpr bool supports {sink<T>};

        struct mixin
        {
            inline void put(this auto& self, const char c)
            {
                self.vtable()->put(self.obj(), c);
            }

            inline void write(this auto& self, const char* const buf, const std::size_t n)
            {
                self.vtable()->write(self.obj(), buf, n);
            }

            inline void write(this auto& self, const std::span<const char> buf)
            {
                self.write(buf.data(), buf.size());
            }

            inline void write(this auto& self, const char* const cstr)
            {
                self.write(cstr, std::char_traits<char>::length(cstr));
            }

            inline void fill(this auto& self, const char c, const std::size_t n)
            {
                self.vtable()->fill(self.obj(), c, n);
            }
        };
    };

    export
    using dyn_sink = dyn<sink_facet>;
    static_assert(sink<dyn_sink>);

    export
    struct size_sink
    {
        std::size_t size {0};

        constexpr void put(const char) noexcept { ++size; }
        constexpr void write(const char* const, const std::size_t n) noexcept { size += n; }
        constexpr void write(const char* const buf) noexcept { size += std::char_traits<char>::length(buf); }
        constexpr void write(const std::span<const char> buf) noexcept { size += buf.size(); }
        constexpr void fill(const char, const std::size_t n) noexcept { size += n; }
    };
    static_assert(sink<size_sink>);

    export
    struct null_sink
    {
        constexpr void put(const char) noexcept {}
        constexpr void write(const char* const, const std::size_t) noexcept {}
        constexpr void write(const char* const) noexcept {}
        constexpr void write(const std::span<const char>) noexcept {}
        constexpr void fill(const char, const std::size_t) noexcept {}
    };
    static_assert(sink<size_sink>);

    export
    struct throwing_sink
    {
        [[noreturn]] constexpr void put(const char) { throw std::runtime_error{"throwing_sink"}; }
        [[noreturn]] constexpr void write(const char* const, const std::size_t) { throw std::runtime_error{"throwing_sink"}; }
        [[noreturn]] constexpr void write(const char* const) { throw std::runtime_error{"throwing_sink"}; }
        [[noreturn]] constexpr void write(const std::span<const char>) { throw std::runtime_error{"throwing_sink"}; }
        [[noreturn]] constexpr void fill(const char, const std::size_t) { throw std::runtime_error{"throwing_sink"}; }
    };
    static_assert(sink<size_sink>);
}