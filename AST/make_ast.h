#pragma once
#include "simple_tree.h"

class ANode;
using AST = SimpleTree<ANode>;

auto make_ast(const std::vector<ANode>& tokens) -> AST;