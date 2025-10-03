#pragma once

namespace lib2
{
    export
    template<class CharT>
    class range_fill_align_parser : public fill_align_parser<CharT>
    {
    public:
        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            const auto it {fill_align_parser<CharT>::parse(ctx)};

            switch (this->get_fill())
            {
            case '{':
            case '}':
            case ':':
                throw std::format_error{"Invalid fill character"};
            }

            return it;
        }
    protected:
        using fill_align_parser<CharT>::fill_align_parser;
    };

    template<class Tuple, class CharT>
    class tuple_formatter : public range_fill_align_parser<CharT>
    {
    public:
        enum class tuple_type
        {
            normal,
            memberwise
        };
        
        constexpr tuple_formatter() noexcept
            : n_{false}, tuple_type_{tuple_type::normal}, separator_{STATICALLY_WIDEN(CharT, ", ")}, opening_bracket_{STATICALLY_WIDEN(CharT, "[")}, closing_bracket_{STATICALLY_WIDEN(CharT, "]")} {}

        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            n_ = false;
            tuple_type_ = tuple_type::normal;
            separator_ = STATICALLY_WIDEN(CharT, ", ");
            opening_bracket_ = STATICALLY_WIDEN(CharT, "[");
            closing_bracket_ = STATICALLY_WIDEN(CharT, "]");

            auto it {range_fill_align_parser<CharT>::parse(ctx)};
            ctx.advance_to(it);

            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'n':
                    ++it;
                    n_ = true;
                    break;
                case 'm':
                    ++it;
                    set_tuple_type(tuple_type::memberwise);
                    break;
                }
            }

            return it;
        }

        void format(const Tuple& tup, basic_format_context<CharT>& ctx) const
        {
            auto& os {ctx.stream()};

            switch (tuple_type_)
            {
            case tuple_type::normal:
            {
                if (!n_)
                {
                    os << opening_bracket_;
                }

                std::apply(overloaded{
                    [](){},
                    [&]<class T, class... Ts>(const T& v1, const Ts&... vs) {
                        formatter<T, CharT>{}.format(v1, ctx);
                        ((os << separator_, formatter<Ts, CharT>{}.format(vs, ctx)), ...);
                    }
                }, tup);

                if (!n_)
                {
                    os << closing_bracket_;
                }
                break;
            }
            case tuple_type::memberwise:
                if constexpr (std::tuple_size_v<Tuple> == 2)
                {
                    formatter<std::remove_cvref_t<std::tuple_element_t<0, Tuple>>, CharT>{}.format(std::get<0>(tup), ctx);
                    os << STATICALLY_WIDEN(CharT, ": ");
                    formatter<std::remove_cvref_t<std::tuple_element_t<1, Tuple>>, CharT>{}.format(std::get<1>(tup), ctx);
                }
                break;
            }
        }

        constexpr void set_no_brackets(const bool n = true) noexcept
        {
            n_ = n;
        }

        [[nodiscard]] constexpr bool is_no_brackets() const noexcept
        {
            return n_;
        }

        constexpr void set_tuple_type(const tuple_type t)
        {
            if constexpr (std::tuple_size_v<Tuple> != 2)
            {
                if (t == tuple_type::memberwise)
                {
                    throw std::format_error{"Invalid format"};
                }
            }

            tuple_type_ = t;
        }

        [[nodiscard]] constexpr tuple_type get_tuple_type() const noexcept
        {
            return tuple_type_;
        }

        constexpr void set_separator(const std::basic_string_view<CharT> sep) noexcept
        {
            separator_ = sep;
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> get_separator() const noexcept
        {
            return separator_;
        }

        constexpr void set_opening_bracket(const std::basic_string_view<CharT> opening) noexcept
        {
            opening_bracket_ = opening;
        }

        constexpr void set_closing_bracket(const std::basic_string_view<CharT> closing) noexcept
        {
            closing_bracket_ = closing;
        }

        constexpr void set_brackets(const std::basic_string_view<CharT> opening, const std::basic_string_view<CharT> closing) noexcept
        {
            set_opening_bracket(opening);
            set_closing_bracket(closing);
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> get_opening_bracket() const noexcept
        {
            return opening_bracket_;
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> get_closing_bracket() const noexcept
        {
            return closing_bracket_;
        }
        
    private:
        bool n_;
        tuple_type tuple_type_;

        std::basic_string_view<CharT> separator_;
        std::basic_string_view<CharT> opening_bracket_;
        std::basic_string_view<CharT> closing_bracket_;
    };

    export
    template<class CharT, formattable<CharT> T1, formattable<CharT> T2>
    struct formatter<std::pair<T1, T2>, CharT> : public tuple_formatter<std::pair<T1, T2>, CharT> {};

    export
    template<class CharT, formattable<CharT>... Ts>
    struct formatter<std::tuple<Ts...>, CharT> : public tuple_formatter<std::tuple<Ts...>, CharT> {};

    export
    template<class T, class CharT = char>
        requires(std::same_as<std::remove_cvref_t<T>, T> && formattable<T, CharT>)
    class range_formatter : public range_fill_align_parser<CharT>
    {
    public:
        enum class range_type
        {
            normal,
            memberwise,
            string,
            escaped_string
        };

        constexpr range_formatter() noexcept(std::is_nothrow_default_constructible_v<formatter<T, CharT>>)
            : n_{false}, range_type_{range_type::normal}, separator_{STATICALLY_WIDEN(CharT, ", ")}, opening_bracket_{STATICALLY_WIDEN(CharT, "[")}, closing_bracket_{STATICALLY_WIDEN(CharT, "]")} {}

        constexpr auto parse(basic_format_parse_context<CharT>& ctx)
        {
            n_ = false;
            range_type_ = range_type::normal;
            separator_ = STATICALLY_WIDEN(CharT, ", ");
            opening_bracket_ = STATICALLY_WIDEN(CharT, "[");
            closing_bracket_ = STATICALLY_WIDEN(CharT, "]");

            auto it {range_fill_align_parser<CharT>::parse(ctx)};
            ctx.advance_to(it);

            if (it != ctx.end() && *it == 'n')
            {
                ++it;
                n_ = true;
            }

            if (it != ctx.end())
            {
                switch (*it)
                {
                case 'm':
                    ++it;
                    set_range_type(range_type::memberwise);
                    break;
                case '?':
                    ++it;
                    if (it == ctx.end() || *it != 's' || n_)
                    {
                        throw std::format_error{"Invalid format"};
                    }
                    set_range_type(range_type::escaped_string);
                    break;
                case 's':
                    if (n_)
                    {
                        throw std::format_error{"Invalid format"};
                    }
                    ++it;
                    set_range_type(range_type::string);
                    break;
                }
            }
            
            if (it != ctx.end() && *it == ':')
            {
                switch (range_type_)
                {
                case range_type::string:
                case range_type::escaped_string:
                    throw std::format_error{"Invalid format"};
                }

                ++it;
                ctx.advance_to(it);
                it = underlying_.parse(ctx);
            }

            return it;
        }

        template<std::ranges::input_range R>
            requires(formattable<std::ranges::range_reference_t<R>, CharT> && std::same_as<std::remove_cvref_t<std::ranges::range_reference_t<R>>, T>)
        void format(R&& r, basic_format_context<CharT>& ctx) const
        {
            auto& os {ctx.stream()};

            switch (range_type_)
            {
            case range_type::normal:
            {
                if (!n_)
                {
                    os << opening_bracket_;
                }

                auto it {std::ranges::begin(r)};
                const auto end {std::ranges::end(r)};
                if (it != end)
                {
                    underlying_.format(*it, ctx);
                    ++it;
                }

                for (; it != end; ++it)
                {
                    os << separator_;
                    underlying_.format(*it, ctx);
                }

                if (!n_)
                {
                    os << closing_bracket_;
                }
                break;
            }
            case range_type::memberwise:
                os.put('{');
                auto it {std::ranges::begin(r)};
                const auto end {std::ranges::end(r)};
                if (it != end)
                {
                    underlying_.format(*it, ctx);
                    ++it;
                }

                for (; it != end; ++it)
                {
                    os << STATICALLY_WIDEN(CharT, ", ");
                    underlying_.format(*it, ctx);
                }
                os.put('}');
                break;
            case range_type::string:
                if constexpr (std::convertible_to<T, CharT>)
                {
                    if constexpr (std::ranges::contiguous_range<R>)
                    {
                        os.write(std::ranges::data(r), std::ranges::size(r));
                    }
                    else
                    {
                        for (const CharT ch : r)
                        {
                            os << ch;
                        }
                    }
                }
                break;
            case range_type::escaped_string:
                if constexpr (std::convertible_to<T, CharT>)
                {
                    os << escaped(std::forward<R>(r));
                }
                break;
            }
        }

        constexpr void set_no_brackets(const bool n = true) noexcept
        {
            n_ = n;
        }

        [[nodiscard]] constexpr bool is_no_brackets() const noexcept
        {
            return n_;
        }

        constexpr void set_range_type(const range_type t)
        {
            if constexpr (!specialization_of<T, std::pair> && !specialization_of<T, std::tuple>)
            {
                if (t == range_type::memberwise)
                {
                    throw std::format_error{"Invalid format"};
                }
            }
            else
            {
                if (t == range_type::memberwise)
                {
                    underlying_.set_memberwise();
                }
            }
            
            if constexpr (specialization_of<T, std::tuple>)
            {
                if constexpr (std::tuple_size_v<T> != 2)
                {
                    if (t == range_type::memberwise)
                    {
                        throw std::format_error{"Invalid format"};
                    }
                }
                else
                {
                    if (t == range_type::memberwise)
                    {
                        underlying_.set_memberwise();
                    }
                }
            }

            if constexpr (!std::convertible_to<T, CharT>)
            {
                switch (t)
                {
                case range_type::string:
                case range_type::escaped_string:
                    throw std::format_error{"Invalid format"};
                }
            }

            range_type_ = t;
        }

        [[nodiscard]] constexpr range_type get_range_type() const noexcept
        {
            return range_type_;
        }

        constexpr void set_separator(const std::basic_string_view<CharT> sep) noexcept
        {
            separator_ = sep;
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> get_separator() const noexcept
        {
            return separator_;
        }

        constexpr void set_opening_bracket(const std::basic_string_view<CharT> opening) noexcept
        {
            opening_bracket_ = opening;
        }

        constexpr void set_closing_bracket(const std::basic_string_view<CharT> closing) noexcept
        {
            closing_bracket_ = closing;
        }

        constexpr void set_brackets(const std::basic_string_view<CharT> opening, const std::basic_string_view<CharT> closing) noexcept
        {
            set_opening_bracket(opening);
            set_closing_bracket(closing);
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> get_opening_bracket() const noexcept
        {
            return opening_bracket_;
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> get_closing_bracket() const noexcept
        {
            return closing_bracket_;
        }

        [[nodiscard]] constexpr formatter<T, CharT>& underlying() noexcept
        {
            return underlying_;
        }

        [[nodiscard]] constexpr const formatter<T, CharT>& underlying() const noexcept
        {
            return underlying_;
        }
    private:
        bool n_;
        range_type range_type_;

        formatter<T, CharT> underlying_;
        std::basic_string_view<CharT> separator_;
        std::basic_string_view<CharT> opening_bracket_;
        std::basic_string_view<CharT> closing_bracket_;
    };
}