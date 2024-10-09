#pragma once

#include <sstream>
#include <string>
#include <variant>
#include <vector>

//=================== TokenType ====================================================================
enum class TokenType
{
    Unknown,
    Parenthesis,
    Operator,
    Number
};
//
//==================================================================================================

//=================== Number =======================================================================
template <typename T = int>
struct Number
{
    T value;

    [[nodiscard]] auto str() const -> std::string
    {
        std::stringstream ss;
        ss << "[" << "Number, " << value << "]";
        return ss.str();
    }
};
//
//==================================================================================================

//=================== Brackets =====================================================================
template <char brackets_tag>
struct Brackets
{
    char value{brackets_tag};

    [[nodiscard]] auto str() const -> std::string
    {
        std::stringstream ss;
        ss << "[" << "Bracket, " << value << "]";
        return ss.str();
    }
};

template <char brackets_tag>
using Parenthesis = Brackets<brackets_tag>;

using LeftParentheses = Parenthesis<'('>;
using RightParentheses = Parenthesis<')'>;
//
//==================================================================================================

//=================== Operator =====================================================================
template <char operator_tag>
struct Operator
{
    char value{operator_tag};

    [[nodiscard]] auto str() const -> std::string
    {
        std::stringstream ss;
        ss << "[" << "Operator, " << value << "]";
        return ss.str();
    }
};

using Plus = Operator<'+'>;
using Minus = Operator<'-'>;
using Mul = Operator<'*'>;
using Div = Operator<'/'>;
//
//==================================================================================================

//=================== TokenValue ===================================================================
using TokenValue = std::variant<Number<>, LeftParentheses, RightParentheses, Plus, Minus, Mul, Div>;

inline auto str(const TokenValue& token_value) -> std::string
{
    auto visitor = [&](auto&& value) { return value.str(); };
    return std::visit(std::move(visitor), token_value);
}
//
//==================================================================================================

class ANode
{
public:
    ANode(const TokenType token_type, const TokenValue token_value)
        : token_type(token_type)
        , token_value(token_value)
    {
    }
    ANode() = default;

    TokenType token_type{TokenType::Unknown};
    TokenValue token_value;

    [[nodiscard]] auto to_string() const -> std::string
    {
        return str(token_value);
    }
};

template <class... Ts>
struct Overloaded : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

template <typename... F>
auto make_visitors(F &&... visitor)
{
    return Overloaded{std::forward<F>(visitor)...};
}

auto tokenize(const std::string& expression) -> std::vector<ANode>;