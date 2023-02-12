#pragma once

#include "types.h"

using CoreFunctionContainer = std::unordered_map<MalSymbol*, MalType*, HashMalHashMap, MalHashMapComparator>;

CoreFunctionContainer create_core_functions();