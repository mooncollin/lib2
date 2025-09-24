export module lib2.strings:lazy_string;

import std;

import :character;

namespace lib2
{
    export
    template<character CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
    class basic_lazy_string
    {
        static constexpr std::size_t sso_threshold  {
            (sizeof(std::shared_ptr<CharT[]>) / sizeof(CharT)) - sizeof(CharT)
        };

        static constexpr std::size_t ownership_flag {std::size_t{1} << (sizeof(std::size_t) * 8 - 1)};
        static constexpr std::size_t size_mask      {~ownership_flag};

    public:
        using traits_type       = Traits;
        using value_type        = CharT;
        using allocator_type    = Allocator;
        using size_type         = std::allocator_traits<Allocator>::size_type;
        using difference_type   = std::allocator_traits<Allocator>::difference_type;
        using reference         = value_type&;
        using const_reference   = const value_type&;

        [[nodiscard]] static constexpr std::size_t max_size() noexcept
        {
            return std::numeric_limits<std::size_t>::max() >> 1;
        }

        constexpr basic_lazy_string() noexcept(std::is_nothrow_default_constructible_v<Allocator>)
            : ptr{""}, size_{0} {}

        constexpr basic_lazy_string(const basic_lazy_string& other) noexcept(std::is_nothrow_copy_constructible_v<Allocator>)
            : alloc{std::allocator_traits<Allocator>::select_on_container_copy_construction(other.alloc)}
            , size_{other.size_}
        {
            if (owns())
            {
                if (is_heap())
                {
                    std::construct_at(&heap, other.heap);
                }
                else
                {
                    traits_type::copy(sso_buffer, other.sso_buffer, size());
                    sso_buffer[size()] = {};
                }
            }
            else
            {
                ptr = other.ptr;
            }
        }

        constexpr basic_lazy_string(basic_lazy_string&& other) noexcept
            : alloc{std::move(other.alloc)}
            , size_{std::exchange(other.size_, 0)}
        {
            if (owns())
            {
                if (is_heap())
                {
                    std::construct_at(&heap, std::move(other.heap));
                }
                else
                {
                    traits_type::copy(sso_buffer, other.sso_buffer, size() + 1);
                }
            }
            else
            {
                ptr = std::exchange(other.ptr, "");
            }
        }

        constexpr ~basic_lazy_string() noexcept
        {
            if (owns() && is_heap())
            {
                std::destroy_at(&heap);
            }
        }

        template<std::size_t N>
        constexpr basic_lazy_string(const CharT(&str)[N]) noexcept(std::is_nothrow_default_constructible_v<Allocator>)
            : ptr{str}, size_{N - 1}
        {
            static_assert((N - 1) <= max_size(), "string too large");
        }

        template<std::size_t N>
        constexpr basic_lazy_string(const CharT(&str)[N], const Allocator alloc) noexcept(std::is_nothrow_copy_constructible_v<Allocator>)
            : alloc{alloc}, ptr{str}, size_{N - 1}
        {
            static_assert((N - 1) <= max_size(), "string too large");
        }

        constexpr basic_lazy_string(const std::basic_string_view<CharT, Traits> view) noexcept(std::is_nothrow_default_constructible_v<Allocator>)
            : ptr{view.data()}, size_{view.size() & ~ownership_flag} {}

        constexpr basic_lazy_string(const std::basic_string_view<CharT, Traits> view, const Allocator alloc) noexcept(std::is_nothrow_copy_constructible_v<Allocator>)
            : alloc{alloc}, ptr{view.data()}, size_{view.size() & ~ownership_flag} {}

        constexpr explicit basic_lazy_string(const std::basic_string<CharT, Traits, Allocator>& str)
            : alloc{std::allocator_traits<Allocator>::select_on_container_copy_construction(str.get_allocator())}
            , size_{str.size() | ownership_flag}
        {
            if (is_heap())
            {
                std::construct_at(&heap, std::allocate_shared<CharT[]>(alloc, size() + 1));
                traits_type::copy(heap.get(), str.data(), size());
                heap.get()[size()] = {};
            }
            else
            {
                traits_type::copy(sso_buffer, str.data(), size());
                sso_buffer[size()] = {};
            }
        }

        constexpr basic_lazy_string& operator=(basic_lazy_string other) noexcept
        {
            swap(other);
            return *this;
        }

        template<std::size_t N>
        constexpr basic_lazy_string& operator=(const CharT(&str)[N]) noexcept
        {
            swap(basic_lazy_string{str});
            return *this;
        }

        constexpr basic_lazy_string& operator=(const std::basic_string_view<CharT, Traits> sv) noexcept
        {
            swap(basic_lazy_string{sv});
            return *this;
        }

        constexpr basic_lazy_string& operator=(const std::basic_string<CharT, Traits, Allocator>& str)
        {
            swap(basic_lazy_string{str});
            return *this;
        }

        [[nodiscard]] constexpr size_type size() const noexcept
        {
            return size_ & size_mask;
        }

        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return size() == 0;
        }

        constexpr const CharT* data() const noexcept
        {
            if (owns())
            {
                if (is_heap())
                {
                    return heap.get();
                }

                return sso_buffer;
            }

            return ptr;
        }

