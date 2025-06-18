export module lib2.scan:basic_scan_parse_context;

import std;

import :scan_error;

namespace lib2
{
    export
    template<class CharT>
    class basic_scan_parse_context
    {
        enum class indexing_mode
        {
            none,
            automatic,
            manual
        };

    public:
        using char_type      = CharT;
        using iterator       = std::basic_string_view<CharT>::const_iterator;
        using const_iterator = iterator;

        constexpr explicit basic_scan_parse_context(const std::basic_string_view<CharT> fmt) noexcept
            : begin_{fmt.begin()}, end_{fmt.end()}, num_args_{0}, mode_{indexing_mode::none} {}

        basic_scan_parse_context(const basic_scan_parse_context&) = delete;

        constexpr const_iterator begin() const noexcept
        {
            return begin_;
        }

        constexpr const_iterator end() const noexcept
        {
            return end_;
        }

        constexpr void advance_to(const const_iterator it) noexcept
        {
            begin_ = it;
        }

        constexpr std::size_t next_arg_id()
        {
            if (mode_ == indexing_mode::manual)
            {
                throw scan_parse_error{"Cannot mix indexing modes"};
            }

            mode_ = indexing_mode::automatic;
            return num_args_++;
        }

        constexpr void check_arg_id(const std::size_t id)
        {
            if (mode_ == indexing_mode::automatic)
            {
                throw scan_parse_error{"Cannot mix indexing modes"};
            }

            mode_ = indexing_mode::manual;
        }
    private:
        const_iterator begin_;
        const_iterator end_;
        std::size_t num_args_;
        indexing_mode mode_;
    };
}