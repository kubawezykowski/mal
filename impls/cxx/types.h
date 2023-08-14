#pragma once
#include <vector>
#include "string_helpers.h"

enum class EType
{
    List,
    Vector,
    Map,
    Symbol,
    Keyword,
    String,
    Integer
};

class MalType
{
public:
    virtual EType type() const = 0;
    virtual std::string to_str(bool print_readably = false) const = 0;

    template<typename T>
    T* as() { return static_cast<T*>(this); }

    template<typename T>
    const T* as() const { return static_cast<const T*>(this); }
};

class MalList : public MalType
{
public:
    void append(MalType*);
    EType type() const override { return EType::List; }
    std::string to_str(bool print_readably = false) const override;
private:
    std::vector<MalType*> m_elements;
};

class MalSymbol : public MalType
{
public:
    MalSymbol(std::string symbol) : m_symbol(symbol) {}
    EType type() const override { return EType::Symbol; }
    std::string to_str(bool) const override { return m_symbol; }
private:
    std::string m_symbol;
};

class MalKeyword : public MalType
{
public:
    MalKeyword(std::string keyword) : m_keyword(static_cast<char>('\0x7f') + keyword) {}
    EType type() const override { return EType::Keyword; }
    std::string to_str(bool print_readably = false) const override { return print_readably ? m_keyword.substr(1) : m_keyword; }
private:
    std::string m_keyword;
};

class MalString : public MalType
{
public:
    MalString(const std::string& string) : m_string(string) {}
    EType type() const override { return EType::String; }
    std::string to_str(bool print_readably = false) const override { return print_readably ? escape_string(m_string) : m_string; }
private:
    std::string m_string;
};

class MalInteger : public MalType
{
public:
    MalInteger(int number) : m_number(number) {}
    EType type() const override { return EType::Integer; }
    std::string to_str(bool) const override { return std::to_string(m_number); }
private:
    int m_number { 0 };
};

class MalVector : public MalType
{
public:
    void append(MalType*);
    EType type() const override { return EType::Vector; }
    std::string to_str(bool print_readably = false) const override;
private:
    std::vector<MalType*> m_elements;
};

class MalMap : public MalType
{
public:
    void insert(MalType* key, MalType* value);
    EType type() const override { return EType::Map; }
    std::string to_str(bool print_readably = false) const override;
private:

    struct MalTypeHash
    {
        std::size_t operator()(const MalType* t) const
        {
            return std::hash<std::string>{}(t->to_str());
        }
    };
    struct MalTypeEqual
    {
        bool operator()(const MalType* lhs, const MalType* rhs) const 
        {
            return lhs->type() == rhs->type() && lhs->to_str() == rhs->to_str();
        }
    };
    std::unordered_map<MalType*, MalType*, MalTypeHash, MalTypeEqual> m_map;
};