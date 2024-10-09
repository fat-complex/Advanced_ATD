#include "tokenizer.h"

#include "parenthesis_substitution.h"

#include <unordered_map>

namespace
{
const std::unordered_map</*raw token type*/ char, ANode> node_factory{
    {'(', ANode{TokenType::Parenthesis, LeftParentheses{}}},
    {')', ANode{TokenType::Parenthesis, RightParentheses{}}},
    {'+', ANode{TokenType::Operator, Plus{}}},
    {'-', ANode{TokenType::Operator, Minus{}}},
    {'*', ANode{TokenType::Operator, Mul{}}},
    {'/', ANode{TokenType::Operator, Div{}}}};
}

auto tokenize(const std::string& expression) -> std::vector<ANode>
{
    const auto subst_exp = ParenthesisSubstitution::perform(expression);
    std::vector<ANode> tokens;

    for (auto it = subst_exp.begin(), end = subst_exp.end(); it != end; ++it)
    {
        const char ch = *it;
        if (node_factory.contains(ch))
        {
            tokens.push_back(node_factory.at(ch));
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
            tokens.emplace_back(TokenType::Number, Number{number});
        }
    }
    return tokens;
}