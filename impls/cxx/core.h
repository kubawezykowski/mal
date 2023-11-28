#pragma once
#include <unordered_map>
#include <string>
#include "span.h"

class MalType;

namespace core
{
    extern const std::unordered_map<std::string, MalType*(*)(span<MalType*>)> ns;

    MalType* add(span<MalType*> arguments);
    MalType* sub(span<MalType*> arguments);
    MalType* mul(span<MalType*> arguments);
    MalType* divide(span<MalType*> arguments);
    MalType* prstr(span<MalType*> arguments);
    MalType* str(span<MalType*> arguments);
    MalType* prn(span<MalType*> arguments);
    MalType* println(span<MalType*> arguments);
    MalType* list(span<MalType*> arguments);
    MalType* is_list(span<MalType*> arguments);
    MalType* is_empty(span<MalType*> arguments);
    MalType* count(span<MalType*> arguments);
    MalType* equals(span<MalType*> arguments);
    MalType* less(span<MalType*> arguments);
    MalType* less_equals(span<MalType*> arguments);
    MalType* greater(span<MalType*> arguments);
    MalType* greater_equals(span<MalType*> arguments);
}