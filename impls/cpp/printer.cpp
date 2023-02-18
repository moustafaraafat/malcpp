#include "printer.h"
#include "types.h"

std::string pr_str(MalType* mal_type, bool print_readably)
{
    if (mal_type)
        return mal_type->inspect(print_readably);

    return {};
}