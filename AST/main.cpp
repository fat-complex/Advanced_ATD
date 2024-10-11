#include "make_ast.h"
#include "simple_tree.h"
#include "tokenizer.h"

using namespace std;

using TRoot = decltype(AST{}.get_root());
using RetVisitorType = function<Number<>(Number<>, Number<>)>;

auto plus_visitor = [](Plus) -> RetVisitorType
{ return [](Number<> l, Number<> r) { return Number<>{l.value + r.value}; }; };

auto minus_visitor = [](Minus) -> RetVisitorType
{ return [](Number<> l, Number<> r) { return Number<>{l.value - r.value}; }; };

auto mul_visitor = [](Mul) -> RetVisitorType
{ return [](Number<> l, Number<> r) { return Number<>{l.value * r.value}; }; };

auto div_visitor = [](Div) -> RetVisitorType
{ return [](Number<> l, Number<> r) { return Number<>{l.value / r.value}; }; };

auto no_ops = [](auto) -> RetVisitorType { return [](Number<>, Number<>) { return Number<>{0}; }; };

auto visitors = make_visitors(plus_visitor, minus_visitor, mul_visitor, div_visitor, no_ops);

auto extract_number(TokenValue token_value)
{
    return visit(make_visitors([](Number<> num) { return num; }, [](auto) { return Number<>{0}; }),
                 token_value);
}

auto eval(const AST& ast, const TRoot& root) -> void
{
    if (ast.get_root() == root and root->data.token_type != TokenType::Operator)
    {
        return;
    }
    if (root->data.token_type == TokenType::Number)
    {
        return;
    }
    if (root->data.token_type == TokenType::Operator)
    {
        auto left = root->children[0];
        auto right = root->children[1];
        eval(ast, left);
        eval(ast, right);

        const auto op = visit(visitors, root->data.token_value);
        auto res =
            op(extract_number(left->data.token_value), extract_number(right->data.token_value));
        root->data = ANode{TokenType::Number, res};
    }
}

int main()
{
    // Create AST
    const auto expression = "((7+3)*(5-2))";
    const auto tokens = tokenize(expression);

    auto ast = make_ast(tokens);

    eval(ast, ast.get_root());

    cout << endl << "RES: " << ast.get_root()->data.to_string() << endl;

    return 0;
}