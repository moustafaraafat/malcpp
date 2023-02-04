#include <iostream>
#include <string>

#include "linenoise.hpp"

#include "reader.h"
#include "printer.h"

constexpr auto g_line_history_path = "line_history.txt";

MalType* READ(std::string& input)
{
    return read_str(input);
}

MalType* EVAL(MalType* input)
{
    return input;
}

std::string PRINT(MalType* input)
{
    return pr_str(input);
}

std::string rep(std::string& input)
{
    auto ast = READ(input);
    auto result = EVAL(ast);
    return PRINT(result);
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