        constexpr const CharT* begin() const noexcept
        {
            return data();
        }

        constexpr const CharT* end() const noexcept
        {
            return data() + size();
        }

        constexpr std::basic_string_view<CharT, Traits> view() const noexcept
        {
            return {data(), size()};
        }

        constexpr operator std::basic_string_view<CharT, Traits>() const noexcept
        {
            return view();
        }

        constexpr void swap(basic_lazy_string& other) noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
                                                               std::allocator_traits<Allocator>::is_always_equal::value)
        {
            using std::swap;
            
            if (owns())
            {
                if (is_heap())
                {
                    if (other.owns())
                    {
                        if (other.is_heap())
                        {
                            swap(heap, other.heap);
                        }
                        else
                        {
                            auto temp_heap {std::move(heap)};
                            std::destroy_at(&heap);

                            traits_type::copy(sso_buffer, other.sso_buffer, other.size());
                            sso_buffer[other.size()] = {};

                            std::construct_at(&other.heap, std::move(temp_heap));
                        }
                    }
                    else
                    {
                        auto temp_heap {std::move(heap)};
                        std::destroy_at(&heap);
                        ptr = other.ptr;

                        std::construct_at(&other.heap, std::move(temp_heap));
                    }
                }
                else
                {
                    if (other.owns())
                    {
                        if (other.is_heap())
                        {
                            swap(other, *this);
                            return;
                        }
                        else
                        {
                            std::swap_ranges(sso_buffer, sso_buffer + sso_threshold, other.sso_buffer);
                            sso_buffer[other.size()] = {};
                            other.sso_buffer[size()] = {};
                        }
                    }
                    else
                    {
                        const auto other_ptr {other.ptr};
                        traits_type::copy(other.sso_buffer, sso_buffer, size());
                        other.sso_buffer[size()] = {};

                        ptr = other_ptr;
                    }
                }
            }
            else if (other.owns())
            {
                swap(other, *this);
                return;
            }
            else
            {
                swap(ptr, other.ptr);
            }

            if constexpr(std::allocator_traits<Allocator>::propagate_on_container_swap::value)
            {
                swap(alloc, other.alloc);
            }
            
            swap(size_, other.size_);
        }

        [[nodiscard]] constexpr bool operator==(const basic_lazy_string& other) const noexcept
        {
            if (size() != other.size())
            {
                return false;
            }

            return traits_type::compare(data(), other.data(), size()) == 0;
        }

        [[nodiscard]] constexpr bool operator!=(const basic_lazy_string& other) const noexcept
        {
            return !(*this == other);
        }

        [[nodiscard]] constexpr bool operator==(const CharT* const other) const noexcept
        {
            const auto other_size {traits_type::length(other)};
            if (size() != other_size)
            {
                return false;
            }

            return traits_type::compare(data(), other, size()) == 0;
        }

        [[nodiscard]] constexpr bool operator!=(const CharT* const other) const noexcept
        {
            return !(*this == other);
        }

        [[nodiscard]] constexpr bool operator==(const std::basic_string_view<CharT, Traits> other) const noexcept
        {
            if (size() != other.size())
            {
                return false;
            }

            return traits_type::compare(data(), other.data(), size()) == 0;
        }

        [[nodiscard]] constexpr bool operator!=(const std::basic_string_view<CharT, Traits> other) const noexcept
        {
            return !(*this == other);
        }

        template<class Alloc2>
        [[nodiscard]] constexpr bool operator==(const std::basic_string<CharT, Traits, Alloc2>& other) const noexcept
        {
            if (size() != other.size())
            {
                return false;
            }

            return traits_type::compare(data(), other.data(), size()) == 0;
        }

        template<class Alloc2>
        [[nodiscard]] constexpr bool operator!=(const std::basic_string<CharT, Traits, Alloc2>& other) const noexcept
        {
            return !(*this == other);
        }
    private:
        [[msvc::no_unique_address]] Allocator alloc;
        std::size_t size_;

        union
        {
            // May be a view
            const CharT* ptr;

            // May be stack allocated
            char sso_buffer[sso_threshold + 1];

            // May be heap allocated
            std::shared_ptr<CharT[]> heap;
        };

        [[nodiscard]] constexpr bool owns() const noexcept
        {
            return size_ & ownership_flag;
        }

        [[nodiscard]] constexpr bool is_heap() const noexcept
        {
            return size() > sso_threshold;
        }
    };

    export
    template<class CharT, class Traits>
    basic_lazy_string(std::basic_string_view<CharT, Traits>) -> basic_lazy_string<CharT, Traits, std::allocator<CharT>>;

    export
    template<class CharT, std::size_t N>
    basic_lazy_string(const CharT(&)[N]) -> basic_lazy_string<CharT, std::char_traits<CharT>, std::allocator<CharT>>;

    export
    template<class CharT, class Traits, class Allocator>
    basic_lazy_string(const std::basic_string<CharT, Traits, Allocator>&) -> basic_lazy_string<CharT, Traits, Allocator>;

    export
    using lazy_string = basic_lazy_string<char>;
}