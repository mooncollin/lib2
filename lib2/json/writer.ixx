export module lib2.json:writer;

import std;
import lib2.io;

namespace lib2::json
{
    export
    class writer
    {
    public:
        explicit writer(io::dyn_sink output) noexcept
            : output_{output} {}

        void start_object();
        void end_object();
        void write_key(std::string_view key);

        void start_array();
        void end_array();

        void write_string(std::string_view value);
        void write_number(double value);
        void write_bool(bool value);
        void write_null();

    private:
        struct state
        {
            state(const bool is_object = false) noexcept
                : is_object{is_object}, first{true} {}

            bool is_object : 1;
            bool first : 1;
        };

        void prepare_value();
        void escape_string(std::string_view str);

        io::dyn_sink output_;
        std::vector<state> stack_;
    };
}