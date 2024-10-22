#include <bits/fs_fwd.h>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

template <typename T>
using Uptr = unique_ptr<T>;

template <typename T>
using Sptr = shared_ptr<T>;

template <typename T, typename Property>
class Edge
{
public:
    T u;
    T v;
    Property property;

    Edge(T u, T v, Property property)
        : u{u}
        , v{v}
        , property{property}
    {
    }
};

enum Colors
{
    Noop,
    Red,
    Green,
    Blue,
    Grey,
    Yellow,
    Brown,
    Violet
};

// default mode
array<Colors, 4> colors = {Red, Green, Blue, Grey};

template <typename T = int>
class BTree
{
public:
    struct Node;
    using Link = Sptr<Node>;
    using ColorEdge = Edge<const Link&, Colors>;

    struct Node
    {
        T data;
        Link left;
        Link right;
        Link parent;

        explicit Node(T data, Link left = nullptr, Link right = nullptr, Link parent = nullptr)
            : data(move(data))
            , left(move(left))
            , right(move(right))
        {
        }
    };
    BTree() = default;

    auto insert(const T& data) -> void
    {
        insert(data, root_, nullptr);
    }

    auto print(ostream& out = cout) -> void
    {
        if (root_ == nullptr)
        {
            cout << "{}" << endl;
            return;
        }
        out << as_string(root_) << endl;
    }

    auto get_leafs() const -> vector<ColorEdge>
    {
        vector<ColorEdge> leaf_edges;
        for (ColorEdge& edge : edges_)
        {
            if (edge.left == nullptr and edge.right == nullptr)
            {
                leaf_edges.push_back(edge);
            }
        }
        return leaf_edges;
    }

private:
    void insert(T data, Link& root, const Link& parent)
    {
        if (root == nullptr)
        {
            root = make_unique<Node>(data);
            root->parent = parent;
            color_edge_parent_to_child(root, parent);
            return;
        }
        if (data < root->data)
        {
            insert(data, root->left, root);
            return;
        }
        if (data > root->data)
        {
            insert(data, root->right, root);
        }
    }

    [[nodiscard]] string as_string(const Link& root) const
    {
        std::string left_str = (root->left == nullptr) ? "{}" : as_string(root->left);
        std::string right_str = (root->right == nullptr) ? "{}" : as_string(root->right);
        std::string result =
            "{" + std::to_string(root->data) + ", " + left_str + ", " + right_str + "}";
        return result;
    }

    auto color_edge_parent_to_child(const Link& leaf, const Link& parent) -> void
    {
        if (parent == nullptr)
        {
            return;
        }
        auto color_result = Colors::Noop;
        auto parent_color =  get_color_edge(parent, parent->parent);
        if (is_left_child(leaf, parent) and has_right_bro(parent))
        {
            auto right_bro_color = get_color_edge(parent, parent->right);
            color_result = get_avaliable_color({parent_color, right_bro_color});
        }
        if (is_left_child(leaf, parent) and !has_right_bro(parent))
        {
            color_result = get_avaliable_color({parent_color});
        }
        if (is_right_child(leaf, parent) and has_left_bro(parent))
        {
            auto left_bro_color = get_color_edge(parent, parent->left);
            color_result = get_avaliable_color({parent_color, left_bro_color});
        }
        if (is_right_child(leaf, parent) and !has_left_bro(parent))
        {
            color_result = get_avaliable_color({parent_color});
        }
        auto color_edge = ColorEdge(leaf, parent, color_result);
        edges_.push_back(move(color_edge));
    }

    auto get_color_edge(const Link& u, const Link& v) -> Colors
    {
        const auto foundIt = find_if(edges_.begin(), edges_.end(), [&](auto& edge)
        {
            return edge.u == u and edge.v == v or edge.v == u and edge.u == v;
        });
        return foundIt != edges_.end() ? foundIt->property : Colors::Noop;
    }

    auto is_left_child(const Link& root, const Link& parent) -> bool
    {
        return parent != nullptr && parent->left == root;
    }

    auto is_right_child(const Link& root, const Link& parent) -> bool
    {
        return parent != nullptr && parent->right == root;
    }

    auto has_left_bro(const Link& root) -> bool
    {
        return root->left != nullptr;
    }

    auto has_right_bro(const Link& root) -> bool
    {
        return root->right != nullptr;
    }

    auto get_avaliable_color(const vector<Colors>& busy_colors)
    {
        auto available_colors = colors;
        auto rest_colors = vector<Colors>{};
        for (const auto& color : busy_colors)
        {

            if (find(colors.begin(), colors.end(), color) == colors.end() and color != Noop)
            {
                rest_colors.push_back(color);
            }
        }
        mt19937 engine;
        engine.seed(time(nullptr));
        if (rest_colors.empty())
        {
            return available_colors[engine() % available_colors.size()];
        }
        auto idx = engine() % rest_colors.size();
        return rest_colors[idx];
    }

    Link root_;
    vector<ColorEdge> edges_;
};

class BWT
{
public:
    explicit BWT(const uint8_t depth = 4, const uint8_t number_of_colors = 4)
        : depth_{depth}
        , number_of_colors_{number_of_colors}
    {
    }

    auto feel_first(const vector<int>& nodes) -> void
    {
        for (const auto& node : nodes)
        {
            first_tree_.insert(node);
        }
    }

    auto feel_second(const vector<int>& nodes) -> void
    {
        for (const auto& node : nodes)
        {
            second_tree_.insert(node);
        }
    }

private:
    uint8_t depth_;
    uint8_t number_of_colors_;
    BTree<> first_tree_;
    BTree<> second_tree_;
};

int main()
{
    BWT bwt;
    bwt.feel_first({8, 3, 10, 1, 6, 9, 14});
    bwt.feel_second({8, 3, 10, 1, 6, 9, 14});

    return 0;
}