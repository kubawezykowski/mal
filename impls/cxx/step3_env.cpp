#include <string>
#include "linenoise.hpp"
#include "reader.h"
#include "printer.h"
#include "types.h"
#include "env.h"

class MalType;

MalType* eval_ast(MalType* ast, Env& env);

MalType* READ(std::string str)
{
    return read_str(str);
}

MalType* EVAL(MalType* ast, Env& env)
{
    if (ast->type() == EType::List)
    {
        auto& list =  ast->as<MalList>();
        if (list.is_empty())
        {
            return ast;
        }
        else
        {
            if (list.head()->type() == EType::Symbol)
            {
                auto& first_symbol = list.head()->as<MalSymbol>();
                if (first_symbol.name() == "def!")
                {
                    if (list.size() != 3)
                    {
                        throw std::runtime_error("def! expected 2 arguments");
                    }
                    auto first_argument = list.at(1);
                    if (first_argument->type() != EType::Symbol)
                    {
                        throw std::runtime_error("def! expected a symbol as a first argument");
                    }
                    auto& new_symbol = first_argument->as<MalSymbol>();
                    auto value = EVAL(list.at(2), env);
                    env.set(new_symbol.name(), value);
                    return value;
                }
                else if (first_symbol.name() == "let*")
                {
                    if (list.size() != 3)
                    {
                        throw std::runtime_error("let* expected 2 arguments");
                    }
                    auto first_argument = list.at(1);
                    if (first_argument->type() != EType::List && first_argument->type() != EType::Vector)
                    {
                        throw std::runtime_error("let* expected a list or vector as a first argument");
                    }
                    Env let_env(&env);
                    const bool is_bindings_a_list = first_argument->type() == EType::List;
                    auto bindings_begin = is_bindings_a_list ? first_argument->as<MalList>().begin() : first_argument->as<MalVector>().begin();
                    auto bindings_end = is_bindings_a_list ? first_argument->as<MalList>().end() : first_argument->as<MalVector>().end();
                    for (auto element = bindings_begin; element != bindings_end; ++element)
                    {
                        if ((*element)->type() != EType::Symbol)
                        {
                            throw std::runtime_error("let* expected symbol as an element");
                        }
                        auto& binding_symbol = (*element)->as<MalSymbol>();
                        ++element;
                        if (element == bindings_end)
                        {
                            throw std::runtime_error("let* missing value for binding " + binding_symbol.name());
                        }
                        auto value = EVAL(*element, let_env);
                        let_env.set(binding_symbol.name(), value);
                    }
                    return EVAL(list.at(2), let_env);
                }
            }
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
        return env.get(symbol.name());
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
        throw std::runtime_error("wrong argument count - expected 2");
    
    auto first = arguments[0];
    auto second = arguments[1];

    if (first->type() != EType::Integer || second->type() != EType::Integer)
        throw std::runtime_error("wrong argument type - expected Integer");

    return new MalInteger(first->as<MalInteger>().value() + second->as<MalInteger>().value());
}

MalType* sub(span<MalType*> arguments)
{
    if (arguments.size() != 2)
        throw std::runtime_error("wrong argument count - expected 2");
    
    auto first = arguments[0];
    auto second = arguments[1];

    if (first->type() != EType::Integer || second->type() != EType::Integer)
        throw std::runtime_error("wrong argument type - expected Integer");

    return new MalInteger(first->as<MalInteger>().value() - second->as<MalInteger>().value());
}

MalType* mul(span<MalType*> arguments)
{
    if (arguments.size() != 2)
        throw std::runtime_error("wrong argument count - expected 2");
    
    auto first = arguments[0];
    auto second = arguments[1];

    if (first->type() != EType::Integer || second->type() != EType::Integer)
        throw std::runtime_error("wrong argument type - expected Integer");

    return new MalInteger(first->as<MalInteger>().value() * second->as<MalInteger>().value());
}

MalType* div(span<MalType*> arguments)
{
    if (arguments.size() != 2)
        throw std::runtime_error("wrong argument count - expected 2");
    
    auto first = arguments[0];
    auto second = arguments[1];

    if (first->type() != EType::Integer || second->type() != EType::Integer)
        throw std::runtime_error("wrong argument type - expected Integer");

    return new MalInteger(first->as<MalInteger>().value() / second->as<MalInteger>().value());
}

int main()
{
    const auto path = "history.txt";
    linenoise::LoadHistory(path);

    Env repl_env(nullptr);

    repl_env.set("+", new MalFunction(add));
    repl_env.set("-", new MalFunction(sub));
    repl_env.set("*", new MalFunction(mul));
    repl_env.set("/", new MalFunction(div));

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
            std::cout << rep(line, repl_env) << '\n';
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
