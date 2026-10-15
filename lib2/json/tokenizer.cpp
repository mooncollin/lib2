module lib2.json;

import std;

import lib2.strings;

import :tokenizer;

namespace lib2::json
{
    token& tokenizer::next()
    {
        io::skipws(input_);

        auto c {input_.get()};
        if (!c)
        {
            current_token_.type = token_type::eof;
            return current_token_;
        }

        switch (*c)
        {
        case ',':
            current_token_.type = token_type::comma;
            break;
        case ':':
            current_token_.type = token_type::colon;
            break;
        case '{':
            current_token_.type = token_type::lbrace;
            break;
        case '}':
            current_token_.type = token_type::rbrace;
            break;
        case '[':
            current_token_.type = token_type::lbracket;
            break;
        case ']':
            current_token_.type = token_type::rbracket;
            break;
        case '"':
            {
                current_token_.value.clear();
                while (!std::empty(input_))
                {
                    io::read_to(input_, io::string_ref_sink{current_token_.value}, [](const auto w) {
                        constexpr std::string_view haystack {"\\\""};
                        return std::ranges::find_first_of(w, haystack);
                    });

                    c = input_.get();
                    if (!c)
                    {
                        throw std::runtime_error{"Unterminated string"};
                    }

                    if (c == '"')
                    {
                        break;
                    }

                    if (c == '\\')
                    {
                        c = input_.get();
                        if (!c)
                        {
                            throw std::runtime_error{"Unterminated string"};
                        }

                        switch (*c)
                        {
                        case '"':
                        case '\\':
                        case '/':
                            current_token_.value.push_back(*c);
                            break;
                        case 'b':
                            current_token_.value.push_back('\b');
                            break;
                        case 'f':
                            current_token_.value.push_back('\f');
                            break;
                        case 'n':
                            current_token_.value.push_back('\n');
                            break;
                        case 'r':
                            current_token_.value.push_back('\r');
                            break;
                        case 't':
                            current_token_.value.push_back('\t');
                            break;
                        default:
                            throw std::runtime_error{"Invalid escape sequence"};
                        }
                    }
                }
                current_token_.type = token_type::string;
                break;
            }
        case '-':
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
                current_token_.value.clear();
                current_token_.value.push_back(*c);

                io::take_while(input_, io::string_ref_sink{current_token_.value}, [](const auto ch) {
                    return isspace_ascii(ch) || ch == '.' || ch == 'e' || ch == 'E' || ch == '+' || ch == '-';
                });
                current_token_.type = token_type::number;
                break;
            }
        case 't': // true
        case 'f': // false
        case 'n': // null
            {
                current_token_.value.clear();
                current_token_.value.push_back(*c);
                io::take_while(input_, io::string_ref_sink{current_token_.value}, isalpha_ascii);

                if (current_token_.value == "true" || current_token_.value == "false")
                    current_token_.type = token_type::boolean;
                else if (current_token_.value == "null")
                    current_token_.type = token_type::null;
                else
                    throw std::runtime_error{"Invalid keyword"};
                break;
            }
        default:
            throw std::runtime_error{"Invalid character"};
        }

        return current_token_;
    }
}