#include "env.h"
#include "types.h"

Env::Env(Env* outer, MalSequence* binds, MalList* exprs)
    : m_outer(outer)
{
    auto exprs_it = exprs->begin();

    for (size_t i = 0; i < binds->size(); ++i)
    {
        auto symbol = binds->at(i);
        if (symbol->type() != EType::Symbol)
            throw std::runtime_error("env: expected a symbol");

        auto& symbol_name = symbol->as<MalSymbol>().name();
        if (symbol_name == "&")
        {
            if (i + 1 >= binds->size() || binds->at(i + 1)->type() != EType::Symbol)
                throw std::runtime_error("env: expected a symbol");

            auto& rest_symbol_name = binds->at(i + 1)->as<MalSymbol>().name();
            auto rest = new MalList();
            while (exprs_it != exprs->end())
            {
                rest->append(*exprs_it);
                ++exprs_it;
            }
            set(rest_symbol_name, rest);
            break;
        }

        if (exprs_it == exprs->end())
            throw std::runtime_error("env: not enough arguments");

        set(symbol_name, *exprs_it);
        ++exprs_it;
    }
}

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