#include "printer.h"
#include "types.h"
#include "string_helpers.h"

std::string pr_str(MalType* value, bool print_readably)
{
    return value ? value->to_str(print_readably) : "";
}