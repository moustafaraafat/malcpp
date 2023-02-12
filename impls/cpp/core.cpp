#include "core.h"
#include "printer.h"

#include <iostream>

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

MalType* list(size_t argc, MalType** argv)
{
    auto new_list = new MalList();
    for (std::size_t i = 0; i < argc; ++i)
        new_list->push(argv[i]);
    return new_list;
}

MalType* is_list([[maybe_unused]]size_t argc, MalType** argv)
{
    assert(argc >= 1);
    if (argv[0]->type() == MalType::Type::List)
        return new MalTrue();
    else
        return new MalFalse();
}

MalType* is_empty([[maybe_unused]]size_t argc, MalType** argv)
{
    assert(argc >= 1);
    if (static_cast<MalList*>(argv[0])->empty())
        return new MalTrue();
    else
        return new MalFalse();
}

MalType* count([[maybe_unused]]size_t argc, MalType** argv)
{
    assert(argc >= 1);
    if (argv[0]->type() == MalType::Type::Nil)
        return new MalInteger { 0 };
    return new MalInteger { static_cast<long int>(static_cast<MalList*>(argv[0])->size()) };
}

MalType* is_equal([[maybe_unused]]size_t argc, MalType** argv)
{
    assert(argc >= 2);
    auto* a = argv[0];
    auto* b = argv[1];
    if (*a == *b)
        return new MalTrue();
    else
        return new MalFalse();
}

MalType* prn([[maybe_unused]]size_t argc, MalType** argv)
{
    if (argc >= 1)
        std::cout << pr_str(argv[0]) << '\n';
    return new MalNil();
}

MalType* is_lt([[maybe_unused]]size_t argc, MalType** argv)
{
    assert(argc >= 2);
    auto* a = static_cast<MalInteger*>(argv[0]);
    auto* b = static_cast<MalInteger*>(argv[1]);
    if (a->value() < b->value())
        return new MalTrue();
    else
        return new MalFalse();
}

MalType* is_lte([[maybe_unused]]size_t argc, MalType** argv)
{
    assert(argc >= 2);
    auto* a = static_cast<MalInteger*>(argv[0]);
    auto* b = static_cast<MalInteger*>(argv[1]);
    if (a->value() <= b->value())
        return new MalTrue();
    else
        return new MalFalse();
}

MalType* is_gt([[maybe_unused]]size_t argc, MalType** argv)
{
    assert(argc >= 2);
    auto* a = static_cast<MalInteger*>(argv[0]);
    auto* b = static_cast<MalInteger*>(argv[1]);
    if (a->value() > b->value())
        return new MalTrue();
    else
        return new MalFalse();
}

MalType* is_gte([[maybe_unused]]size_t argc, MalType** argv)
{
    assert(argc >= 2);
    auto* a = static_cast<MalInteger*>(argv[0]);
    auto* b = static_cast<MalInteger*>(argv[1]);
    if (a->value() >= b->value())
        return new MalTrue();
    else
        return new MalFalse();
}

CoreFunctionContainer create_core_functions()
{
    CoreFunctionContainer core_functions;
    core_functions.insert( { new MalSymbol("+"), new MalFunction (add)} );
    core_functions.insert( { new MalSymbol("-"), new MalFunction (subtract) } );
    core_functions.insert( { new MalSymbol("*"), new MalFunction (multiply) } );
    core_functions.insert( { new MalSymbol("/"), new MalFunction (divide) } );
    core_functions.insert( { new MalSymbol("list"), new MalFunction (list) } );
    core_functions.insert( { new MalSymbol("list?"), new MalFunction (is_list) } );
    core_functions.insert( { new MalSymbol("empty?"), new MalFunction (is_empty) } );
    core_functions.insert( { new MalSymbol("count"), new MalFunction (count) } );
    core_functions.insert( { new MalSymbol("prn"), new MalFunction (prn) } );
    core_functions.insert( { new MalSymbol("="), new MalFunction (is_equal) } );
    core_functions.insert( { new MalSymbol("<"), new MalFunction (is_lt) } );
    core_functions.insert( { new MalSymbol("<="), new MalFunction (is_lte) } );
    core_functions.insert( { new MalSymbol(">"), new MalFunction (is_gt) } );
    core_functions.insert( { new MalSymbol(">="), new MalFunction (is_gte) } );

    return core_functions;
}