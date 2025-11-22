export module lib2.strings:unicode;

import std;

namespace lib2
{
    export
    template<std::ranges::view V>
        requires(std::convertible_to<std::remove_cvref_t<std::ranges::range_reference_t<V>>, char8_t>)
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

            [[nodiscard]] constexpr bool invalid() const noexcept
            {
                return invalid_;
            }

            constexpr std::ranges::subrange<std::ranges::iterator_t<V>, std::ranges::sentinel_t<V>> leftover()
            {
                return {it_, end_};
            }

            constexpr bool operator==(const sentinel) const noexcept { return done; }
            constexpr bool operator!=(const sentinel) const noexcept { return !done; }

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
            bool invalid_ {false};

            constexpr void advance()
            {
                invalid_ = false;
                if (it_ == end_)
                {
                    done = true;
                    return;
                }

                const char8_t b {static_cast<char8_t>(*it_++)};

                if ((b & 0x80) == 0)
                {
                    codepoint = b;
                    return;
                }

                std::size_t num_bytes {0};
                if ((b & 0xE0) == 0xC0)
                {
                    num_bytes = 2;
                }
                else if ((b & 0xF0) == 0xE0)
                {
                    num_bytes = 3;
                }
                else if ((b & 0xF8) == 0xF0)
                {
                    num_bytes = 4;
                }
                else
                {
                    invalid_ = true;
                    codepoint = b;
                    return;
                }

                uint32_t cp = b & ((1 << (8 - num_bytes - 1)) - 1);
                for (std::size_t i {1}; i < num_bytes; ++i)
                {
                    if (it_ == end_)
                    {
                        invalid_ = true;
                        codepoint = b;
                        return;
                    }

                    const char8_t next {static_cast<char8_t>(*it_++)};
                    if ((next & 0xC0) != 0x80)
                    {
                        invalid_ = true;
                        codepoint = b;
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
                    invalid_ = true;
                    codepoint = b;
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
    template<std::ranges::view V>
        requires (std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<V>>, char16_t> ||
                (std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<V>>, wchar_t> && sizeof(wchar_t) == 2))
    class utf16_codepoint_view : public std::ranges::view_interface<utf16_codepoint_view<V>>
    {
    public:
        constexpr explicit utf16_codepoint_view(V base)
            : base_{std::move(base)} {}

        struct sentinel {};

        class iterator
        {
        public:
            using value_type        = std::uint32_t;
            using difference_type   = std::ptrdiff_t;
            using iterator_concept  = std::input_iterator_tag;

            constexpr iterator() : done{true} {}

            constexpr iterator(std::ranges::iterator_t<V> it, std::ranges::iterator_t<V> end)
                : it_{std::move(it)}, end_{std::move(end)}
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

            constexpr void operator++(int) { advance(); }

            [[nodiscard]] constexpr bool invalid() const noexcept
            {
                return invalid_;
            }

            constexpr std::ranges::subrange<std::ranges::iterator_t<V>, std::ranges::sentinel_t<V>> leftover()
            {
                return {it_, end_};
            }

            constexpr bool operator==(sentinel) const noexcept { return done; }
            constexpr bool operator!=(sentinel) const noexcept { return !done; }

            friend constexpr bool operator==(sentinel, iterator const& rhs) noexcept { return rhs.done; }
            friend constexpr bool operator!=(sentinel, iterator const& rhs) noexcept { return !rhs.done; }

        private:
            std::ranges::iterator_t<V> it_;
            std::ranges::sentinel_t<V> end_;
            std::uint32_t codepoint{0};
            bool done{false};
            bool invalid_{false};

            constexpr void advance()
            {
                invalid_ = false;
                if (it_ == end_)
                {
                    done = true;
                    return;
                }

                const char16_t lead {static_cast<char16_t>(*it_++)};

                // Single-unit BMP code point (not surrogate)
                if (lead < 0xD800 || lead > 0xDFFF)
                {
                    codepoint = lead;
                    return;
                }

                // Surrogates
                if (lead >= 0xD800 && lead <= 0xDBFF) // high surrogate
                {
                    if (it_ == end_)
                    {
                        invalid_ = true;
                        codepoint = lead;
                        return;
                    }

                    const char16_t trail = *it_;
                    if (trail >= 0xDC00 && trail <= 0xDFFF)
                    {
                        ++it_;
                        codepoint = 0x10000 + (((lead - 0xD800) << 10) | (trail - 0xDC00));
                        return;
                    }

                    // Unpaired high surrogate
                    invalid_ = true;
                    codepoint = lead;
                    return;
                }

                // Unpaired low surrogate
                invalid_ = true;
                codepoint = lead;
            }
        };

        constexpr iterator begin() const
        {
            return {std::ranges::begin(base_), std::ranges::end(base_)};
        }

        constexpr sentinel end() const noexcept { return {}; }

    private:
        V base_;
    };

    export
    template<std::ranges::view V>
        requires (std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<V>>, char32_t> ||
                (std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<V>>, wchar_t> && sizeof(wchar_t) == 4))
    class utf32_codepoint_view : public std::ranges::view_interface<utf32_codepoint_view<V>>
    {
    public:
        constexpr explicit utf32_codepoint_view(V base)
            : base_{std::move(base)} {}

        struct sentinel {};

        class iterator
        {
        public:
            using value_type        = std::uint32_t;
            using difference_type   = std::ptrdiff_t;
            using iterator_concept  = std::input_iterator_tag;

            constexpr iterator() : done{true} {}

            constexpr iterator(std::ranges::iterator_t<V> it, std::ranges::iterator_t<V> end)
                : it_{std::move(it)}, end_{std::move(end)}
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

            constexpr void operator++(int) { advance(); }

            [[nodiscard]] constexpr bool invalid() const noexcept
            {
                return invalid_;
            }

            constexpr std::ranges::subrange<std::ranges::iterator_t<V>, std::ranges::sentinel_t<V>> leftover()
            {
                return {it_, end_};
            }

            constexpr bool operator==(sentinel) const noexcept { return done; }
            constexpr bool operator!=(sentinel) const noexcept { return !done; }

            friend constexpr bool operator==(sentinel, iterator const& rhs) noexcept { return rhs.done; }
            friend constexpr bool operator!=(sentinel, iterator const& rhs) noexcept { return !rhs.done; }

        private:
            std::ranges::iterator_t<V> it_;
            std::ranges::sentinel_t<V> end_;
            std::uint32_t codepoint{0};
            bool done{false};
            bool invalid_{false};

            constexpr void advance()
            {
                invalid_ = false;
                if (it_ == end_)
                {
                    done = true;
                    return;
                }

                const char32_t unit {static_cast<char32_t>(*it_++)};
                std::uint32_t cp = unit;

                if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF))
                {
                    invalid_ = true;
                }

                codepoint = cp;
            }
        };

        constexpr iterator begin() const
        {
            return {std::ranges::begin(base_), std::ranges::end(base_)};
        }

        constexpr sentinel end() const noexcept { return {}; }

    private:
        V base_;
    };

    namespace views
    {
        export
        template<std::ranges::viewable_range R>
        utf8_codepoint_view<std::views::all_t<R>> utf8_codepoints(R&& r)
        {
            return utf8_codepoint_view<std::views::all_t<R>>{std::ranges::views::all(std::forward<R>(r))};
        }

        export
        template<std::ranges::viewable_range R>
        auto utf_codepoints(R&& r)
        {
            using T = std::remove_cvref_t<std::ranges::range_value_t<R>>;
            if constexpr (std::same_as<T, char> || std::same_as<T, char8_t> || std::same_as<T, std::byte>)
            {
                return utf8_codepoint_view{std::forward<R>(r)};
            }
            else if constexpr (std::same_as<T, char16_t> || (std::same_as<T, wchar_t> && sizeof(wchar_t)==2))
            {
                return utf16_codepoint_view{std::forward<R>(r)};
            }
            else if constexpr (std::same_as<T, char32_t> || (std::same_as<T, wchar_t> && sizeof(wchar_t)==4))
            {
                return utf32_codepoint_view{std::forward<R>(r)};
            }
            else
            {
                static_assert(false, "Unsupported character type for codepoint_view");
            }
        }
    }

    export
    template<std::input_iterator I, std::sentinel_for<I> S, std::output_iterator<char> O>
    constexpr std::ranges::in_out_result<I, O> utf16_to_utf8(I begin, const S end, O out)
    {
        while (begin != end)
        {
            char32_t cp = *begin;
            if (cp >= 0xD800 && cp <= 0xDBFF)
            {
                ++begin;
                if (begin == end)
                {
                    throw std::invalid_argument{"Unpaired high surrogate"};
                }

                const char32_t low = *begin;
                if (low < 0xDC00 || low > 0xDFFF)
                {
                    throw std::invalid_argument{"invalid_ surrogate pair"};
                }

                cp = 0x10000 + (((cp - 0xD800) << 10) | (low - 0xDC00));
            }
            else if (cp >= 0xDC00 && cp <= 0xDFFF)
            {
                throw std::invalid_argument{"Unpaired low surrogate"};
            }

            if (cp <= 0x7F)
            {
                *out++ = static_cast<char>(cp);
            }
            else if (cp <= 0x7FF)
            {
                *out++ = static_cast<char>(0xC0 | (cp >> 6));
                *out++ = static_cast<char>(0x80 | (cp & 0x3F));
            }
            else if (cp <= 0xFFF)
            {
                *out++ = static_cast<char>(0xE0 | (cp >> 12));
                *out++ = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                *out++ = static_cast<char>(0x80 | (cp & 0x3F));
            }
            else
            {
                *out++ = static_cast<char>(0xF0 | (cp >> 18));
                *out++ = static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
                *out++ = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                *out++ = static_cast<char>(0x80 | (cp & 0x3F));
            }

            ++begin;
        }

        return {begin, out};
    }

    export
    template<std::ranges::input_range R, std::output_iterator<char> O>
    constexpr std::ranges::in_out_result<std::ranges::borrowed_iterator_t<R>, O> utf16_to_utf8(R&& r, O out)
    {
        return utf16_to_utf8(std::ranges::begin(r), std::ranges::end(r), std::move(out));
    }

    export
    template<std::input_iterator I, std::sentinel_for<I> S, std::output_iterator<char16_t> O>
    constexpr std::ranges::in_out_result<I, O> utf8_to_utf16(I begin, const S end, O out)
    {
        while (begin != end)
        {
            char8_t c = *begin;

            char32_t cp;
            std::size_t n;

            if (c <= 0x7F)
            {
                cp = c;
            }
            else if ((c & 0xE0) == 0xC0)
            {
                ++begin;
                if (begin == end)
                {
                    throw std::invalid_argument{"Truncated UTF-8"};
                }

                const char8_t c1 = *begin;
                if ((c1 & 0xC0) != 0x80)
                {
                    throw std::invalid_argument{"invalid_ UTF-8 continuation"};
                }

                cp = ((c & 0x1F) << 6) | (c1 & 0x3F);

                if (cp < 0x80)
                {
                    throw std::invalid_argument{"Overlong UTF-8"};
                }
            }
            else if ((c & 0xF0) == 0xE0)
            {
                ++begin;
                if (begin == end)
                {
                    throw std::invalid_argument{"Truncated UTF-8"};
                }

                const char8_t c1 = *begin;

                ++begin;
                if (begin == end)
                {
                    throw std::invalid_argument{"Truncated UTF-8"};
                }

                const char8_t c2 = *begin;

                if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80)
                {
                    throw std::invalid_argument{"invalid_ UTF-8 continuation"};
                }

                cp = ((c & 0x0F) << 12)
                   | ((c1 & 0x3F) << 6)
                   | (c2 & 0x3F);

                if (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF))
                {
                    throw std::invalid_argument{"invalid_ UTF-8 codepoint"};
                }
            }
            else if ((c & 0xF8) == 0xF0)
            {
                ++begin;
                if (begin == end)
                {
                    throw std::invalid_argument{"Truncated UTF-8"};
                }

                const char8_t c1 = *begin;

                ++begin;
                if (begin == end)
                {
                    throw std::invalid_argument{"Truncated UTF-8"};
                }

                const char8_t c2 = *begin;

                ++begin;
                if (begin == end)
                {
                    throw std::invalid_argument{"Truncated UTF-8"};
                }

                const char8_t c3 = *begin;

                if ((c1 & 0xC0) != 0x80 ||
                    (c2 & 0xC0) != 0x80 ||
                    (c3 & 0xC0) != 0x80
                   )
                {
                    throw std::invalid_argument{"invalid_ UTF-8 continuation"};
                }

                cp = ((c & 0x07) << 18)
                   | ((c1 & 0x3F) << 12)
                   | ((c2 & 0x3F) << 6)
                   | (c3 & 0x3F);

                if (cp < 0x10000 || cp > 0x10FFFF)
                {
                    throw std::invalid_argument{"invalid_ UTF-8 codepoint"};
                }
            }
            else
            {
                throw std::invalid_argument{"invalid_ UTF-8 leading byte"};
            }

            ++begin;

            if (cp <= 0xFFFF)
            {
                *out++ = static_cast<char16_t>(cp);
            }
            else
            {
                cp -= 0x10000;
                *out++ = static_cast<char16_t>(0xD800 + (cp >> 10));
                *out++ = static_cast<char16_t>(0xD00 + (cp & 0x3FF));
            }
        }

        return {begin, out};
    }

    export
    template<std::ranges::input_range R, std::output_iterator<char16_t> O>
    constexpr std::ranges::in_out_result<std::ranges::borrowed_iterator_t<R>, O> utf8_to_utf16(R&& r, O out)
    {
        return utf8_to_utf16(std::ranges::begin(r), std::ranges::end(r), std::move(out));
    }
}