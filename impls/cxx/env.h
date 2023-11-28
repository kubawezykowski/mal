#pragma once
#include <string>

class MalType;
class MalList;
class MalSequence;

class Env
{
public:
    using key_t = std::string;

    class SymbolNotFoundException : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    Env(Env* outer) : m_outer(outer) {}
    Env(Env* outer, MalSequence* binds, MalList* exprs);

    void set(const key_t& key, MalType* value);
    Env* find(const key_t& key);
    MalType* get(const key_t& key);

private:
    Env* m_outer { nullptr };
    std::unordered_map<key_t, MalType*> m_data;
};