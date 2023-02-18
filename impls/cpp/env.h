#pragma once

#include <unordered_map>

#include "types.h"

class Env {
public:
    Env(Env* outer, MalList* binds = nullptr, MalList* exprs = nullptr)
        : m_outer_env(outer)
    {
        if (!binds || !exprs)
            return;
        for (std::size_t i = 0; i < binds->size(); ++i) {
            if (binds->at(i)->inspect() == "&") {
                auto * rest_of_exprs = new MalList();
                for (std::size_t j = i; j < exprs->size(); ++j)
                    rest_of_exprs->push(exprs->at(j));
                set(static_cast<MalSymbol*>(binds->at(i+1)), rest_of_exprs);
                break;
            }
            set(static_cast<MalSymbol*>(binds->at(i)), exprs->at(i));
        }
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