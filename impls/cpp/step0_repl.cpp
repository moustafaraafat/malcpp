#include <iostream>
#include <string>

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
    while (true) {
        std::cout << "user> ";
        std::string input;
        if (!std::getline(std::cin, input))
            break;
        std::cout << rep(input) << '\n';
    }
}