#include <string>
#include "types.h"

void MalList::append(MalType* value)
{
    m_elements.push_back(value);
}

std::string MalList::to_str() const
{
    std::string result = "(";

    for (const MalType* value : m_elements)
    {
        result += value->to_str() + ' ';
    }

    if (result.length() > 1)
    {
        result[result.length() - 1] = ')';
    }
    else
    {
        result += ')';
    }

    return result;
}