#include <iostream>
#include <string>

#include "linenoise.hpp"

constexpr auto g_line_history_path = "line_history.txt";

std::string READ(std::string const& input)
{
    return input;
}

std::string EVAL(std::string const& input)
{
    return input;
}

std::string PRINT(std::string const& input)
{
    return input;
}

std::string rep(std::string const& input)
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