#pragma once

#include <charconv>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

class MalType;
class MalList;
class MalVector;
class MalHashMap;

class Tokenizer {
public:
    Tokenizer(std::string& input)
        : m_input(input)
    {
    }

    std::string_view next()
    {
        auto input_view = std::string_view(m_input);

        while (m_index < m_input.length()) {
            auto c = m_input.at(m_index);
            switch (c) {
            case ' ':
            case '\t':
            case '\n':
            case ',':
                break;
            case '~': {
                if (m_index + 1 < m_input.length() && m_input.at(m_index + 1) == '@') {
                        m_index += 2;
                        return input_view.substr(m_index - 2, 2);
                }
                return input_view.substr(m_index++, 1);
            }
            case '[':
            case ']':
            case '{':
            case '}':
            case '(':
            case ')':
            case '\'':
            case '`':
            case '^':
            case '@':
                return input_view.substr(m_index++, 1);
            case '"': {
                auto first_quote_index = m_index;
                ++m_index;
                bool found_closing_quote = false;
                while (!found_closing_quote && m_index < m_input.length()) {
                    switch (m_input.at(m_index)) {
                        case '\\':
                            if (m_index + 1 < m_input.length() && m_input.at(m_index) == '\\') // Double backslash case, should return them as one.
                                ++m_index;
                            break;
                        case '"':
                            found_closing_quote = true;
                            --m_index;
                            break;
                        default:
                            break;

                    }
                    ++m_index;
                }
                if (m_index >= m_input.length()) {
                    std::cerr << "EOF";
                    return  {};
                }
                ++m_index; // To take the ending " as well
                return input_view.substr(first_quote_index, m_index - first_quote_index);
            }
            case ';': {
                auto semicolon_index = m_index;
                m_index = m_input.length();
                return input_view.substr(semicolon_index, m_input.length() - semicolon_index);
            }
            case '-':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                // TODO: For 123foo, it'll be extracted as '123, foo' tokens, check whether we're okay with that.
                // FIXME: It's wasteful, to use std::from_chars() just to know the length of the number. However, it may be useful for float types.
                long int extracted_number { 0 };
                auto [ptr, error_code] = std::from_chars(input_view.begin() + m_index, input_view.end(), extracted_number);
                if (error_code == std::errc()) {
                    auto first_number_index = m_index;
                    auto number_length = std::distance(input_view.begin() + m_index, ptr);
                    m_index += number_length;
                    return input_view.substr(first_number_index, number_length);
                }
                [[fallthrough]]; // To handle inputs like `-` or `-abc`.
            }
            default: {
                auto first_index = m_index;
                bool special_char_found = false;
                while (!special_char_found && m_index < m_input.length()) {
                    c = m_input.at(m_index);
                    switch (c) {
                    case ' ':
                    case '\t':
                    case '\n':
                    case ',':
                    case '[':
                    case ']':
                    case '{':
                    case '}':
                    case '(':
                    case ')':
                    case '\'':
                    case '`':
                    case '^':
                    case '"':
                    case ';':
                        special_char_found = true;
                        break;
                    default:
                        ++m_index;
                        break;
                    }
                }
                    return input_view.substr(first_index, m_index - first_index);
            }
                std::cerr << "I believe we should never get here!\n";
                break;
            }
            ++m_index;
        }
        return {};
    }
private:
    std::string& m_input;
    size_t m_index { 0 };
};

class Reader {
public:
    Reader(std::vector<std::string_view>& tokens)
        : m_tokens(tokens)
    {
    }

    std::string_view next()
    {
        if (m_index < m_tokens.size())
            return m_tokens.at(m_index++);
        return {};
    }

    std::string_view peek()
    {
        if (m_index < m_tokens.size())
            return m_tokens.at(m_index);
        return {};
    }

private:
    std::vector<std::string_view>& m_tokens;
    size_t m_index { 0 };
};

std::vector<std::string_view> tokenize(std::string& input);
MalType* read_str(std::string& input);
MalType* read_form(Reader& reader);
MalType* read_integer(Reader& reader);
MalList* read_list(Reader& reader);
MalType* read_quote_value(Reader& reader, std::string_view quote_string);
MalType* read_with_meta(Reader& reader);
MalVector* read_vector(Reader& reader);
MalHashMap* read_hash_map(Reader& reader);
MalType* read_atom(Reader& reader);

bool is_number(std::string_view str);