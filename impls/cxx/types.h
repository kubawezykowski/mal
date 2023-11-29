#pragma once
#include <vector>
#include "string_helpers.h"
#include "span.h"

class Env;

enum class EType
{
    List,
    Vector,
    Map,
    Symbol,
    Keyword,
    String,
    Nil,
    True,
    False,
    Integer,
    Function,
    UserFunction,
};

class MalType
{
public:
    virtual EType type() const = 0;
    virtual std::string to_str(bool print_readably = false) const = 0;
    virtual bool is_sequence() const { return false; }
    virtual bool operator==(const MalType* other) const = 0;
    bool operator!=(const MalType* other) const { return !(*this == other); }

    template<typename T>
    T& as() { return static_cast<T&>(*this); }

    template<typename T>
    const T& as() const { return static_cast<const T&>(*this); }
};

class MalSymbol : public MalType
{
public:
    MalSymbol(std::string symbol) : m_symbol(symbol) {}
    EType type() const override { return EType::Symbol; }
    std::string to_str(bool) const override { return m_symbol; }
    bool operator==(const MalType* other) const override { return other->type() == EType::Symbol && other->as<MalSymbol>().m_symbol == m_symbol; }
    const std::string& name() const { return m_symbol; }
private:
    std::string m_symbol;
};

class MalKeyword : public MalType
{
public:
    MalKeyword(std::string keyword) : m_keyword(static_cast<char>('\0x7f') + keyword) {}
    EType type() const override { return EType::Keyword; }
    std::string to_str(bool print_readably = false) const override { return print_readably ? m_keyword.substr(1) : m_keyword; }
    bool operator==(const MalType* other) const override { return other->type() == EType::Keyword && other->as<MalKeyword>().m_keyword == m_keyword; }
private:
    std::string m_keyword;
};

class MalString : public MalType
{
public:
    MalString(const std::string& string) : m_string(string) {}
    EType type() const override { return EType::String; }
    std::string to_str(bool print_readably = false) const override { return print_readably ? escape_string(m_string) : m_string; }
    bool operator==(const MalType* other) const override { return other->type() == EType::String && other->as<MalString>().m_string == m_string; }
private:
    std::string m_string;
};

class MalNil : public MalType
{
public:
    EType type() const override { return EType::Nil; }
    std::string to_str(bool) const override { return "nil"; }
    bool operator==(const MalType* other) const override { return other == this; }
    static MalNil* instance() { static MalNil instance; return &instance; }
private:
    MalNil() = default;
};

class MalTrue : public MalType
{
public:
    EType type() const override { return EType::True; }
    std::string to_str(bool) const override { return "true"; }
    bool operator==(const MalType* other) const override { return other == this; }
    static MalTrue* instance() { static MalTrue instance; return &instance; }
private:
    MalTrue() = default;
};

class MalFalse : public MalType
{
public:
    EType type() const override { return EType::False; }
    std::string to_str(bool) const override { return "false"; }
    bool operator==(const MalType* other) const override { return other == this; }
    static MalFalse* instance() { static MalFalse instance; return &instance; }
private:
    MalFalse() = default;
};

class MalInteger : public MalType
{
public:
    MalInteger(int number) : m_number(number) {}
    EType type() const override { return EType::Integer; }
    std::string to_str(bool) const override { return std::to_string(m_number); }
    bool operator==(const MalType* other) const override { return other->type() == EType::Integer && other->as<MalInteger>().m_number == m_number; }
    int value() const { return m_number; }
private:
    int m_number { 0 };
};

class MalSequence : public MalType
{
public:
    void append(MalType*);
    EType type() const override { return EType::List; }
    bool is_sequence() const override { return true; }
    bool operator==(const MalType* other) const override;

    MalType* head() { return m_elements.empty() ? nullptr : m_elements.front(); }
    span<MalType*> tail() { return m_elements.size() > 1 ? span<MalType*>{std::next(m_elements.data()), m_elements.size() - 1} : span<MalType*>{};}

    MalType* at(size_t index) { return m_elements.at(index); }
    const MalType* at(size_t index) const { return m_elements.at(index); }
    size_t size() const { return m_elements.size(); }

    auto begin() { return m_elements.begin(); }
    auto end() { return m_elements.end(); }

    bool is_empty() const { return m_elements.empty(); }
protected:
    std::vector<MalType*> m_elements;
};

class MalList : public MalSequence
{
public:
    EType type() const override { return EType::List; }
    std::string to_str(bool print_readably = false) const override;
};

class MalVector : public MalSequence
{
public:
    EType type() const override { return EType::Vector; }
    std::string to_str(bool print_readably = false) const override;
};

class MalMap : public MalType
{
public:
    void insert(MalType* key, MalType* value);
    EType type() const override { return EType::Map; }
    std::string to_str(bool print_readably = false) const override;
    bool operator==(const MalType* other) const override;

    auto begin() { return m_map.begin(); }
    auto end() { return m_map.end(); }
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

// native function
class MalFunction : public MalType
{
public:
    //using MalFnPtr = MalType*(*)(span<MalType*>);
    using MalFn = std::function<MalType*(span<MalType*>)>;

    MalFunction(MalFn function) : m_function(function) {}
    EType type() const override { return EType::Function; }
    std::string to_str(bool) const override { return "<fn>"; }
    bool operator==(const MalType* other) const override { return other == this; }
    MalType* call(span<MalType*>);
private:
    MalFn m_function;
};

class MalUserFunction : public MalType
{
public:
    MalUserFunction(MalType* body, MalSequence* params, Env* env) : m_body(body), m_params(params), m_env(env) {}
    EType type() const override { return EType::UserFunction; }
    std::string to_str(bool) const override { return "<fn>"; }
    bool operator==(const MalType* other) const override { return other == this; }

    MalType* get_body() const { return m_body; }
    MalSequence* get_params() const { return m_params; }
    Env* get_env() const { return m_env; }

private:
    MalType* m_body;
    MalSequence* m_params;
    Env* m_env;
};