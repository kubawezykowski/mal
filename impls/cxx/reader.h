#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <stdexcept>

class MalType;
class MalList;
class MalVector;
class MalMap;
class MalString;
class Reader;

class UnbalancedToken : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

std::vector<std::string_view> tokenize(const std::string& input);
MalType* read_str(const std::string& input);
MalType* read_form(Reader& reader);
MalList* read_list(Reader& reader);
MalVector* read_vector(Reader& reader);
MalMap* read_map(Reader& reader);
MalType* read_atom(Reader& reader);

class Reader
{
public:
    Reader(std::vector<std::string_view> tokens) : m_tokens(tokens) {}

    std::optional<std::string_view> next();
    std::optional<std::string_view> peek() const;

private:
    std::vector<std::string_view> m_tokens;
    size_t m_index { 0 };
};