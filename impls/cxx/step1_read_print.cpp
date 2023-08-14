#include <string>
#include "linenoise.hpp"
#include "reader.h"
#include "printer.h"

class MalType;

MalType* READ(std::string str)
{
    return read_str(str);
}

MalType* EVAL(MalType* value)
{
    return value;
}

std::string PRINT(MalType* value)
{
    return pr_str(value, true);
}

std::string rep(std::string str)
{
    auto ast = READ(str);
    auto result = EVAL(ast);
    return PRINT(result);
}

int main()
{
    const auto path = "history.txt";
    linenoise::LoadHistory(path);

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
            std::cout << rep(line) << '\n';
        }
        catch (const UnbalancedTokenException& e)
        {
            std::cerr << "Runtime error: " << e.what() << '\n';
        }
        catch (const EmptyTokenListException& e)
        {
            // do nothing
        }

        linenoise::AddHistory(line.c_str());
    }

    linenoise::SaveHistory(path);
}
