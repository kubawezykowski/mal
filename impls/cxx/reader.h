#include <string>
#include <string_view>
#include <vector>
#include <optional>

class MalType;
class MalList;
class Reader;

std::vector<std::string_view> tokenize(const std::string& input);
MalType* read_str(const std::string& input);
MalType* read_form(Reader& reader);
MalList* read_list(Reader& reader);
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