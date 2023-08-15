#include "env.h"

void Env::set(const key_t& key, MalType* value)
{
    m_data[key] = value;
}

Env* Env::find(const key_t& key)
{
    return m_data.find(key) != m_data.end() ? this : (m_outer ? m_outer->find(key) : nullptr);
}

MalType* Env::get(const key_t& key)
{
    if (Env* containing_env = find(key))
    {
        return containing_env->m_data.at(key);
    }
    else
    {
        throw SymbolNotFoundException("symbol " + key + " not found");
    }
}