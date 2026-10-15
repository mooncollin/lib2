export module lib2.json:tokenizer;

import std;

import lib2.io;

namespace lib2::json
{
    export
    enum class token_type
    {
        lbrace,
        rbrace,
        lbracket,
        rbracket,
        colon,
        comma,
        string,
        number,
        boolean,
        null,
        eof
    };

    export
    struct token
    {
        token_type type;
        std::string value;

        explicit operator bool() const noexcept
        {
            return type != token_type::eof;
        }
    };

    export
    class tokenizer
    {
    public:
        tokenizer(io::dyn_peekable_source input)
            : input_{input}
        {
            next();
        }

        token& current_token() noexcept
        {
            return current_token_;
        }

        const token& current_token() const noexcept
        {
            return current_token_;
        }

        token& next();
    private:
        io::dyn_peekable_source input_;
        token current_token_;
    };
}