#pragma once

namespace lib2
{
    export
    template<class CharT>
    class fill_align_parser
    {
    public:
        enum class align_type : CharT
        {
            left   = '<',
            right  = '>',
            center = '^'
        };
 
        constexpr auto parse(basic_format_parse_context<CharT>& ctx) noexcept
        {
            fill_  = ' ';
            align_ = default_align_;

            auto it {ctx.begin()};

            if (it != ctx.end() && *it != '}')
            {
                switch (*it)
                {
                case '<':
                case '>':
                case '^':
                    align_ = static_cast<align_type>(*it);
                    ++it;
                    break;
                default:
                    fill_ = *it;
                    ++it;
                    if (it == ctx.end())
                    {
                        --it;
                    }
                    else
                    {
                        switch (*it)
                        {
                        case '<':
                        case '>':
                        case '^':
                            align_ = static_cast<align_type>(*it);
                            ++it;
                            break;
                        default:
                            --it;
                            fill_ = ' ';
                            break;
                        }
                    }
                    break;
                }
            }

            return it;
        }

        [[nodiscard]] constexpr CharT get_fill() const noexcept
        {
            return fill_;
        }

        constexpr void set_fill(CharT f) noexcept
        {
            fill_ = f;
        }

        [[nodiscard]] constexpr align_type get_align() const noexcept
        {
            return align_;
        }

        constexpr void set_align(align_type a) noexcept
        {
            align_ = a;
        }
    protected:
        constexpr fill_align_parser(align_type default_a = align_type::left) noexcept
            : fill_{' '}, align_{default_a}, default_align_{default_a} {}
    private:
        CharT fill_;
        align_type align_;
        align_type default_align_;
    };

    export
    template<class CharT>
    class sign_parser
    {
    public:
        enum class sign_type : CharT
        {
            plus  = '+',
            minus = '-',
            space = ' '
        };

        constexpr auto parse(basic_format_parse_context<CharT>& ctx) noexcept
        {
            sign_ = sign_type::minus;

            auto it {ctx.begin()};
            const auto end {ctx.end()};

            if (it != end)
            {
                switch (*it)
                {
                case '+':
                case '-':
                case ' ':
                    sign_ = static_cast<sign_type>(*it);
                    ++it;
                    break;
                }
            }

            return it;
        }

        [[nodiscard]] constexpr sign_type get_sign() const noexcept
        {
            return sign_;
        }

        constexpr void set_sign(sign_type s) noexcept
        {
            sign_ = s;
        }
    protected:
        constexpr sign_parser() noexcept = default;
    private:
        sign_type sign_{sign_type::minus};
    };

    export
    template<class CharT>
    class alt_parser
    {
    public:
        [[nodiscard]] constexpr bool is_alt_format() const noexcept
        {
            return alt_;
        }

        constexpr void set_alt_format(const bool a = true) noexcept
        {
            alt_ = a;
        }

        [[nodiscard]] constexpr bool is_zero_format() const noexcept
        {
            return zero_;
        }

        constexpr void set_zero_format(const bool z = true) noexcept
        {
            zero_ = z;
        }
        
        constexpr auto parse(basic_format_parse_context<CharT>& ctx) noexcept
        {
            alt_ = false;
            zero_ = false;

            auto it {ctx.begin()};
            const auto end {ctx.end()};

            if (it != end)
            {
                switch (*it)
                {
                case '#':
                    alt_ = true;
                    ++it;
                    if (it == end || *it != '0')
                    {
                        break;
                    }
                case '0':
                    zero_ = true;
                    ++it;
                }
            }

            return it;
        }
    protected:
        constexpr alt_parser() noexcept = default;
    private:
        bool alt_ {false};
        bool zero_ {false};
    };

    export
    template<class CharT>
    class width_parser
    {
    public:
        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            width_ = 0;
            dynamic_ = false;

            auto it {ctx.begin()};
            const auto end {ctx.end()};

            if (it != end)
            {
                switch (*it)
                {
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    it = parse_num(it, end, width_);
                    break;
                case '{':
                    ++it;
                    if (it == end)
                    {
                        throw std::format_error{"Unmatched '{' in format string"};
                    }
                    dynamic_ = true;
                    switch (*it)
                    {
                    case '}':
                        width_ = ctx.next_arg_id();
                        ++it;
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        it = parse_num(it, end, width_);
                        ctx.check_arg_id(width_);
                        break;
                    default:
                        throw std::format_error{"Unmatched '{' in format string."};
                    }
                    ctx.check_dynamic_spec_integral(width_);
                    break;
                }
            }

