#include "printer.h"
#include "types.h"

std::string pr_str(MalType* value)
{
    return value ? value->to_str() : "";
}