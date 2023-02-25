#pragma once

#include <cassert>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

class MalType {
public:
    // A quick&dirty non-RTTI solution.
    enum class Type {
        List,
        Vector,
        HashMap,
        Symbol,
        Nil,
        False,
        True,
        Integer,
        Function,
        String,
        Keyword
    };

    std::string type_as_string()
    {
        switch (type()) {
        case Type::List: return "List";
        case Type::Vector: return "Vector";
        case Type::HashMap: return "HashMap";
        case Type::Symbol: return "Symbol";
        case Type::Nil: return "Nil";
        case Type::False: return "False";
        case Type::True: return "True";
        case Type::Integer: return "Integer";
        case Type::Function: return "Function";
        case Type::String: return "String";
        case Type::Keyword: return "Keyword";
        default: return "Unkown!";
        }
    }

    virtual std::string inspect(bool print_readably = false) const = 0;
    virtual Type type() const = 0;
    virtual bool operator==(MalType const&) const = 0;
};

class MalList : public MalType {
public:
    void push(MalType* mal_type)
    {
        m_list.push_back(mal_type);
    }

    std::string inspect(bool print_readably = false) const override
    {
        std::string result = "(";
        for (auto* mal_type : m_list)
            result.append(mal_type->inspect(print_readably) + " ");

        if (m_list.size() > 0)
            result[result.length() - 1] = ')';
        else
            result.append(")");
        return result;
    }

    bool operator==(MalType const& other) const override
    {
        if (type() != other.type() && other.type() != Type::Vector)
            return false;
        if (size() != static_cast<MalList const&>(other).size())
            return false;
        for (std::size_t i = 0; i < size(); ++i) {
            if (*m_list[i] != *static_cast<MalList const&>(other).m_list[i])
                return false;
        }
        return true;
    }

    // TODO: Add the other 4? const ones.
    auto begin() { return m_list.begin(); }
    auto end() { return m_list.end(); }

    auto at(size_t index) const { return m_list.at(index); }

    auto data() { return m_list.data(); }

    auto empty() const { return m_list.empty(); }
    std::size_t size() const { return m_list.size(); }

    Type type() const override { return Type::List; }

private:
    std::vector<MalType*> m_list { };
};

class MalVector : public MalType {
public:
    void push(MalType* mal_type)
    {
        m_list.push_back(mal_type);
    }

    std::string inspect(bool print_readably = false) const override
    {
        std::string result = "[";
        for (auto* mal_type : m_list)
            result.append(mal_type->inspect(print_readably) + " ");

        if (m_list.size() > 0)
            result[result.length() - 1] = ']';
        else
            result.append("]");
        return result;
    }

    bool operator==(MalType const& other) const override
    {
        if (type() != other.type() && other.type() != Type::List)
            return false;
        if (size() != static_cast<MalVector const&>(other).size())
            return false;
        for (std::size_t i = 0; i < size(); ++i) {
            if (*m_list[i] != *static_cast<MalVector const&>(other).m_list[i])
                return false;
        }
        return true;
    }

    auto begin() { return m_list.begin(); }
    auto end() { return m_list.end(); }
    std::size_t size() const { return m_list.size(); }

    Type type() const override { return Type::Vector; }

private:
    std::vector<MalType*> m_list { };
};

struct HashMalHashMap {
    std::size_t operator()(MalType* key) const noexcept
    {
        return std::hash<std::string>{}(key->inspect(false));
    }
};

struct MalHashMapComparator {
    constexpr bool operator()(MalType* lhs, MalType* rhs) const
    {
        return lhs->inspect(false) == rhs->inspect(false);
    }
};

class MalHashMap : public MalType {
public:
    void insert_or_assign(MalType* key, MalType* value)
    {
        m_hash_map.insert_or_assign(key, value);
    }

    MalType* find(MalType* key) const
    {
        auto value = m_hash_map.find(key);
        if (value != m_hash_map.end())
            return value->second;
        return {};
    }

    std::string inspect(bool print_readably = false) const override
    {
        std::string result = "{";
        for (auto [key, value] : m_hash_map)
            result.append(key->inspect(print_readably) + " " + value->inspect(print_readably) + " ");

        if (m_hash_map.size() > 0)
            result[result.length() - 1] = '}';
        else
            result.append("}");
        return result;
    }

