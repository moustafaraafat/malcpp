#include <iostream>
#include <string>
#include <unordered_map>

#include "linenoise.hpp"

#include "reader.h"
#include "printer.h"
#include "types.h"

constexpr auto g_line_history_path = "line_history.txt";

using Env = std::unordered_map<MalSymbol*, MalType*, HashMalHashMap, MalHashMapComparator>;

MalType* READ(std::string& input)
{
    return read_str(input);
}

MalType* EVAL(MalType* ast, Env& env);

MalType* eval_ast(MalType* ast, Env& env)
{
    switch (ast->type()) {
    case MalType::Type::Symbol: {
        auto mal_type = env.find(static_cast<MalSymbol*>(ast));
        if (mal_type != env.end())
            return mal_type->second;
        throw new MalException("eval_ast() Couldn't find that symbol! " + ast->inspect());
    }
    case MalType::Type::Vector: {
        auto vector = new MalVector();
        for (auto* mal_type : *static_cast<MalVector*>(ast))
            vector->push(EVAL(mal_type, env));;
        return vector;
    }
    case MalType::Type::List: {
        auto list = new MalList();
        for (auto* mal_type : *(static_cast<MalList*>(ast)))
            list->push(EVAL(mal_type, env));;
        return list;
    }
    case MalType::Type::HashMap: {
        auto hash_map = new MalHashMap();
        for (auto [key, value] : *static_cast<MalHashMap*>(ast))
            hash_map->insert_or_assign(key, EVAL(value, env));
        return hash_map;
    }
    default:
        return ast;
    }
}

MalType* EVAL(MalType* ast, Env& env)
{
    if (ast->type() != MalType::Type::List)
        return eval_ast(ast, env);

    if (static_cast<MalList*>(ast)->empty())
        return ast;

    auto* new_list = static_cast<MalList*>(eval_ast(ast, env));
    auto fn = static_cast<MalFunction*>(new_list->at(0))->function_ptr();
    return fn(new_list->size() - 1, new_list->data() + 1);
}

std::string PRINT(MalType* input)
{
    return pr_str(input);
}

MalType* add(size_t argc, MalType** argv)
{
    assert(argc == 2);

    auto* a = argv[0];
    auto* b = argv[1];

    assert(a->type() == MalType::Type::Integer);
    assert(b->type() == MalType::Type::Integer);

    auto a_value = static_cast<MalInteger*>(a)->value();
    auto b_value = static_cast<MalInteger*>(b)->value();
    return new MalInteger(a_value + b_value);
}

MalType* subtract(size_t argc, MalType** argv)
{
    assert(argc == 2);

    auto* a = argv[0];
    auto* b = argv[1];

    auto a_value = static_cast<MalInteger*>(a)->value();
    auto b_value = static_cast<MalInteger*>(b)->value();
    return new MalInteger(a_value - b_value);
}

MalType* multiply(size_t argc, MalType** argv)
{
    assert(argc == 2);

    auto* a = argv[0];
    auto* b = argv[1];

    assert(a->type() == MalType::Type::Integer);
    assert(b->type() == MalType::Type::Integer);

    auto a_value = static_cast<MalInteger*>(a)->value();
    auto b_value = static_cast<MalInteger*>(b)->value();
    return new MalInteger(a_value * b_value);
}

MalType* divide(size_t argc, MalType** argv)
{
    assert(argc == 2);

    auto* a = argv[0];
    auto* b = argv[1];

    assert(a->type() == MalType::Type::Integer);
    assert(b->type() == MalType::Type::Integer);

    auto a_value = static_cast<MalInteger*>(a)->value();
    auto b_value = static_cast<MalInteger*>(b)->value();
    return new MalInteger(a_value / b_value);
}

std::string rep(std::string& input)
{
    Env env {};
    env.insert( { new MalSymbol("+"), new MalFunction (add) } );
    env.insert( { new MalSymbol("-"), new MalFunction (subtract) } );
    env.insert( { new MalSymbol("*"), new MalFunction (multiply) } );
    env.insert( { new MalSymbol("/"), new MalFunction (divide) } );
    try {
        auto* ast = READ(input);
        auto* result = EVAL(ast, env);
        return PRINT(result);
    } catch (MalException* mal_exception) {
        std::cerr << mal_exception->what() << std::endl;
        return {};
    }
}


int main()
{
    linenoise::LoadHistory(g_line_history_path);

    while (true) {
        std::string input;
        if (linenoise::Readline("user> ", input))
            break;
        std::cout << rep(input) << '\n';
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(g_line_history_path);
}