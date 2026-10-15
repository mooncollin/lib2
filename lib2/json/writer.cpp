module lib2.json;

import std;
import lib2.fmt;

namespace lib2::json
{
    void writer::prepare_value()
    {
        if (stack_.empty()) return;

        auto& current {stack_.back()};
        if (!current.is_object)
        {
            if (!current.first)
            {
                output_.put(',');
            }
            current.first = false;
        }
    }

    void writer::start_object()
    {
        prepare_value();
        output_.put('{');
        stack_.emplace_back(true);
    }

    void writer::end_object()
    {
        stack_.pop_back();
        output_.put('}');
    }

    void writer::write_key(const std::string_view key)
    {
        auto& current {stack_.back()};
        if (!current.first)
        {
            output_.put(',');
        }
        current.first = false;

        write_string(key);
        output_.put(':');
    }

    void writer::start_array()
    {
        prepare_value();
        output_.put('[');
        stack_.emplace_back();
    }

    void writer::end_array()
    {
        stack_.pop_back();
        output_.put(']');
    }

    void writer::write_string(const std::string_view value)
    {
        prepare_value();
        output_.put('"');
        escape_string(value);
        output_.put('"');
    }

    void writer::write_number(const double value)
    {
        prepare_value();
        lib2::format_to<"{}">(output_, value);
    }

    void writer::write_bool(const bool value)
    {
        prepare_value();
        output_.write(value ? "true" : "false");
    }

    void writer::write_null()
    {
        prepare_value();
        output_.write("null");
    }

    void writer::escape_string(const std::string_view str)
    {
        for (const char c : str)
        {
            switch (c)
            {
            case '\"': output_.write("\\\""); break;
            case '\\': output_.write("\\\\"); break;
            case '\b': output_.write("\\b"); break;
            case '\f': output_.write("\\f"); break;
            case '\n': output_.write("\\n"); break;
            case '\r': output_.write("\\r"); break;
            case '\t': output_.write("\\t"); break;
            default:
                if (static_cast<unsigned char>(c) < 0x20)
                {
                    lib2::format_to<"\\u{:04x}">(output_, static_cast<int>(c));
                }
                else
                {
                    output_.put(c);
                }
                break;
            }
        }
    }
}