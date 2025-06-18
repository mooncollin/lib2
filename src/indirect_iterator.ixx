export module lib2.indirect_iterator;

import std;

namespace lib2
{
    export
    template<std::input_iterator It>
    class indirect_iterator
    {
    public:
        static_assert(std::indirectly_readable<std::iter_value_t<It>>, "Iterator's value must be dereferenceable");

        using difference_type   = std::iter_difference_t<It>;
        using value_type        = std::iter_value_t<std::iter_value_t<It>>;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = typename std::iterator_traits<It>::iterator_category;

        constexpr indirect_iterator() noexcept(std::is_nothrow_default_constructible_v<It>) = default;

        constexpr indirect_iterator(It it) noexcept(std::is_nothrow_move_constructible_v<It>)
            : it_{std::move(it)} {}

        constexpr reference operator*() const
        {
            return **it_;
        }

        pointer operator->() noexcept
        {
            return std::to_address(*it_);
        }

        indirect_iterator& operator++()
        {
            ++it_;
            return *this;
        }

        indirect_iterator operator++(int)
        {
            auto copy {*this};
            ++it_;
            return copy;
        }

        constexpr bool operator==(const indirect_iterator& rhs) const = default;
        constexpr bool operator!=(const indirect_iterator& rhs) const = default;
    private:
        It it_;
    };
}