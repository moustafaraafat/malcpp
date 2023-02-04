#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

class MalType {
public:
    virtual std::string inspect() { assert(0); }
};

class MalList : public MalType {
public:
    void push(MalType* mal_type)
    {
        m_list.push_back(mal_type);
    }

    std::string inspect() override
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

private:
    std::vector<MalType*> m_list { };
};

class MalVector : public MalType {
public:
    void push(MalType* mal_type)
    {
        m_list.push_back(mal_type);
    }

    std::string inspect() override
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
        return lhs == rhs;
    }
};

class MalHashMap : public MalType {
public:
    void insert_or_assign(MalType* key, MalType* value)
    {
        m_hash_map.insert_or_assign(key, value);
    }

    MalType* find(MalType* key)
    {
        auto value = m_hash_map.find(key);
        if (value != m_hash_map.end())
            return value->second;
        return {};
    }

    std::string inspect() override
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

private:
    std::unordered_map<MalType*, MalType*, HashMalHashMap, MalHashMapComparator> m_hash_map { };
};

class MalSymbol : public MalType {
public:
    MalSymbol(std::string_view str)
        : m_str(str)
    {
    }

    std::string str() { return m_str; }

    std::string inspect() override { return str(); }

private:
    std::string m_str;
};