            return it;
        }

        constexpr void set_width(const std::size_t w, const bool d = false) noexcept
        {
            width_   = w;
            dynamic_ = d;
        }

        std::size_t get_width(const basic_format_context<CharT>& ctx) const
        {
            if (dynamic_)
            {
                const auto arg {ctx.arg(width_)};
                return arg.visit(lib2::overloaded {
                    [](const unsigned int value) -> std::size_t {
                        return value;
                    }
                ,   [](const unsigned long long int value) -> std::size_t {
                        return value;
                    }
                ,   [](const int value) -> std::size_t {
                        if (value < 0)
                        {
                            throw std::invalid_argument{"Width cannot be negative."};
                        }
                        return static_cast<std::size_t>(value);
                    }
                ,   [](const long long int value) -> std::size_t {
                        if (value < 0)
                        {
                            throw std::invalid_argument{"Width cannot be negative."};
                        }
                        return static_cast<std::size_t>(value);
                    }
                ,   [](const auto&) -> std::size_t {
                        throw std::invalid_argument{"Width argument is not integral."};
                    }
                });
            }

            return width_;
        }
    protected:
        constexpr width_parser() noexcept = default;
    private:
        std::size_t width_ {0};
        bool dynamic_ {false};
    };

    export
    template<class CharT>
    class precision_parser
    {
    public:
        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            precision_.reset();
            dynamic_ = false;

            auto it {ctx.begin()};
            const auto end {ctx.end()};

            if (it != end)
            {
                if (*it == '.')
                {
                    ++it;
                    if (it == end)
                    {
                        throw std::format_error{"Invalid precision format"};
                    }
                    
                    switch (*it)
                    {
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        precision_ = 0;
                        it = parse_num(it, end, *precision_);
                        break;
                    case '{':
                        ++it;
                        if (it == end)
                        {
                            throw std::logic_error{"Unmatched '{' in format string"};
                        }

                        switch (*it)
                        {
                        case '}':
                            precision_ = ctx.next_arg_id();
                            ++it;
                            break;
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            precision_ = 0;
                            it = parse_num(it, end, *precision_);
                            ctx.check_arg_id(*precision_);
                            break;
                        default:
                            throw std::logic_error{"Unmatched '{' in format string."};
                        }
                        dynamic_ = true;
                        ctx.check_dynamic_spec_integral(*precision_);
                        break;
                    }
                }
            }

            return it;
        }

        constexpr void set_no_precision() noexcept
        {
            precision_.reset();
            dynamic_ = false;
        }

        constexpr void set_precision(const std::size_t p, const bool d = false) noexcept
        {
            precision_ = p;
            dynamic_   = d;
        }

        std::optional<std::size_t> get_precision(const basic_format_context<CharT>& ctx) const
        {
            if (dynamic_)
            {
                const auto arg {ctx.arg(*precision_)};
                return arg.visit(lib2::overloaded {
                    [](const unsigned int value) -> std::size_t {
                        return value;
                    }
                ,   [](const unsigned long long int value) -> std::size_t {
                        return value;
                    }
                ,   [](const int value) -> std::size_t {
                        if (value < 0)
                        {
                            throw std::invalid_argument{"Precision cannot be negative."};
                        }
                        return static_cast<std::size_t>(value);
                    }
                ,   [](const long long int value) -> std::size_t {
                        if (value < 0)
                        {
                            throw std::invalid_argument{"Precision cannot be negative."};
                        }
                        return static_cast<std::size_t>(value);
                    }
                ,   [](const auto&) -> std::size_t {
                        throw std::invalid_argument{"Precision argument is not integral."};
                    }
                });
            }

            return precision_;
        }
    protected:
        constexpr precision_parser() noexcept = default;
    private:
        std::optional<std::size_t> precision_;
        bool dynamic_ {false};
    };

    export
    template<class CharT>
    class locale_parser
    {
    public:
        [[nodiscard]] constexpr bool is_locale_formatting() const noexcept
        {
            return locale_;
        }

        constexpr void set_locale_formatting(bool l = true) noexcept
        {
            locale_ = l;
        }

        constexpr auto parse(basic_format_parse_context<CharT>& ctx) noexcept
        {
            locale_ = false;

            auto iter {ctx.begin()};
            if (iter != ctx.end())
            {
                if (*iter == 'L')
                {
                    locale_ = true;
                    ++iter;
                }
            }

            return iter;
        }
    private:
        bool locale_ {false};
    };
}