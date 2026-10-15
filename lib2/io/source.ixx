export module lib2.io:source;

import std;

import lib2.compact_optional;
import lib2.dyn;

namespace lib2::io
{
    export
    class opt_char : private compact_optional<int, -1>
    {
        using base = compact_optional<int, -1>;
    public:
        constexpr opt_char() noexcept = default;
        constexpr opt_char(std::nullopt_t) noexcept
            : base{std::nullopt} {}

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
    template<class T>
    concept source =
        requires(T& t, char c, char* buf, std::size_t n, std::span<char> sp)
    {
        { t.get() }         -> std::same_as<opt_char>;
        { t.read(buf, n) }  -> std::same_as<std::size_t>;
        { t.read(sp) }      -> std::same_as<std::size_t>;
    };

    export
    template<class T>
    concept peekable_source =
        source<T> &&
        requires(T& t, std::size_t n)
    {
        { t.peek() }        -> std::same_as<std::span<const char>>;
        { t.advance(n) }    -> std::same_as<void>;
    };

    export
    template<class T>
    concept rewindable_source =
        peekable_source<T> &&
        requires(T& t, std::size_t n)
    {
        { t.rewind(n) }  -> std::same_as<void>;
        { t.tell() }     -> std::same_as<std::size_t>;
    };

    export
    template<class T>
    concept contiguous_source = peekable_source<T> && rewindable_source<T> &&
        requires(const T& t, std::size_t n)
    {
        { t[n] } -> std::same_as<char>;
    };

    export
    struct source_facet
    {
        struct vtable_type
        {
            opt_char(*get)(void*);
            std::size_t(*read)(void*, char*, std::size_t);
        };

        // template<source S>
        // static constexpr vtable_type vtable_for {
        //     .get = [](void* const ptr) {
        //         return static_cast<S*>(ptr)->get();
        //     },
        //     .read = [](void* const ptr, char* const buf, const std::size_t n) {
        //         return static_cast<S*>(ptr)->read(buf, n);
        //     }
        // };

        template<source S>
        static consteval const vtable_type& get_vtable() noexcept
        {
            static constexpr vtable_type vt {
                .get = [](void* const ptr) {
                    return static_cast<S*>(ptr)->get();
                },
                .read = [](void* const ptr, char* const buf, const std::size_t n) {
                    return static_cast<S*>(ptr)->read(buf, n);
                }
            };
            return vt;
        };

        template<class T>
        static constexpr bool supports {source<T>};

        struct mixin
        {
            inline opt_char get(this auto& self)
            {
                return self.vtable()->get(self.obj());
            }

            inline std::size_t read(this auto& self, char* const buf, const std::size_t n)
            {
                return self.vtable()->read(self.obj(), buf, n);
            }

            inline std::size_t read(this auto& self, const std::span<char> buf)
            {
                return self.read(buf.data(), buf.size());
            }
        };
    };

    export
    using dyn_source = dyn<source_facet>;
    static_assert(source<dyn_source>);

    export
    struct peekable_source_facet
    {
        struct vtable_type
        {
            std::span<const char>(*peek)(void*);
            void(*advance)(void*, std::size_t);
        };

        // template<peekable_source S>
        // static constexpr vtable_type vtable_for {
        //     .peek = [](void* const ptr) {
        //         return static_cast<S*>(ptr)->peek();
        //     },
        //     .advance = [](void* const ptr, const std::size_t n) {
        //         static_cast<S*>(ptr)->advance(n);
        //     }
        // };

        template<peekable_source S>
        static consteval const vtable_type& get_vtable() noexcept
        {
            static constexpr vtable_type vt {
                .peek = [](void* const ptr) {
                    return static_cast<S*>(ptr)->peek();
                },
                .advance = [](void* const ptr, const std::size_t n) {
                    static_cast<S*>(ptr)->advance(n);
                }
            };

            return vt;
        };

        template<class T>
        static constexpr bool supports {peekable_source<T>};

        struct mixin
        {
            inline std::span<const char> peek(this auto& self)
            {
                return self.vtable()->peek(self.obj());
            }

            inline void advance(this auto& self, const std::size_t n)
            {
                self.vtable()->advance(self.obj(), n);
            }
        };
    };

    export
    using dyn_peekable_source = dyn<source_facet, peekable_source_facet>;
    static_assert(peekable_source<dyn_peekable_source>);

    export
    struct rewindable_source_facet
    {
        struct vtable_type
        {
            void(*rewind)(void*, std::size_t);
            std::size_t(*tell)(void*);
        };

        // template<rewindable_source S>
        // static constexpr vtable_type vtable_for {
        //     .rewind = [](void* const ptr, const std::size_t n) {
        //         static_cast<S*>(ptr)->rewind(n);
        //     },
        //     .tell = [](void* const ptr) {
        //         return static_cast<S*>(ptr)->tell();
        //     }
        // };

        template<rewindable_source S>
        static consteval const vtable_type& get_vtable() noexcept
        {
            static constexpr vtable_type vt {
                .rewind = [](void* const ptr, const std::size_t n) {
                    static_cast<S*>(ptr)->rewind(n);
                },
                .tell = [](void* const ptr) {
                    return static_cast<S*>(ptr)->tell();
                }
            };

            return vt;
        };

        template<class T>
        static constexpr bool supports {rewindable_source<T>};

        struct mixin
        {
            inline void rewind(this auto& self, const std::size_t n)
            {
                self.vtable()->rewind(self.obj(), n);
            }

            inline std::size_t tell(this auto& self)
            {
                return self.vtable()->tell(self.obj());
            }
        };
    };

    export
    using dyn_rewindable_source = dyn<source_facet, peekable_source_facet, rewindable_source_facet>;
    static_assert(rewindable_source<dyn_rewindable_source>);

    export
    struct contiguous_source_facet
    {
        struct vtable_type
        {
            char(*operator_index)(void*, std::size_t);
        };

        // template<contiguous_source S>
        // static constexpr vtable_type vtable_for {
        //     .operator_index = [](void* const ptr, const std::size_t n) {
        //         return static_cast<S*>(ptr)[n];
        //     }
        // };

        template<contiguous_source S>
        static consteval const vtable_type& get_vtable() noexcept
        {
            static constexpr vtable_type vt {
                .operator_index = [](void* const ptr, const std::size_t n) {
                    return static_cast<S*>(ptr)[n];
                }
            };

            return vt;
        };

        template<class T>
        static constexpr bool supports {contiguous_source<T>};

        struct mixin
        {
            char operator[](this const auto& self, const std::size_t n)
            {
                return self.vtable()->operator_index(self.obj(), n);
            }
        };
    };

    export
    using dyn_contiguous_source = dyn<source_facet, peekable_source_facet, rewindable_source_facet, contiguous_source_facet>;
    static_assert(contiguous_source<dyn_contiguous_source>);
}