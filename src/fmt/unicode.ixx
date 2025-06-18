export module lib2.fmt:unicode;

import std;

namespace lib2
{
    export
    template<std::ranges::view V>
    class utf8_codepoint_view : public std::ranges::view_interface<utf8_codepoint_view<V>>
    {
    public:
        constexpr explicit utf8_codepoint_view(V base)
            : base_{std::move(base)} {}

        struct sentinel {};

        class iterator
        {
        public:
            using value_type        = std::uint32_t;
            using difference_type   = std::ptrdiff_t;
            using iterator_category = std::input_iterator_tag;

            constexpr iterator()
                : done{true} {}

            constexpr iterator(std::ranges::iterator_t<V> it, std::ranges::iterator_t<V> end)
                : it_{std::move(it)}
                , end_{std::move(end)}
            {
                advance();
            }

            [[nodiscard]] constexpr value_type operator*() const noexcept
            {
                return codepoint;
            }

            constexpr iterator& operator++()
            {
                advance();
                return *this;
            }

            constexpr void operator++(int)
            {
                advance();
            }

            [[nodiscard]] constexpr bool is_invalid() const noexcept
            {
                return invalid;
            }

            constexpr std::ranges::subrange<std::ranges::iterator_t<V>, std::ranges::sentinel_t<V>> leftover()
            {
                return {it_, end_};
            }

            constexpr bool operator==(const sentinel other) const noexcept
            {
                return done;
            }

            constexpr bool operator!=(const sentinel other) const noexcept
            {
                return !done;
            }

            constexpr friend bool operator==(const sentinel lhs, const iterator& rhs) noexcept
            {
                return rhs == lhs;
            }

            constexpr friend bool operator!=(const sentinel lhs, const iterator& rhs) noexcept
            {
                return rhs != lhs;
            }
        private:
            std::ranges::iterator_t<V> it_;
            std::ranges::sentinel_t<V> end_;
            std::uint32_t codepoint {0};
            bool done {false};
            bool invalid {false};

            constexpr void advance()
            {
                invalid = false;
                if (it_ == end_)
                {
                    done = true;
                    return;
                }

                const auto ch {static_cast<unsigned char>(*it_++)};

                if ((ch & 0x80) == 0)
                {
                    codepoint = ch;
                    return;
                }

                std::size_t num_bytes {0};
                if ((ch & 0xE0) == 0xC0)
                {
                    num_bytes = 2;
                }
                else if ((ch & 0xF0) == 0xE0)
                {
                    num_bytes = 3;
                }
                else if ((ch & 0xF8) == 0xF0)
                {
                    num_bytes = 4;
                }
                else
                {
                    invalid = true;
                    codepoint = ch;
                    return;
                }

                uint32_t cp = ch & ((1 << (8 - num_bytes - 1)) - 1);
                for (std::size_t i {1}; i < num_bytes; ++i)
                {
                    if (it_ == end_)
                    {
                        invalid = true;
                        codepoint = ch;
                        return;
                    }

                    const auto next {static_cast<unsigned char>(*it_++)};
                    if ((next & 0xC0) != 0x80)
                    {
                        invalid = true;
                        codepoint = ch;
                        return;
                    }

                    cp = (cp << 6) | (next & 0x3F);
                }

                if ( (num_bytes == 2 && cp < 0x80) ||
                     (num_bytes == 3 && cp < 0x800) ||
                     (num_bytes == 4 && cp < 0x10000) ||
                     (cp > 0x10FFFF) ||
                     (cp >= 0xD800 && cp <= 0xDFFF)
                   )
                {
                    invalid = true;
                    codepoint = ch;
                    return;
                }

                codepoint = cp;
            }
        };

        constexpr iterator begin() const
        {
            return {std::ranges::begin(base_), std::ranges::end(base_)};
        }

        constexpr sentinel end() const noexcept
        {
            return {};
        }
    private:
        V base_;
    };

    export
    template<std::ranges::viewable_range R>
    utf8_codepoint_view(R&&) -> utf8_codepoint_view<std::views::all_t<R>>;
}