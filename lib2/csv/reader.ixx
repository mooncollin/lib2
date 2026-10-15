export module lib2.csv:reader;

import std;

import lib2.io;
import lib2.meta;

import :dialect;

namespace lib2::csv
{
    export
    class reader
    {
    public:
        class iterator;
        friend class iterator;

        class row
        {
        public:
            class iterator
            {
            public:
                using value_type = std::string;
                using difference_type = std::ptrdiff_t;
                using pointer = const std::string*;
                using reference = const std::string&;
                using iterator_category = std::input_iterator_tag;

                reference operator*() const noexcept
                {
                    return current_;
                }
            
                constexpr iterator() noexcept = default;

                iterator& operator++()
                {
                    it_ += it_ < (parent_->data_.data() + parent_->data_.size());
                    next();
                    return *this;
                }

                iterator operator++(int) noexcept
                {
                    auto temp {*this};
                    ++*this;
                    return temp;
                }

                bool operator==(const std::default_sentinel_t) const noexcept
                {
                    return current_.empty();
                }

                bool operator!=(const std::default_sentinel_t) const noexcept
                {
                    return !current_.empty();
                }
            private:
                explicit iterator(const row& parent)
                    : parent_{&parent}, it_{parent.data_.data()}
                {
                    next();
                }

                const row* parent_;
                const char* it_;
                std::string current_;

                void next()
                {
                    current_.clear();
                    const auto end {parent_->data_.data() + parent_->data_.size()};

                    bool in_quotes {false};
                    bool escaped   {false};
                    for (; it_ < end; ++it_)
                    {
                        if (escaped)
                        {
                            escaped = false;
                            continue;
                        }

                        if (*it_ == parent_->parent_->dialect_.escapechar)
                        {
                            escaped = true;
                        }
                        else if (*it_ == parent_->parent_->dialect_.quotechar)
                        {
                            in_quotes = !in_quotes;
                        }
                        else if (!in_quotes && *it_ == parent_->parent_->dialect_.delimiter)
                        {
                            break;
                        }

                        current_.push_back(*it_);
                    }
                }

                friend class row;
            };

            constexpr row() noexcept = default;

            iterator begin() const noexcept
            {
                return iterator{*this};
            }

            static constexpr lib2::constexpr_value<std::default_sentinel> end {};
        private:
            row(reader& parent) noexcept
                : parent_{&parent} {}

            reader* parent_;
            std::string data_;

            friend class reader::iterator;
        };

        class iterator
        {
        public:
            using value_type = row;
            using difference_type = std::ptrdiff_t;
            using pointer = row*;
            using reference = row&;
            using iterator_category = std::input_iterator_tag;

            reference operator*() noexcept
            {
                return row_;
            }

            iterator& operator++()
            {
                row_.data_.clear();
                while (io::getline(row_.parent_->input_, io::string_ref_sink{row_.data_}, '\n'))
                {
                    if (!row_.data_.empty())
                    {
                        break;
                    }
                }

                if (row_.data_.empty())
                {
                    row_.parent_ = nullptr;
                }

                return *this;
            }

            constexpr iterator() noexcept = default;

            bool operator==(const std::default_sentinel_t) const noexcept
            {
                return !row_.parent_;
            }

            bool operator!=(const std::default_sentinel_t) const noexcept
            {
                return row_.parent_;
            }
        private:
            friend class reader;

            iterator(reader& parent) noexcept
                : row_{parent}
            {
                this->operator++();
            }

            row row_;
        };

        reader(io::dyn_peekable_source input, dialect d = {}) noexcept
            : input_{input}, dialect_{std::move(d)} {}

        const lib2::csv::dialect& dialect() const noexcept
        {
            return dialect_;
        }

        iterator begin() noexcept
        {
            return iterator{*this};
        }

        static constexpr lib2::constexpr_value<std::default_sentinel> end {};
    private:
        io::dyn_peekable_source input_;
        lib2::csv::dialect dialect_;
    };
}