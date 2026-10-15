export module lib2.io:ref;

import std;

import :sink;

namespace lib2::io
{
    template<class C>
    concept back_inserter_for_sink =
        requires
    {
        typename C::value_type;
        typename C::iterator;
    } && std::same_as<typename C::value_type, char>
      && requires(C& container, char val, const char* str, std::size_t n, typename C::iterator pos)
    {
        container.push_back(val);
        container.insert(pos, str, str);
        container.insert(pos, n, val);
    };

    export
    template<back_inserter_for_sink C>
    class back_insert_sink
    {
    public:
        back_insert_sink(C& c) noexcept
            : container{std::addressof(c)} {}

        void put(const char c)
        {
            container->push_back(c);
        }

        void write(const char* const buf, const std::size_t n)
        {
            if constexpr (requires(C& container, const char* buf, std::size_t n) {
                container.append(buf, n);
            })
            {
                container->append(buf, n);
            }
            else
            {
                container->insert(container->end(), buf, buf + n);
            }
        }

        void write(const char* const buf)
        {
            write(buf, std::char_traits<char>::length(buf));
        }

        void write(const std::span<const char> buf)
        {
            write(buf.data(), buf.size());
        }

        void fill(const char c, const std::size_t n)
        {
            if constexpr (requires(C& container, const char c, std::size_t n) {
                container.append(n, c);
            })
            {
                container->append(n, c);
            }
            else
            {
                container->insert(container->end(), n, c);
            }
        }
    protected:
        C* container;
    };

    export
    template<back_inserter_for_sink C>
    back_insert_sink(C&) -> back_insert_sink<C>;

    export
    using string_ref_sink = back_insert_sink<std::string>;
    static_assert(sink<string_ref_sink>);
}