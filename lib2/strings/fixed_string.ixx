export module lib2.strings:basic_fixed_string;

import std;

import lib2.meta;
import lib2.compact_optional;

import :character;

namespace lib2
{
    export
    template<character CharT, std::size_t N>
    class basic_fixed_string
    {
    public:
        using value_type                = CharT;
        using size_type                 = std::size_t;
        using difference_type           = std::ptrdiff_t;
        using reference                 = value_type&;
        using const_reference           = const value_type&;
        using pointer                   = value_type*;
        using const_pointer             = const value_type*;
        using iterator                  = pointer;
        using const_iterator            = const_pointer;
        using reverse_iterator          = std::reverse_iterator<iterator>;
        using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

        using npos_type = optional_size;

        static constexpr constexpr_value<N> capacity {};
        static constexpr constexpr_value<N> max_size {};

        constexpr basic_fixed_string() noexcept
            : data_{0}, size_{0} {}

        constexpr basic_fixed_string(const size_type count, const value_type ch)
        {
            assign(count, ch);
        }

        template<std::input_iterator I, std::sentinel_for<I> S>
        constexpr basic_fixed_string(I first, S last)
        {
            assign(std::move(first), std::move(last));
        }

        template<std::ranges::input_range R>
        constexpr basic_fixed_string(const std::from_range_t, R&& r)
            : basic_fixed_string{std::ranges::begin(r), std::ranges::end(r)} {}

        constexpr basic_fixed_string(const CharT* const s, const size_type count)
        {
            assign(s, count);
        }

        constexpr basic_fixed_string(const CharT* const s)
            : basic_fixed_string{s, std::char_traits<CharT>::length(s)} {}

        constexpr basic_fixed_string(std::nullptr_t) = delete;

        template<class S>
            requires(std::convertible_to<const S&, std::basic_string_view<CharT>> && !std::convertible_to<const S&, const CharT*>)
        constexpr basic_fixed_string(const S& s)
        {
            assign(s);
        }

        constexpr basic_fixed_string(const basic_fixed_string&) noexcept = default;

        template<std::size_t N2>
            requires(N2 < max_size())
        constexpr basic_fixed_string(const basic_fixed_string<CharT, N2>& other) noexcept
        {
            assign(other);
        }

        constexpr basic_fixed_string& operator=(const basic_fixed_string&) noexcept = default;

        template<std::size_t N2>
            requires(N2 < max_size())
        constexpr basic_fixed_string& operator=(const basic_fixed_string<CharT, N2>& other) noexcept
        {
            return assign(other);
        }

        constexpr basic_fixed_string& operator=(const CharT* const s)
        {
            return assign(s, std::char_traits<CharT>::length(s));
        }

        constexpr basic_fixed_string& operator=(const CharT ch)
        {
            return assign(std::addressof(ch), 1);
        }

        template<class S>
            requires(std::convertible_to<const S&, std::basic_string_view<CharT>> && !std::convertible_to<const S&, const CharT*>)
        constexpr basic_fixed_string& operator=(const S& s)
        {
            return assign(s);
        }

        basic_fixed_string& operator=(std::nullptr_t) = delete;

        constexpr basic_fixed_string& assign(const size_type count, const value_type ch)
        {
            if (count > max_size())
            {
                throw std::length_error{"basic_fixed_string::assign"};
            }

            size_ = count;
            *std::fill_n(data_, size_, ch) = 0;
            return *this;
        }

        constexpr basic_fixed_string& assign(const value_type* const str, const size_type count)
        {
            if (count > max_size())
            {
                throw std::length_error{"basic_fixed_string::assign"};
            }

            size_ = count;
            *std::copy_n(data_, size_, str) = 0;
            return *this;
        }

        constexpr basic_fixed_string& assign(const value_type* const str)
        {
            return assign(str, std::char_traits<CharT>::length(str));
        }

        template<class S>
            requires(std::convertible_to<const S&, std::basic_string_view<CharT>> && !std::convertible_to<const S&, const CharT*>)
        constexpr basic_fixed_string& assign(const S& str)
        {
            const std::basic_string_view<CharT> view {str};
            return assign(view.data(), view.size());
        }

