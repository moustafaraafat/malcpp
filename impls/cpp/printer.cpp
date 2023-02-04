#include "printer.h"
#include "types.h"

std::string pr_str(MalType* mal_type)
{
    if (mal_type)
        return mal_type->inspect();
    return {};
}