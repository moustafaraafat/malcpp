#include <iostream>
#include <string>
#include <unordered_map>

#include "linenoise.hpp"

#include "env.h"
#include "reader.h"
#include "printer.h"
#include "types.h"

constexpr auto g_line_history_path = "line_history.txt";


MalType* READ(std::string& input)
{
    return read_str(input);
}

MalType* EVAL(MalType* ast, Env& env);

MalType* eval_ast(MalType* ast, Env& env)
{
    switch (ast->type()) {
    case MalType::Type::Symbol: {
        return env.get(static_cast<MalSymbol*>(ast));
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

    auto ast_as_list = static_cast<MalList*>(ast);
    if (ast_as_list->empty())
        return ast;

    // The Apply section.
    if (ast_as_list->at(0)->inspect() == "def!") {
        auto* key = static_cast<MalSymbol*>(ast_as_list->at(1));
        auto* value = EVAL(ast_as_list->at(2), env);
        env.set(key, value);
        return value;
    }

    if (ast_as_list->at(0)->inspect() == "let*") {
        // create a new environment using the current environment as the outer value
        Env let_env(&env);
        auto* new_bindings = static_cast<MalList*>(ast_as_list->at(1));
        for (std::size_t i = 0; i < new_bindings->size() - 1; i += 2) {
            // then use the first parameter as a list of new bindings in the "let*" environment
            auto* key = static_cast<MalSymbol*>(new_bindings->at(i));
            // Take the second element of the binding list, call EVAL using the new "let*" environment as the evaluation environment
            auto* value = EVAL(new_bindings->at(i + 1), let_env);
            // then call set on the "let*" environment using the first binding list element as the key and the evaluated second element as the value.
            let_env.set(key, value);
        }
        // Finally, the second parameter (third element) of the original let* form is evaluated using the new "let*" environment and the
        // result is returned as the result of the let* (the new let environment is discarded upon completion).
        return EVAL(ast_as_list->at(2), let_env);
    }

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

std::string rep(std::string& input, Env& env)
{
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

    Env env {nullptr};
    env.set(new MalSymbol("+"), new MalFunction (add));
    env.set(new MalSymbol("-"), new MalFunction (subtract));
    env.set(new MalSymbol("*"), new MalFunction (multiply));
    env.set(new MalSymbol("/"), new MalFunction (divide));

    while (true) {
        std::string input;
        if (linenoise::Readline("user> ", input))
            break;
        std::cout << rep(input, env) << '\n';
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(g_line_history_path);
}