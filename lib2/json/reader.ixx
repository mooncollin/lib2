export module lib2.json:reader;

import std;

import lib2.io;

import :tokenizer;

namespace lib2::json
{
    export
    class reader
    {
    public:
        explicit reader(io::dyn_peekable_source input)
            : tokenizer_{input} {}

        void start_object();
        void end_object();
        std::optional<std::string> next_key();
        bool next_key(std::string_view key);

        void start_array();
        void end_array();
        bool next_element();

        std::string read_string();
        double read_number();
        bool read_bool();
        void read_null();

        token_type peek() const noexcept { return tokenizer_.current_token().type; }

    private:
        void expect(token_type type);
        tokenizer tokenizer_;
    };
}