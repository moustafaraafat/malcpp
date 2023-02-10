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
        Functiion
    };

    virtual std::string inspect() const = 0;
    virtual Type type() const = 0;
};

class MalList : public MalType {
public:
    void push(MalType* mal_type)
    {
        m_list.push_back(mal_type);
    }

    std::string inspect() const override
    {
        std::string result = "(";
        for (auto* mal_type : m_list)
            result.append(mal_type->inspect() + " ");

        if (m_list.size() > 0)
            result[result.length() - 1] = ')';
        else
            result.append(")");
        return result;
    }

    // TODO: Add the other 4? const ones.
    auto begin() { return m_list.begin(); }
    auto end() { return m_list.end(); }

    auto at(size_t index) const { return m_list.at(index); }

    auto data() { return m_list.data(); }

    auto empty() const { return m_list.empty(); }
    auto size() const { return m_list.size(); }

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

    std::string inspect() const override
    {
        std::string result = "[";
        for (auto* mal_type : m_list)
            result.append(mal_type->inspect() + " ");

        if (m_list.size() > 0)
            result[result.length() - 1] = ']';
        else
            result.append("]");
        return result;
    }

    auto begin() { return m_list.begin(); }
    auto end() { return m_list.end(); }

    Type type() const override { return Type::Vector; }

private:
    std::vector<MalType*> m_list { };
};

struct HashMalHashMap {
    std::size_t operator()(MalType* key) const noexcept
    {
        return std::hash<std::string>{}(key->inspect());
    }
};

struct MalHashMapComparator {
    constexpr bool operator()(MalType* lhs, MalType* rhs) const
    {
        return lhs->inspect() == rhs->inspect();
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

    std::string inspect() const override
    {
        std::string result = "{";
        for (auto [key, value] : m_hash_map)
            result.append(key->inspect() + " " + value->inspect() + " ");

        if (m_hash_map.size() > 0)
            result[result.length() - 1] = '}';
        else
            result.append("}");
        return result;
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

    std::string inspect() const override { return m_str; }

    Type type() const override { return Type::Symbol; }

private:
    std::string m_str;
};

class MalNil : public MalType {
public:
    std::string inspect() const override { return "nil"; }

    Type type() const override { return Type::Nil; }
};

class MalFalse : public MalType {
public:
    std::string inspect() const override { return "false"; }

    Type type() const override { return Type::False; }
};

class MalTrue : public MalType {
public:
    std::string inspect() const override { return "true"; }

    Type type() const override { return Type::True; }
};

class MalInteger : public MalType {
public:
    MalInteger(long int long_value)
        : m_long(long_value)
    {
    }


    std::string inspect() const override { return std::to_string(value()); }

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

    Type type() const override { return Type::Functiion; }

    std::string inspect() const override { return "MalFunction"; }

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