#include <iostream>
#include <charconv>
#include "reader.h"
#include "types.h"
#include "string_helpers.h"

std::vector<std::string_view> tokenize(const std::string& input)
{
    size_t index = 0;

    auto tokenize_symbol = [&]() -> std::string_view
    {
        std::string_view view {input};
        
        size_t start = index;
        bool done = false;
        char c;
        while (!done && index < input.length())
        {
            c = input.at(index);

            if (std::isspace(c))
            {
                done = true;
                break;
            }
            switch (c)
            {
            case ',':
            case '~':
            case '[':
            case ']':
            case '{':
            case '}':
            case '(':
            case ')':
            case '\'':
            case '"':
            case '`':
            case '^':
            case '@':
            case ';':
                done = true;
                break;
            default:
                ++index;
            }
        }
        return view.substr(start, index - start);
    };

    auto next = [&]() -> std::optional<std::string_view>
    {
        std::string_view view {input};

        while (index < input.length())
        {
            char c = input[index];

            if (std::isspace(c) || c == ',')
            {
                ++index;
            }
            else
            {
                switch (c)
                {
                case '~':
                    ++index;
                    if (index < input.length() && input.at(index) == '@')
                    {
                        ++index;
                        return view.substr(index - 2, 2);
                    }
                    return view.substr(index - 1, 1);
                case '[':
                case ']':
                case '{':
                case '}':
                case '(':
                case ')':
                case '\'':
                case '`':
                case '^':
                case '@':
                    return view.substr(index++, 1);
                case '"':
                {
                    const size_t start = index;
                    ++index;
                    while (index < input.length())
                    {
                        c = input.at(index);
                        switch (c)
                        {
                        case '"':
                            ++index;
                            return view.substr(start, index - start);
                        case '\\':
                            ++index;
                            break;
                        }
                        ++index;
                    }
                    throw UnbalancedToken("unbalanced \"");
                }
                case ';':
                {
                    const size_t start = index;
                    while (index < input.length())
                    {
                        c = input.at(index);
                        if (c == '\n')
                            break;
                        ++index;
                    }
                    return view.substr(start, index - start);
                }
                default:
                    return tokenize_symbol();
                }
            }
        }
        return std::nullopt;
    };

    std::vector<std::string_view> tokens;

    while (auto token = next())
    {
        tokens.push_back(*token);
    }

    return tokens;
}

MalType* read_str(const std::string& input)
{
    auto tokens = tokenize(input);

    Reader reader{tokens};

    return read_form(reader);
}

MalType* read_form(Reader& reader)
{
    if (auto token = reader.peek())
    {
        if (*token == "(")
        {
            return read_list(reader);
        }
        else if (*token == "[")
        {
            return read_vector(reader);
        }
        else if (*token == "{")
        {
            return read_map(reader);
        }
        else
        {
            return read_atom(reader);
        }
    }

    return nullptr;
}

MalList* read_list(Reader& reader)
{
    reader.next(); // consume "("

    MalList* list = new MalList();

    while (auto token = reader.peek())
    {
        if (*token == ")")
        {
            reader.next();
            return list;
        }
        list->append(read_form(reader));
    }

    throw UnbalancedToken("unbalanced )");
}

MalVector* read_vector(Reader& reader)
{
    reader.next(); // consume "["

    MalVector* vector = new MalVector();

    while (auto token = reader.peek())
    {
        if (*token == "]")
        {
            reader.next();
            return vector;
        }
        vector->append(read_form(reader));
    }

    throw UnbalancedToken("unbalanced ]");
}

MalMap* read_map(Reader& reader)
{
    reader.next(); // consume "{"

    MalMap* map = new MalMap();

    while (auto token = reader.peek())
    {
        if (*token == "}")
        {
            reader.next();
            return map;
        }
        auto key = read_form(reader);
        if (auto value = reader.peek())
        {
            map->insert(key, read_form(reader));
        }
    }

    throw UnbalancedToken("unbalanced }");
}

MalType* read_atom(Reader& reader)
{
    auto token = *reader.next();
    
    switch (token[0])
    {
    case '\"':
        return new MalString(deescape_string(std::string(token.substr(1, token.length() - 2))));
    case ':':
        return new MalKeyword(std::string(token));
    case '-':
        if (token.length() == 1 || std::isdigit(token[2]) == false)
        {
            break;
        }
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        {
            int number = 0;
            std::from_chars(token.data(), token.data() + token.size(), number);
            return new MalInteger(number);
        }
        break;
    
    default:
        break;
    }
    
    return new MalSymbol(std::string(token));
}

std::optional<std::string_view> Reader::next()
{
    return m_index < m_tokens.size() ? std::optional<std::string_view>(m_tokens[m_index++]) : std::nullopt;
}

std::optional<std::string_view> Reader::peek() const
{
    return m_index < m_tokens.size() ? std::optional<std::string_view>(m_tokens[m_index]) : std::nullopt;
}