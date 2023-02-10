#pragma once

#include <unordered_map>

#include "types.h"

class Env {
public:
    Env(Env* outer)
        : m_outer_env(outer)
    {

    }
    void set(MalSymbol* key, MalType* value)
    {
        m_data.insert_or_assign(key, value);
    }

    Env* find(MalSymbol* key)
    {
        auto value = m_data.find(key);
        // takes a symbol key and if the current environment contains that key then return the environment.
        if (value != m_data.end())
            return this;
        // If no key is found and outer is not nil then call find (recurse) on the outer environment.
        if (m_outer_env)
            return m_outer_env->find(key);

        return {};
    }

    MalType* get(MalSymbol* key)
    {
        auto env = find(key);
        if (env)
            return env->m_data[key]; // I use [] as we checked in find() that the pair exists.
        throw new MalException("'" + key->inspect() + "'" + " not found.");
    }

private:
    std::unordered_map<MalSymbol*, MalType*, HashMalHashMap, MalHashMapComparator> m_data;
    Env* m_outer_env { nullptr };
};