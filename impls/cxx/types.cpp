#include <string>
#include "types.h"

std::string print_elements(std::vector<MalType*> elements, char opening_character, char closing_character, bool print_readably)
{
    std::string result;
    result += opening_character;

    for (const MalType* value : elements)
    {
        result += value->to_str(print_readably) + ' ';
    }

    if (result.length() > 1)
    {
        result[result.length() - 1] = closing_character;
    }
    else
    {
        result += closing_character;
    }

    return result;
}

void MalList::append(MalType* value)
{
    m_elements.push_back(value);
}

std::string MalList::to_str(bool print_readably) const
{
    return print_elements(m_elements, '(', ')', print_readably);
}

void MalVector::append(MalType* value)
{
    m_elements.push_back(value);
}

std::string MalVector::to_str(bool print_readably) const
{
    return print_elements(m_elements, '[', ']', print_readably);
}

void MalMap::insert(MalType* key, MalType* value)
{
    if (key->type() == EType::String || key->type() == EType::Keyword)
    {
        m_map[key] = value;
    }
}

std::string MalMap::to_str(bool print_readably) const
{
    std::string result;
    result += '{';

    for (const auto& kv_pair : m_map)
    {
        result += kv_pair.first->to_str(print_readably) + ' ' + kv_pair.second->to_str(print_readably) + ' ';
    }

    if (result.length() > 1)
    {
        result[result.length() - 1] = '}';
    }
    else
    {
        result += '}';
    }

    return result;
}

MalType* MalFunction::call(span<MalType*> arguments)
{
    return m_function(arguments);
}