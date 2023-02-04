#pragma once

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
MalList* read_list(Reader& reader);
MalType* read_quote_value(Reader& reader, std::string_view quote_string);
MalType* read_with_meta(Reader& reader);
MalVector* read_vector(Reader& reader);
MalHashMap* read_hash_map(Reader& reader);
MalType* read_atom(Reader& reader);