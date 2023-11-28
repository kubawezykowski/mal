#include <string>
#include "linenoise.hpp"
#include "reader.h"
#include "printer.h"
#include "types.h"

class MalType;
class Env
{
public:
    MalType* find_symbol_value(const std::string& symbol_name) const
    {
        if (auto it = m_env.find(symbol_name); it != m_env.end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }

    void add_symbol_value(const std::string& symbol_name, MalType* value)
    {
        m_env[symbol_name] = value;
    }

private:
    std::unordered_map<std::string, MalType*> m_env;
};

class UnknownSymbolException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class WrongArgumentCountException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class WrongArgumentTypeException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

MalType* eval_ast(MalType* ast, Env& env);

MalType* READ(std::string str)
{
    return read_str(str);
}

MalType* EVAL(MalType* ast, Env& env)
{
    if (ast->type() == EType::List)
    {
        if (ast->as<MalList>().is_empty())
        {
            return ast;
        }
        else
        {
            auto evaluated_list = eval_ast(ast, env);
            auto& list = evaluated_list->as<MalList>();
            
            auto function = list.head();
            auto arguments = list.tail();

            if (function->type() == EType::Function)
            {
                return function->as<MalFunction>().call(arguments);
            }
            else
            {
                throw std::runtime_error("noncallable first list element");
            }
        }
    }
    else
    {
        return eval_ast(ast, env);
    }
}

std::string PRINT(MalType* value)
{
    return pr_str(value, true);
}

MalType* eval_ast(MalType* ast, Env& env)
{
    if (ast->type() == EType::Symbol)
    {
        auto& symbol = ast->as<MalSymbol>();
        if (auto value = env.find_symbol_value(symbol.name()))
        {
            return value;
        }
        else
        {
            throw UnknownSymbolException("unknown symbol " + symbol.name());
        }
    }
    else if (ast->type() == EType::List)
    {
        auto& list = ast->as<MalList>();
        auto evaluated_list = new MalList();
        for (MalType* element : list)
        {
            evaluated_list->append(EVAL(element, env));
        }
        return evaluated_list;
    }
    else if (ast->type() == EType::Vector)
    {
        auto& vector = ast->as<MalVector>();
        auto evaluated_vector = new MalVector();
        for (MalType* element : vector)
        {
            evaluated_vector->append(EVAL(element, env));
        }
        return evaluated_vector;
    }
    else if (ast->type() == EType::Map)
    {
        auto& map = ast->as<MalMap>();
        auto evaluated_map = new MalMap();
        for (std::pair<MalType*, MalType*> kv : map)
        {
            evaluated_map->insert(kv.first, EVAL(kv.second, env));
        }
        return evaluated_map;
    }
    else
    {
        return ast;
    }
}

std::string rep(std::string str, Env& env)
{
    auto ast = READ(str);
    auto result = EVAL(ast, env);
    return PRINT(result);
}

MalType* add(span<MalType*> arguments)
{
    if (arguments.size() != 2)
        throw WrongArgumentCountException("wrong argument count - expected 2");
    
    auto first = arguments[0];
    auto second = arguments[1];

    if (first->type() != EType::Integer || second->type() != EType::Integer)
        throw WrongArgumentTypeException("wrong argument type - expected Integer");

    return new MalInteger(first->as<MalInteger>().value() + second->as<MalInteger>().value());
}

MalType* sub(span<MalType*> arguments)
{
    if (arguments.size() != 2)
        throw WrongArgumentCountException("wrong argument count - expected 2");
    
    auto first = arguments[0];
    auto second = arguments[1];

    if (first->type() != EType::Integer || second->type() != EType::Integer)
        throw WrongArgumentTypeException("wrong argument type - expected Integer");

    return new MalInteger(first->as<MalInteger>().value() - second->as<MalInteger>().value());
}

MalType* mul(span<MalType*> arguments)
{
    if (arguments.size() != 2)
        throw WrongArgumentCountException("wrong argument count - expected 2");
    
    auto first = arguments[0];
    auto second = arguments[1];

    if (first->type() != EType::Integer || second->type() != EType::Integer)
        throw WrongArgumentTypeException("wrong argument type - expected Integer");

    return new MalInteger(first->as<MalInteger>().value() * second->as<MalInteger>().value());
}

MalType* divide(span<MalType*> arguments)
{
    if (arguments.size() != 2)
        throw WrongArgumentCountException("wrong argument count - expected 2");
    
    auto first = arguments[0];
    auto second = arguments[1];

    if (first->type() != EType::Integer || second->type() != EType::Integer)
        throw WrongArgumentTypeException("wrong argument type - expected Integer");

    return new MalInteger(first->as<MalInteger>().value() / second->as<MalInteger>().value());
}

int main()
{
    const auto path = "history.txt";
    linenoise::LoadHistory(path);

    Env env;

    env.add_symbol_value("+", new MalFunction(add));
    env.add_symbol_value("-", new MalFunction(sub));
    env.add_symbol_value("*", new MalFunction(mul));
    env.add_symbol_value("/", new MalFunction(divide));

    while (true)
    {
        std::string line;
        auto quit = linenoise::Readline("user> ", line);

        if (quit)
        {
            break;
        }

        try 
        {
            std::cout << rep(line, env) << '\n';
        }
        catch (const EmptyTokenListException& e)
        {
            // do nothing
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << "Runtime error: " << e.what() << '\n';
        }

        linenoise::AddHistory(line.c_str());
    }

    linenoise::SaveHistory(path);
}
