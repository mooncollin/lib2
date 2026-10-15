module lib2.json;

import std;

namespace lib2::json
{
    void reader::expect(const token_type type)
    {
        if (tokenizer_.current_token().type != type)
        {
            throw std::runtime_error{"Unexpected token"};
        }
    }

    void reader::start_object()
    {
        expect(token_type::lbrace);
        tokenizer_.next();
    }

    void reader::end_object()
    {
        expect(token_type::rbrace);
        tokenizer_.next();
    }

    std::optional<std::string> reader::next_key()
    {
        if (tokenizer_.current_token().type == token_type::rbrace)
        {
            return {};
        }

        if (tokenizer_.current_token().type == token_type::comma)
        {
            tokenizer_.next();
        }

        expect(token_type::string);
        std::string key {std::move(tokenizer_.current_token().value)};
        
        tokenizer_.next();
        expect(token_type::colon);
        tokenizer_.next();
        
        return key;
    }

    bool reader::next_key(const std::string_view expected_key)
    {
        const auto key {next_key()};
        if (!key || *key != expected_key)
        {
            return false;
        }
        return true;
    }

    void reader::start_array()
    {
        expect(token_type::lbracket);
        tokenizer_.next();
    }

    void reader::end_array()
    {
        expect(token_type::rbracket);
        tokenizer_.next();
    }

    bool reader::next_element()
    {
        if (tokenizer_.current_token().type == token_type::rbracket)
        {
            return false;
        }

        if (tokenizer_.current_token().type == token_type::comma)
        {
            tokenizer_.next();
        }

        return true;
    }

    std::string reader::read_string()
    {
        expect(token_type::string);
        const std::string val {std::move(tokenizer_.current_token().value)};
        tokenizer_.next();
        return val;
    }

    double reader::read_number()
    {
        expect(token_type::number);
        double val;
        std::from_chars(
            tokenizer_.current_token().value.data(),
            tokenizer_.current_token().value.data() + tokenizer_.current_token().value.size(),
            val
        );
        tokenizer_.next();
        return val;
    }

    bool reader::read_bool()
    {
        expect(token_type::boolean);
        const bool val {tokenizer_.current_token().value == "true"};
        tokenizer_.next();
        return val;
    }

    void reader::read_null()
    {
        expect(token_type::null);
        tokenizer_.next();
    }
}