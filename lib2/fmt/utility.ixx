export module lib2.fmt:utility;

import std;

import lib2.io;

namespace lib2
{
    export
    template<class It>
    constexpr std::size_t parse_arg_id(It& begin, const It end)
    {
        std::size_t arg_id {0};

        while (begin != end)
        {
            switch (*begin)
            {
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
            {
                const auto new_arg_id {(arg_id * 10) + (*begin - '0')};
                if (new_arg_id < arg_id)
                {
                    throw std::format_error{"Invalid format string: Number too large"};
                }
                arg_id = new_arg_id;
                break;
            }
            default:
                return arg_id;
            }

            ++begin;
        }

        return arg_id;
    }

    export
    struct char_fmt;

    export
    struct str_fmt;

    export
    struct int_fmt;

    export
    struct float_fmt;

    export
    struct pointer_fmt;

    export
    struct bool_fmt;

    template<class R>
    struct range_fmt;

    export
    template<std::integral T>
    int_fmt format_of(T);

    export
    template<std::floating_point T>
    float_fmt format_of(T);

    export
    pointer_fmt format_of(const void*);

    export
    str_fmt format_of(const char*);

    export
    char_fmt format_of(char);

    export
    bool_fmt format_of(bool);

    export
    template<std::ranges::range R>
    range_fmt<R> format_of(R&& r);

    // Primitives must have their format_of defined in this file to be ADL-enabled
    // All other types must be declared in their namespace for ADL to find it.
    export
    template<class T>
    using format_of_t = decltype(format_of(std::declval<const T&>()));

    export
    template<class T, class Fmt>
    concept formattable_with = requires(text_ostream& os, const T& value, const Fmt& fmt)
    {
        { to_stream(os, value, fmt) } -> std::same_as<text_ostream&>;
    };

    export
    template<class T>
    concept formattable = formattable_with<T, format_of_t<T>>;

    template<class T>
    struct assert_formattable
    {
        static_assert(formattable<T>, "Type is not formattable");
    };
}