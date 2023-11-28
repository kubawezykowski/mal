#include <iostream>
#include "core.h"
#include "types.h"
#include "printer.h"

namespace core
{
    namespace detail
    {
        void ValidateNumericArguments(span<MalType*> arguments)
        {
            if (arguments.size() != 2)
                throw std::runtime_error("wrong argument count - expected 2");

            if (arguments[0]->type() != EType::Integer || arguments[1]->type() != EType::Integer)
                throw std::runtime_error("wrong argument type - expected Integer");
        }
    }

    const std::unordered_map<std::string, MalType*(*)(span<MalType*>)> ns =
    {
        {"+", add},
        {"-", sub},
        {"*", mul},
        {"/", divide},
        {"pr-str", prstr},
        {"str", str},
        {"prn", prn},
        {"println", println},
        {"list", list},
        {"list?", is_list},
        {"empty?", is_empty},
        {"count", count},
        {"=", equals},
        {"<", less},
        {"<=", less_equals},
        {">", greater},
        {">=", greater_equals},
    };

    MalType* add(span<MalType*> arguments)
    {
        detail::ValidateNumericArguments(arguments);

        return new MalInteger(arguments[0]->as<MalInteger>().value() + arguments[1]->as<MalInteger>().value());
    }

    MalType* sub(span<MalType*> arguments)
    {
        detail::ValidateNumericArguments(arguments);

        return new MalInteger(arguments[0]->as<MalInteger>().value() - arguments[1]->as<MalInteger>().value());
    }

    MalType* mul(span<MalType*> arguments)
    {
        detail::ValidateNumericArguments(arguments);

        return new MalInteger(arguments[0]->as<MalInteger>().value() * arguments[1]->as<MalInteger>().value());
    }

    MalType* divide(span<MalType*> arguments)
    {
        detail::ValidateNumericArguments(arguments);

        return new MalInteger(arguments[0]->as<MalInteger>().value() / arguments[1]->as<MalInteger>().value());
    }

    MalType* prstr(span<MalType*> arguments)
    {
        std::string result;
        for (auto value : arguments)
        {
            result += pr_str(value, true) + " ";
        }

        if (arguments.size() > 0)
            result.pop_back();
        
        return new MalString(result);
    }

    MalType* str(span<MalType*> arguments)
    {
        std::string result;
        for (auto value : arguments)
        {
            result += pr_str(value, false);
        }
        return new MalString(result);
    }

    MalType* prn(span<MalType*> arguments)
    {
        std::string result;
        for (auto value : arguments)
        {
            result += pr_str(value, true) + " ";
        }

        if (arguments.size() > 0)
            result.pop_back();

        std::cout << result << "\n";
        return MalNil::instance();
    }

    MalType* println(span<MalType*> arguments)
    {
        std::string result;
        for (auto value : arguments)
        {
            result += pr_str(value, false) + " ";
        }

        if (arguments.size() > 0)
            result.pop_back();

        std::cout << result << "\n";
        return MalNil::instance();
    }

    MalType* list(span<MalType*> arguments)
    {
        auto list = new MalList();
        for (auto value : arguments)
            list->append(value);

        return list;
    }
    
    MalType* is_list(span<MalType*> arguments)
    {
        if (arguments.size() == 0)
            throw std::runtime_error("wrong argument count - expected 1");

        if (arguments[0]->type() == EType::List)
            return MalTrue::instance();

        return MalFalse::instance();
    }
    
    MalType* is_empty(span<MalType*> arguments)
    {
        if (arguments.size() == 0)
            throw std::runtime_error("wrong argument count - expected 1");

        if (arguments[0]->is_sequence() == false)
            throw std::runtime_error("wrong argument type - expected List or Vector");

        if (arguments[0]->as<MalSequence>().size() == 0)
            return MalTrue::instance();

        return MalFalse::instance();
    }
    
    MalType* count(span<MalType*> arguments)
    {
        if (arguments.size() == 0)
            throw std::runtime_error("wrong argument count - expected 1");

        if (arguments[0]->type() == EType::Nil)
            return new MalInteger(0);

        if (arguments[0]->is_sequence() == false)
            throw std::runtime_error("wrong argument type - expected List or Vector");
        
        return new MalInteger(arguments[0]->as<MalSequence>().size());
    }
    
    MalType* equals(span<MalType*> arguments)
    {
        if (arguments.size() != 2)
            throw std::runtime_error("wrong argument count - expected 2");
        
        if (*arguments[0] == arguments[1])
            return MalTrue::instance();

        return MalFalse::instance();
    }
    
    MalType* less(span<MalType*> arguments)
    {
        detail::ValidateNumericArguments(arguments);

        if (arguments[0]->as<MalInteger>().value() < arguments[1]->as<MalInteger>().value())
            return MalTrue::instance();

        return MalFalse::instance();
    }
    
    MalType* less_equals(span<MalType*> arguments)
    {
        detail::ValidateNumericArguments(arguments);

        if (arguments[0]->as<MalInteger>().value() <= arguments[1]->as<MalInteger>().value())
            return MalTrue::instance();

        return MalFalse::instance();
    }
    
    MalType* greater(span<MalType*> arguments)
    {
        detail::ValidateNumericArguments(arguments);

        if (arguments[0]->as<MalInteger>().value() > arguments[1]->as<MalInteger>().value())
            return MalTrue::instance();

        return MalFalse::instance();
    }
    
    MalType* greater_equals(span<MalType*> arguments)
    {
        detail::ValidateNumericArguments(arguments);

        if (arguments[0]->as<MalInteger>().value() >= arguments[1]->as<MalInteger>().value())
            return MalTrue::instance();

        return MalFalse::instance();
    }
}
