#include <string>
#include "linenoise.hpp"

std::string READ(std::string str)
{
	return str;
}

std::string EVAL(std::string str)
{
	return str;
}

std::string PRINT(std::string str)
{
	return str;
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
		std::cout << rep(line) << '\n';

		linenoise::AddHistory(line.c_str());
	}

	linenoise::SaveHistory(path);
}
