#include <vector>

enum class EType
{
    List,
    Symbol,
    Integer
};

class MalType
{
public:
    virtual EType type() const = 0;
    virtual std::string to_str() const = 0;
};

class MalList : public MalType
{
public:
    void append(MalType*);
    EType type() const override { return EType::List; }
    std::string to_str() const override;
private:
    std::vector<MalType*> m_elements;
};

class MalSymbol : public MalType
{
public:
    MalSymbol(std::string symbol) : m_symbol(symbol) {}
    EType type() const override { return EType::Symbol; }
    std::string to_str() const override { return m_symbol; }
private:
    std::string m_symbol;
};

class MalInteger : public MalType
{
public:
    MalInteger(int number) : m_number(number) {}
    EType type() const override { return EType::Integer; }
    std::string to_str() const override { return std::to_string(m_number); }
private:
    int m_number { 0 };
};