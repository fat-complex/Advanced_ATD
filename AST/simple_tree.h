#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

template <typename T>
class SimpleTree
{
    struct Node final
    {
        T data;
        std::shared_ptr<Node> parent{};
        std::vector<std::shared_ptr<Node>> children{};
    };
public:
    using TRoot = std::shared_ptr<Node>;

    static auto make_root(T value) -> TRoot
    {
        return std::make_shared<Node>(value);
    }

    SimpleTree() = default;

    auto get_root() const -> TRoot
    {
        return root_;
    }

    void add_child(TRoot child, TRoot parent)
    {
        if (root_ == nullptr)
        {
            root_ = child;
            return;
        }
        parent->children.push_back(child);
        child->parent = parent;
    }

    void traverse(const std::function<void(const T& visited)>& callback)
    {
        traverse(root_, callback);
    }

private:
    TRoot root_{};
};