        template<std::input_iterator I, std::sentinel_for<I> S>
        constexpr basic_fixed_string& assign(I first, S last)
        {
            if constexpr (std::sized_sentinel_for<S, I>)
            {
                const auto new_size {std::ranges::distance(first, last)};
                if (new_size > max_size())
                {
                    throw std::length_error{"basic_fixed_string::assign"};
                }

                std::ranges::copy(std::move(first), std::move(last), data_);
                size_ = new_size;
            }
            else
            {
                size_type new_size {0};
                value_type new_data[max_size()];
                while (first != last && new_size < max_size())
                {
                    new_data[new_size] = *first;
                    ++new_size;
                    ++first;
                }

                if (first != last)
                {
                    throw std::length_error{"basic_fixed_string::assign"};
                }

                std::copy_n(data_, new_size, new_data);
                size_ = new_size;
            }
            data_[size_] = 0;
            return *this;
        }

        template<std::ranges::input_range R>
        constexpr basic_fixed_string& assign_range(R&& r)
        {
            return assign(std::ranges::begin(r), std::ranges::end(r));
        }

        constexpr value_type& at(const size_type pos)
        {
            if (pos >= size_)
            {
                throw std::out_of_range{"basic_fixed_string::at"};
            }

            return data_[pos];
        }

        constexpr const value_type& at(const size_type pos) const
        {
            if (pos >= size_)
            {
                throw std::out_of_range{"basic_fixed_string::at"};
            }

            return data_[pos];
        }

        [[nodiscard]] constexpr value_type& operator[](const size_type pos) noexcept
        {
            return data_[pos];
        }

        [[nodiscard]] constexpr const value_type& operator[](const size_type pos) const noexcept
        {
            return data_[pos];
        }

        [[nodiscard]] constexpr value_type& front() noexcept
        {
            return data_[0];
        }

        [[nodiscard]] constexpr const value_type& front() const noexcept
        {
            return data_[0];
        }

        [[nodiscard]] constexpr value_type& back() noexcept
        {
            return data_[size_ - 1];
        }

        [[nodiscard]] constexpr const value_type& back() const noexcept
        {
            return data_[size_ - 1];
        }

        [[nodiscard]] constexpr const_pointer data() const noexcept
        {
            return data_;
        }

        [[nodiscard]] constexpr pointer data() noexcept
        {
            return data_;
        }

        [[nodiscard]] constexpr const_pointer c_str() const noexcept
        {
            return data_;
        }

        [[nodiscard]] constexpr operator std::basic_string_view<CharT>() const noexcept
        {
            return std::basic_string_view<CharT>{data_, size_};
        }

        [[nodiscard]] constexpr iterator begin() noexcept
        {
            return data_;
        }

        [[nodiscard]] constexpr const_iterator begin() const noexcept
        {
            return data_;
        }

        [[nodiscard]] constexpr const_iterator cbegin() const noexcept
        {
            return data_;
        }

        [[nodiscard]] constexpr iterator end() noexcept
        {
            return data_ + size_;
        }

        [[nodiscard]] constexpr const_iterator end() const noexcept
        {
            return data_ + size_;
        }

        [[nodiscard]] constexpr const_iterator cend() const noexcept
        {
            return data_ + size_;
        }

        [[nodiscard]] constexpr reverse_iterator rbegin() noexcept
        {
            return reverse_iterator{end()};
        }

