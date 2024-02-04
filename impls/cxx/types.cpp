#include <string>
#include "types.h"
#include "mal.h"
#include "env.h"

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

void MalSequence::append(MalType* value)
{
    m_elements.push_back(value);
}

bool MalSequence::operator==(const MalType* other) const
{
    if (other->is_sequence())
    {
        auto& other_sequence = other->as<MalSequence>();
        if (other_sequence.size() == size())
        {
            for (size_t i = 0; i < size(); ++i)
            {
                if (*at(i) != other_sequence.at(i))
                {
                    return false;
                }
            }
            return true;
        }
    }

    return false;
}

std::string MalList::to_str(bool print_readably) const
{
    return print_elements(m_elements, '(', ')', print_readably);
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

bool MalMap::operator==(const MalType*) const
{
    return false;
}

MalType* MalFunction::call(span<MalType*> arguments)
{
    return m_function(arguments);
}

MalType* MalUserFunction::call(span<MalType*> arguments)
{
    return EVAL(m_body, new Env(m_env, m_params, arguments));
}