    bool operator==(MalType const&) const override
    {
        // TODO
        return false;
    }

    auto begin() { return m_hash_map.begin(); }
    auto end() { return m_hash_map.end(); }

    Type type() const override { return Type::HashMap; }

private:
    std::unordered_map<MalType*, MalType*, HashMalHashMap, MalHashMapComparator> m_hash_map { };
};

class MalSymbol : public MalType {
public:
    MalSymbol(std::string_view str)
        : m_str(str)
    {
    }

    bool operator==(MalType const& other) const override
    {
        if (type() != other.type())
            return false;
        return m_str == static_cast<MalSymbol const&>(other).m_str;
    }

    std::string inspect([[maybe_unused]]bool print_readably = false) const override { return m_str; }

    Type type() const override { return Type::Symbol; }

private:
    std::string m_str;
};

class MalKeyword : public MalType {
public:
    MalKeyword(std::string_view str)
        : m_str(str)
    {
    }

    bool operator==(MalType const& other) const override
    {
        if (type() != other.type())
            return false;
        return m_str == static_cast<MalKeyword const&>(other).m_str;
    }

    std::string inspect([[maybe_unused]]bool print_readably = false) const override { return m_str; }

    Type type() const override { return Type::Keyword; }

private:
    std::string m_str;
};

class MalString : public MalType {
public:
    MalString(std::string_view str)
        : m_str(str)
    {
    }

    bool operator==(MalType const& other) const override
    {
        if (type() != other.type())
            return false;
        return m_str == static_cast<MalString const&>(other).m_str;
    }

    std::string inspect(bool print_readably = false) const override {
        if (!print_readably)
            return m_str;

        std::string result;
        for (auto c : m_str) {
            switch (c) {
                case '"':
                    result += "\\\"";
                    break;
                case '\n':
                    result += "\\n";
                    break;
                case '\\':
                    result += "\\\\";
                    break;
                default:
                    result += c;
                    break;
            }
        }
        result = "\"" + result + "\"";
        return result;
    }

    Type type() const override { return Type::String; }

private:
    std::string m_str;
};

class MalNil : public MalType {
public:
    bool operator==(MalType const& other) const override
    {
        return type() == other.type();
    }

    std::string inspect([[maybe_unused]]bool print_readably = false) const override { return "nil"; }

    Type type() const override { return Type::Nil; }
};

class MalFalse : public MalType {
public:
    bool operator==(MalType const& other) const override
    {
        return type() == other.type();
    }

    std::string inspect([[maybe_unused]]bool print_readably = false) const override { return "false"; }

    Type type() const override { return Type::False; }
};

class MalTrue : public MalType {
public:
    bool operator==(MalType const& other) const override
    {
        return type() == other.type();
    }

    std::string inspect([[maybe_unused]]bool print_readably = false) const override { return "true"; }

    Type type() const override { return Type::True; }
};

class MalInteger : public MalType {
public:
    MalInteger(long int long_value)
        : m_long(long_value)
    {
    }

    bool operator==(MalType const& other) const override
    {
        if (type() != other.type())
            return false;
        return m_long == static_cast<MalInteger const&>(other).m_long;
    }

    std::string inspect([[maybe_unused]]bool print_readably = false) const override { return std::to_string(value()); }

    Type type() const override { return Type::Integer; }

    long int value() const { return m_long; }

private:
    long int m_long { 0 };
};

using MalFunctionPtr = std::function<MalType*(size_t, MalType**)>;

class MalFunction : public MalType {
public:
    MalFunction(MalFunctionPtr function_ptr)
        : m_function_ptr(function_ptr)
    {
    }

    bool operator==(MalType const& other) const override
    {
        if (type() != other.type())
            return false;
        return true;
    }

    Type type() const override { return Type::Function; }

    std::string inspect([[maybe_unused]]bool print_readably = false) const override { return "#<function>"; }

    MalFunctionPtr function_ptr() const { return  m_function_ptr; }

private:
    MalFunctionPtr m_function_ptr;
};

class MalException : public std::exception {
public:
    MalException(std::string_view message)
        : m_message(message)
    {
    }

    char const* what() const noexcept  override { return m_message.c_str(); }

private:
    std::string m_message;
};