        [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
        {
            return reverse_iterator{end()};
        }

        [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
        {
            return reverse_iterator{cend()};
        }

        [[nodiscard]] constexpr reverse_iterator rend() noexcept
        {
            return reverse_iterator{begin()};
        }

        [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator{begin()};
        }

        [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator{cbegin()};
        }

        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return size_ == 0;
        }

        [[nodiscard]] constexpr size_type size() const noexcept
        {
            return size_;
        }

        constexpr void clear() noexcept
        {
            size_ = 0;
        }

        constexpr basic_fixed_string& insert(const size_type index, const size_type count, const value_type ch)
        {
            if (index > size_)
            {
                throw std::out_of_range{"basic_fixed_string::insert"};
            }

            const auto new_size {size_ + count};
            if (new_size > max_size())
            {
                throw std::length_error{"basic_fixed_string::insert"};
            }

            std::move_backward(data_ + index, data_ + size_, data_ + new_size);
            *std::fill_n(data_ + index, count, ch) = 0;
            size_ = new_size;
            return *this;
        }

        constexpr basic_fixed_string& insert(const size_type index, const CharT* const str, const size_type count)
        {
            if (index > size_)
            {
                throw std::out_of_range{"basic_fixed_string::insert"};
            }

            const auto new_size {size_ + count};
            if (new_size > max_size())
            {
                throw std::length_error{"basic_fixed_string::insert"};
            }

            std::move_backward(data_ + index, data_ + size_, data_ + new_size);
            *std::copy_n(data_ + index, count, str) = 0;
            size_ = new_size;
            return *this;
        }

        constexpr basic_fixed_string& insert(const size_type index, const CharT* const str)
        {
            return insert(index, str, std::char_traits<CharT>::length(str));
        }

        constexpr iterator insert(const const_iterator pos, const value_type ch)
        {
            const auto idx {convert_iterator(pos)};
            insert(idx, 1, ch);
            return begin() + idx;
        }

        constexpr iterator insert(const const_iterator pos, const size_type count, const value_type ch)
        {
            const auto idx {convert_iterator(pos)};
            insert(idx, count, ch);
            return begin() + idx;
        }


        template<std::input_iterator I, std::sentinel_for<I> S>
        constexpr iterator insert(const const_iterator pos, I first, S last)
        {
            const auto idx {convert_iterator(pos)};
            if constexpr (std::sized_sentinel_for<S, I>)
            {
                const auto count {std::ranges::distance(first, last)};
                
                if (count + size_ > max_size())
                {
                    throw std::length_error{"basic_fixed_string::insert"};
                }

                std::move_backward(data_ + idx, data_ + size_, data_ + size_ + count);
                std::ranges::copy(std::move(first), std::move(last), data_ + idx);
            }
            else
            {
                value_type temp[max_size()];
                size_type count {0};

                while (first != last && count + size_ < max_size())
                {
                    temp[count] = *first;
                    ++count;
                    ++first;
                }

                if (first != last)
                {
                    throw std::length_error{"basic_fixed_string::insert"};
                }

                std::move_backward(data_ + idx, data_ + size_, data_ + size_ + count);
                std::copy_n(data_ + idx, count, temp);
            }

            return begin() + idx;
        }

        template<class S>
            requires(std::convertible_to<const S&, std::basic_string_view<CharT>> && !std::convertible_to<const S&, const CharT*>)
        constexpr basic_fixed_string& insert(const size_type index, const S& str)
        {
            const std::basic_string_view<CharT> view {str};
            return insert(index, view.data(), view.size());
        }

        template<std::ranges::input_range R>
        constexpr iterator insert_range(const const_iterator pos, R&& r)
        {
            return insert(pos - begin(), std::ranges::begin(r), std::ranges::end(r));
        }

        basic_fixed_string& erase(const size_type index = 0, const npos_type count = {})
        {
            if (index > size_)
            {
                throw std::out_of_range{"basic_fixed_string::erase"};
            }

            const auto erase_count {count ? std::min(*count, size_ - index) : size_ - index};
            std::move(data_ + index + erase_count, data_ + size_, data_ + index);
            size_ -= erase_count;
            data_[size_] = 0;
            return *this;
        }

        iterator erase(const const_iterator pos) noexcept
        {
            const auto idx {convert_iterator(pos)};
            erase(idx, 1);
            return begin() + idx;
        }

        iterator erase(const const_iterator first, const const_iterator last) noexcept
        {
            const auto idx_first {convert_iterator(first)};
            const auto idx_last  {convert_iterator(last)};
            erase(idx_first, idx_last - idx_first);
            return begin() + idx_first;
        }

        void push_back(const value_type ch)
        {
            if (size_ + 1 > max_size())
            {
                throw std::length_error{"basic_fixed_string::push_back"};
            }

            data_[size_] = ch;
            ++size_;
            data_[size_] = 0;
        }

        void pop_back() noexcept
        {
            --size_;
            data_[size_] = 0;
        }

        basic_fixed_string& append(const size_type count, const value_type ch)
        {
            const auto new_size {size_ + count};
            if (new_size > max_size())
            {
                throw std::length_error{"basic_fixed_string::append"};
            }

            *std::fill_n(data_ + size_, count, ch) = 0;
            size_ = new_size;
            return *this;
        }

        basic_fixed_string& append(const value_type* const str, const size_type count)
        {
            const auto new_size {size_ + count};
            if (new_size > max_size())
            {
                throw std::length_error{"basic_fixed_string::append"};
            }

            *std::copy_n(data_ + size_, count, str) = 0;
            size_ = new_size;
            return *this;
        }

        basic_fixed_string& append(const value_type* const str)
        {
            return append(str, std::char_traits<CharT>::length(str));
        }

        template<class S>
            requires(std::convertible_to<const S&, std::basic_string_view<CharT>> && !std::convertible_to<const S&, const CharT*>)
        basic_fixed_string& append(const S& str)
        {
            const std::basic_string_view<CharT> view {str};
            return append(view.data(), view.size());
        }

        template<class S>
            requires(std::convertible_to<const S&, std::basic_string_view<CharT>> && !std::convertible_to<const S&, const CharT*>)
        basic_fixed_string& append(const S& str, const size_type pos, const npos_type count = {})
        {
            const std::basic_string_view<CharT> view {str};
            if (pos > view.size())
            {
                throw std::out_of_range{"basic_fixed_string::append"};
            }

            const auto append_count {count ? std::min(*count, view.size() - pos) : view.size() - pos};
            return append(view.data() + pos, append_count);
        }

        template<std::input_iterator InputIt, std::sentinel_for<InputIt> S>
        basic_fixed_string& append(InputIt first, S last)
        {
            if constexpr (std::sized_sentinel_for<S, InputIt>)
            {
                const auto count {std::ranges::distance(first, last)};
                const auto new_size {size_ + count};
                if (new_size > max_size())
                {
                    throw std::length_error{"basic_fixed_string::append"};
                }

                std::ranges::copy(std::move(first), std::move(last), data_ + size_);
                size_ = new_size;
            }
            else
            {
                const auto old_size {size_};
                while (first != last)
                {
                    if (size_ + 1 > max_size())
                    {
                        size_ = old_size;
                        data_[size_] = 0;
                        throw std::length_error{"basic_fixed_string::append"};
                    }

                    data_[size_] = *first;
                    ++size_;
                    ++first;
                }
            }
            data_[size_] = 0;
            return *this;
        }

        template<std::ranges::input_range R>
        basic_fixed_string& append_range(R&& r)
        {
            return append(std::ranges::begin(r), std::ranges::end(r));
        }

        basic_fixed_string& operator+=(const value_type ch)
        {
            return append(1, ch);
        }

        basic_fixed_string& operator+=(const value_type* const str)
        {
            return append(str);
        }

        template<class S>
            requires(std::convertible_to<const S&, std::basic_string_view<CharT>> && !std::convertible_to<const S&, const CharT*>)
        basic_fixed_string& operator+=(const S& str)
        {
            return append(str);
        }

        basic_fixed_string& replace(const const_iterator first, const const_iterator last, const value_type* const str, const size_type str_count)
        {

        }

        basic_fixed_string& replace(const size_type pos, const size_type count, const value_type* const str, const size_type str_count)
        {
            
        }

        template<class S>
            requires(std::convertible_to<const S&, std::basic_string_view<CharT>> && !std::convertible_to<const S&, const CharT*>)
        basic_fixed_string& replace(const size_type pos, const size_type count, const S& str)
        {

        }
    private:
        value_type data_[N + 1];
        size_type  size_;

        constexpr size_type convert_iterator(const const_iterator it) noexcept
        {
            return static_cast<size_type>(std::distance(cbegin(), it));
        }
    };

    export
    template<std::size_t N>
    using fixed_string = basic_fixed_string<char, N>;

    export
    template<std::size_t N>
    using fixed_wstring = basic_fixed_string<wchar_t, N>;

    export
    template<std::size_t N>
    using fixed_u8string = basic_fixed_string<char8_t, N>;

    export
    template<std::size_t N>
    using fixed_u16string = basic_fixed_string<char16_t, N>;

    export
    template<std::size_t N>
    using fixed_u32string = basic_fixed_string<char32_t, N>;
}