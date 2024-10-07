#include "parenthesis_substitution.h"

#include <functional>
#include <stack>
#include <unordered_set>

using namespace std;

namespace
{
auto extract_from_stack_if(stack<char>& stack, const function<bool()>& predicate) -> string
{
    string result{};
    while (!stack.empty() and predicate())
    {
        result += stack.top();
        stack.pop();
    }
    return result;
}

auto is_operator(const char ch) -> bool
{
    static auto ops = unordered_set{'+', '-', '*', '/'};
    return ops.contains(ch);
}

auto precedence(const char ch) -> int
{
    static auto precedences = unordered_map<char, int>{{'+', 1}, {'-', 2}, {'*', 3}, {'/', 4}};
    return precedences.contains(ch) ? precedences[ch] : -1;
}

auto to_postfix_notation(const string& expression) -> string
{
    string result{};
    stack<char> stack;
    for (const auto& ch : expression)
    {
        if (isdigit(ch))
        {
            result += ch;
            continue;
        }
        if (ch == '(')
        {
            stack.push(ch);
        }
        if (ch == ')')
        {
            result += extract_from_stack_if(stack, [&] { return stack.top() != '('; });
            stack.pop();
            continue;
        }
        if (is_operator(ch))
        {
            result += extract_from_stack_if(
                stack, [&] { return precedence(ch) <= precedence(stack.top()); });
            stack.push(ch);
            result += " ";
        }
    }
    result += extract_from_stack_if(stack, [&] { return true; });
    return result;
}

} // namespace

auto ParenthesisSubstitution::perform(const string& expression) -> string
{
    auto postfix = to_postfix_notation(expression);
    stack<string> expression_stack;
    for (auto it = postfix.begin(); it != postfix.end();)
    {
        if (isdigit(*it))
        {
            string digit{*it};
            while (isdigit(*++it))
            {
                digit += *it;
            }
            expression_stack.push(digit);
            continue;
        }
        if (is_operator(*it))
        {
            auto op1 = expression_stack.top();
            expression_stack.pop();
            auto op2 = expression_stack.top();
            expression_stack.pop();
            expression_stack.push("(" + (op2 + *it) += op1 + ")");
        }
        ++it;
    }
    return expression_stack.top();
}
