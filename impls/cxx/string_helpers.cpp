#include "string_helpers.h"

std::string escape_string(const std::string& str)
{
    std::string result = "\"";

    for (char c : str)
    {
        switch (c)
        {
        case '\"':
            result += "\\\"";
            break;
        case '\\':
            result += "\\\\";
            break;
        case '\n':
            result += "\\n";
            break;
        default:
            result += c;
            break;
        }
    }

    result += "\"";
    return result;
}

std::string deescape_string(const std::string& str)
{
    std::string result;

    for (size_t i = 0; i < str.length(); ++i)
    {
        const char c = str[i];

        if (c == '\\' && i + 1 < str.length())
        {
            switch (str[i+1])
            {
            case '\"':
                result += '\"';
                ++i;
                break;
            case '\\':
                result += "\\";
                ++i;
                break;
            case 'n':
                result += "\n";
                ++i;
                break;
            default:
                result += c;
                break;
            }
        }
        else
        {
            result += c;
        }
    }

    return result;
}