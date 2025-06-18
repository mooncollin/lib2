export module lib2.io:iostream;

import :character;
import :ostream;
import :istream;

namespace lib2
{
    export
    template<class T>
    class basic_iostream : public basic_ostream<T>, public basic_istream<T>
    {
    public:
        using value_type = T;

        virtual ~basic_iostream() noexcept = default;
    protected:
        basic_iostream() noexcept = default;
        basic_iostream(const basic_iostream&) noexcept = default;

        void swap(basic_iostream& other) noexcept
        {
            basic_istream<T>::swap(other);
            basic_ostream<T>::swap(other);
        }
    };

    export
    using byte_iostream = basic_iostream<std::byte>;

    export
    template<io_character CharT>
    using basic_text_iostream = basic_iostream<CharT>;

    export
    using text_iostream = basic_text_iostream<char>;

    export
    using wtext_iostream = basic_text_iostream<wchar_t>;
}