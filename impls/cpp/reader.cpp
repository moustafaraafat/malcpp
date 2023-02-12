#include "reader.h"
#include "types.h"

std::vector<std::string_view> tokenize(std::string& input)
{
    std::vector<std::string_view> tokens;
    Tokenizer tokenizer { input };
    for (auto token = tokenizer.next(); !token.empty(); token = tokenizer.next())
        tokens.push_back(token);

    return tokens;
}

MalType* read_str(std::string& input)
{
    auto tokens = tokenize(input);
    Reader reader { tokens };
    return read_form(reader);
}

MalType* read_form(Reader& reader)
{
    auto token = reader.peek();
    if (token.empty())
        return nullptr;

    if (token == "(")
        return read_list(reader);
    else if (token == "[")
        return read_vector(reader);
    else if (token == "{")
        return read_hash_map(reader);
    else if (token == "\'")
        return read_quote_value(reader, "quote");
    else if (token == "~")
        return read_quote_value(reader, "unquote");
    else if (token == "~@")
        return read_quote_value(reader, "splice-unquote");
    else if (token == "`")
        return read_quote_value(reader, "quasiquote");
    else if (token == "@")
        return read_quote_value(reader, "deref");
    else if (token == "^")
        return read_with_meta(reader);
    else if (token == "nil")
        return read_nil(reader);
    else if (token == "false")
        return read_false(reader);
    else if (token == "true")
        return read_true(reader);
    else if (is_number(token))
        return read_integer(reader);
    else if (token.at(0) == '"')
        return read_string(reader);
    else
        return read_atom(reader);
}

MalList* read_list(Reader& reader)
{
    reader.next(); // Consume the first '('
    auto* list = new MalList();
    for (auto token = reader.peek(); !token.empty(); token = reader.peek()) {
        if (token == ")") {
            reader.next();
            return list;
        }
        list->push(read_form(reader));
    }
    std::cerr << "EOF\n";
    return list;
}

MalType* read_quote_value(Reader& reader, std::string_view quote_string)
{
    reader.next(); // Consume the first quote specifier
    auto* list = new MalList(); // Here, now we made a list, i.e when we 'read' it, it'll be surrounded by ()
    auto* quote = new MalSymbol(quote_string);
    list->push(quote);
    list->push(read_form(reader));
    return list;
}

MalType* read_with_meta(Reader& reader)
{
    // ^{"a" 1} [1 2 3] -> (with-meta [1 2 3] {"a" 1})
    reader.next(); // Consume the first '^'
    auto* list = new MalList(); // Here, now we made a list, i.e when we 'read' it, it'll be surrounded by ()
    auto* quote = new MalSymbol("with-meta");
    list->push(quote);
    auto* read_hash_map = read_form(reader);
    auto* read_vector = read_form(reader);
    // Push the vector first, then tha hash map.
    list->push(read_vector);
    list->push(read_hash_map);
    return list;
}

// TODO: Combine it with read_list().
MalVector* read_vector(Reader& reader)
{
    reader.next(); // Consume the first '['
    auto* vec = new MalVector();

    for (auto token = reader.peek(); !token.empty(); token = reader.peek()) {
        if (token == "]") {
            reader.next();
            return vec;
        }
        vec->push(read_form(reader));
    }
    std::cerr << "EOF\n";
    return vec;
}

MalHashMap* read_hash_map(Reader& reader)
{
    reader.next(); // Consume the first '{'
    auto* hash_map = new MalHashMap();

    for (auto token = reader.peek(); !token.empty(); token = reader.peek()) {
        if (token == "}") {
            reader.next();
            return hash_map;
        }
        auto* key = read_form(reader);

        if (reader.peek() == "}") {
            std::cerr << "EOF. Map value is missing!\n";
            reader.next();
            return hash_map;
        }
        auto* value = read_form(reader);
        hash_map->insert_or_assign(key, value);
    }
    std::cerr << "EOF\n";
    return hash_map;
}

MalType* read_atom(Reader& reader)
{
    return new MalSymbol { reader.next() }; // TODO: Time for a read_symbol()?
}

MalType* read_string(Reader& reader)
{
    return new MalString { reader.next() };
}

MalType* read_nil(Reader& reader)
{
    reader.next();
    return new MalNil();
}

MalType* read_false(Reader& reader)
{
    reader.next();
    return new MalFalse();
}

MalType* read_true(Reader& reader)
{
    reader.next();
    return new MalTrue();
}

MalType* read_integer(Reader& reader)
{
    auto token = reader.next();
    long int extracted_number { 0 };
    auto [ptr, error_code] = std::from_chars(token.begin(), token.end(), extracted_number);
    if (error_code == std::errc())
        return new MalInteger { extracted_number };

    std::cerr << "EOF, read_integer(): error while reading a number. Should never happen!\n";
    return {};
}

bool is_number(std::string_view str)
{
    // A naive way to test if str is a number
    return (str.at(0) >= '0' && str.at(0) <= '9') || (str.at(0) == '-' && str.length() > 1 && (str.at(1) >= '0' && str.at(1) <= '9'));
}