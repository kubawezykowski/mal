#include <string>
#include "linenoise.hpp"
#include "reader.h"
#include "printer.h"
#include "types.h"
#include "env.h"
#include "core.h"

class MalType;

MalType* eval_ast(MalType* ast, Env* env);

MalType* READ(std::string str)
{
    return read_str(str);
}

MalType* EVAL(MalType* ast, Env* env)
{
    while (true)
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
                        env->set(new_symbol.name(), value);
                        return value;
                    }
                    else if (first_symbol.name() == "let*")
                    {
                        if (list.size() != 3)
                        {
                            throw std::runtime_error("let* expected 2 arguments");
                        }
                        auto first_argument = list.at(1);
                        if (first_argument->is_sequence() == false)
                        {
                            throw std::runtime_error("let* expected a sequence as a first argument");
                        }
                        Env* let_env = new Env(env);
                        auto bindings_begin = first_argument->as<MalSequence>().begin();
                        auto bindings_end = first_argument->as<MalSequence>().end();
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
                            let_env->set(binding_symbol.name(), value);
                        }
                        env = let_env;
                        ast = list.at(2);
                        continue; // TCO
                    }
                    else if (first_symbol.name() == "do")
                    {
                        if (list.size() == 1)
                        {
                            throw std::runtime_error("do expected at least one argument");
                        }
                        
                        for (size_t index = 1; index < list.size() - 1; ++index)
                        {
                            EVAL(list.at(index), env);
                        }
                        ast = *list.end();
                        continue; // TCO
                    }
                    else if (first_symbol.name() == "if")
                    {
                        if (list.size() != 3 && list.size() != 4)
                        {
                            throw std::runtime_error("if wrong argument size");
                        }
                        
                        MalType* condition_result = EVAL(list.at(1), env);
                        if (condition_result != MalNil::instance() && condition_result != MalFalse::instance())
                        {
                            ast = list.at(2);
                            continue; // TCO
                        }
                        else if (list.size() == 4)
                        {
                            ast = list.at(3);
                            continue; // TCO
                        }

                        return MalNil::instance();
                    }
                    else if (first_symbol.name() == "fn*")
                    {
                        if (list.size() != 3)
                        {
                            throw std::runtime_error("fn* expected 2 arguments");
                        }
                        auto first_argument = list.at(1);
                        if (first_argument->is_sequence() == false)
                        {
                            throw std::runtime_error("fn* expected a sequence of bindings");
                        }
                        auto binds = &first_argument->as<MalSequence>();
                        auto body = list.at(2);
                        return new MalUserFunction(body, binds, env);
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
                else if (function->type() == EType::UserFunction)
                {
                    auto& user_function = function->as<MalUserFunction>();
                    ast = user_function.get_body();
                    env = new Env(user_function.get_env(), user_function.get_params(), arguments);
                    continue; // TCO
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
}

std::string PRINT(MalType* value)
{
    return pr_str(value, true);
}

MalType* eval_ast(MalType* ast, Env* env)
{
    if (ast->type() == EType::Symbol)
    {
        auto& symbol = ast->as<MalSymbol>();
        return env->get(symbol.name());
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

std::string rep(std::string str, Env* env)
{
    auto ast = READ(str);
    auto result = EVAL(ast, env);
    return PRINT(result);
}

int main()
{
    const auto path = "history.txt";
    linenoise::LoadHistory(path);

    Env repl_env(nullptr);

    for (auto& [symbol, func] : core::ns)
    {
        repl_env.set(symbol, new MalFunction(func));
    }

    rep("(def! not (fn* (a) (if a false true)))", &repl_env);

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
            std::cout << rep(line, &repl_env) << '\n';
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
