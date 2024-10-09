#include "simple_tree.h"
#include "tokenizer.h"

#include <iostream>

using namespace std;

using AST = SimpleTree<ANode>;

int main()
{
    auto ast = AST{};
    ast.add_child(AST::make_root({}), {});

    auto root = ast.get_root();

    // Define visitors
    auto l_paren_visitor = [&](LeftParentheses)
    {
        const auto new_root = AST::make_root(ANode{});
        ast.add_child(new_root, root);
        root = new_root;
    };

    auto r_paren_visitor = [&](RightParentheses) { root = root->parent; };

    auto number_visitor = [&](Number<> number)
    {
        root->data = ANode{TokenType::Number, number};
        root = root->parent;
    };

    auto ops_visitor = [&](auto op)
    {
        root->data = ANode{TokenType::Operator, op};
        const auto new_root = AST::make_root(ANode{});
        ast.add_child(new_root, root);
        root = new_root;
    };

    auto visitors = make_visitors(l_paren_visitor, r_paren_visitor, number_visitor, ops_visitor);

    // Create AST
    const auto expression = "((7+3)*(5-2))";
    for (const auto tokens = tokenize(expression); const auto& token : tokens)
    {
        visit(visitors, token.token_value);
    }

    return 0;
}