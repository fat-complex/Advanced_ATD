#include "parenthesis_substitution.h"

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace std;

//=================== TokenType ====================================================================
enum class TokenType
{
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

    [[nodiscard]] auto str() const -> string
    {
        stringstream ss;
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

    [[nodiscard]] auto str() const -> string
    {
        stringstream ss;
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

    [[nodiscard]] auto str() const -> string
    {
        stringstream ss;
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
using TokenValue = variant<Number<int>, LeftParentheses, RightParentheses, Plus, Minus, Mul, Div>;

auto str(const TokenValue& token_value) -> string
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

    TokenType token_type;
    TokenValue token_value;

    [[nodiscard]] auto to_string() const -> string
    {
        return str(token_value);
    }
};

static const unordered_map</*raw token type*/ char, ANode> node_factory{
    {'(', ANode{TokenType::Parenthesis, LeftParentheses{}}},
    {')', ANode{TokenType::Parenthesis, RightParentheses{}}},
    {'+', ANode{TokenType::Operator, Plus{}}},
    {'-', ANode{TokenType::Operator, Minus{}}},
    {'*', ANode{TokenType::Operator, Mul{}}},
    {'/', ANode{TokenType::Operator, Div{}}}};

int main()
{
    const auto expr = "7+3/25*(5-2)";
    const auto subst_exp = ParenthesisSubstitution::perform(expr);
    cout << "Substitution expr" << subst_exp << endl;

    vector<ANode> nodes;
    for (auto it = subst_exp.begin(), end = subst_exp.end(); it != end; ++it)
    {
        const char ch = *it;
        if (node_factory.contains(ch))
        {
            nodes.push_back(node_factory.at(ch));
            continue;
        }
        if (isdigit(ch))
        {
            int number{ch - '0'};
            while (isdigit(*++it))
            {
                number *= 10;
                number += (*it) - '0';
            }
            --it;
            nodes.emplace_back(TokenType::Number, Number{number});
        }
    }
    for (auto& node : nodes)
    {
        cout << node.to_string() << " ";
    }

    return 0;
}