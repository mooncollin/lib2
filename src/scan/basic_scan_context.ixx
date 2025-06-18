export module lib2.scan:basic_scan_context;

import std;

import :basic_scan_arg;

namespace lib2
{
    export
    template<std::input_iterator I, std::sentinel_for<I> S, class CharT>
        requires(std::same_as<std::iter_value_t<I>, CharT>)
    class basic_scan_context
    {
    public:
        using char_type = CharT;
        using iterator  = I;
        using sentinel  = S;

        basic_scan_context(I begin_, S end_, basic_scan_args<basic_scan_context>& args, const std::locale l = {}) noexcept(std::is_nothrow_move_constructible_v<I> && std::is_nothrow_move_constructible_v<S>)
            : begin_{std::move(begin_)}, end_{std::move(end_)}, args_{std::addressof(args)}, locale_{l} {}

        basic_scan_context(const basic_scan_context&) = delete;

        constexpr basic_scan_arg<basic_scan_context> arg(const std::size_t id) const
        {
            return args_->get(id);
        }

        std::locale locale() const noexcept
        {
            return locale_;
        }

        I begin() const noexcept
        {
            return begin_;
        }

        S end() const noexcept
        {
            return end_;
        }

        void advance_to(I it) noexcept(std::is_nothrow_move_assignable_v<I>)
        {
            begin_ = std::move(it);
        }
    private:
        I begin_;
        S end_;
        basic_scan_args<basic_scan_context>* args_;
        std::locale locale_;
    };
}