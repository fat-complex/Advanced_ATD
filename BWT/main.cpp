#include <bits/fs_fwd.h>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

template <typename T>
using Sptr = shared_ptr<T>;

struct Node
{
    using Link = Sptr<Node>;

    int id;
    Link left;
    Link right;
    Link parent;

    explicit Node(const int id, Link left = nullptr, Link right = nullptr, Link parent = nullptr)
        : id(id)
        , left(move(left))
        , right(move(right))
        , parent(std::move(parent))
    {
    }
};

template <typename Property>
class Edge
{
public:
    Node::Link u;
    Node::Link v;
    Property property;

    Edge(Node::Link u, Node::Link v, Property property)
        : u{std::move(u)}
        , v{std::move(v)}
        , property{property}
    {
    }

    bool operator==(const Edge& rhs) const
    {
        return u == rhs.u && v == rhs.v or u == rhs.v and v == rhs.u;
    }
};

enum Color
{
    Noop,
    Red,
    Green,
    Blue,
    Yellow,
};

// default mode
unordered_set colors = {Red, Green, Blue, Yellow};

random_device rnd_device;
mt19937_64 rng{rnd_device()};

auto get_available_color(const unordered_set<Color>& busy_colors) -> Color
{
    vector<Color> available_colors;
    for (const auto& color : colors)
    {
        if (!busy_colors.contains(color))
        {
            available_colors.push_back(color);
        }
    }
    uniform_int_distribution<mt19937_64::result_type> distribution(0, available_colors.size() - 1);
    return available_colors[distribution(rng)];
}

class BTree
{
public:
    using Link = Node::Link;
    using ColorEdge = Edge<Color>;

    BTree() = default;

    auto insert(const int id) -> void
    {
        insert(id, root_, nullptr);
    }

    auto insert(vector<int> ids) -> void
    {
        if (!is_sorted(ids.begin(), ids.end()))
        {
            sort(ids.begin(), ids.end());
        }
        insert(ids, 0, ids.size() - 1);
    }

    auto find_node(int id) -> Link
    {
        return find_node(root_, id);
    }

    [[nodiscard]] auto get_root() const -> Link
    {
        return root_;
    }

    auto print(ostream& out = cout) const -> void
    {
        if (root_ == nullptr)
        {
            cout << "{}" << endl;
            return;
        }
        out << as_string(root_) << endl;
    }

    [[nodiscard]] auto get_edges() const -> vector<ColorEdge>
    {
        return edges_;
    }

    [[nodiscard]] auto get_leafs() const -> vector<Link>
    {
        auto get_left_and_right = [&](const Link& root) -> pair<Link, Link>
        { return make_pair(root->left, root->right); };
        vector<Link> leaf_edges;
        for (const auto& edge : edges_)
        {
            auto [u_left, u_right] = get_left_and_right(edge.u);
            auto [v_left, v_right] = get_left_and_right(edge.v);
            if (u_left == nullptr and u_right == nullptr)
            {
                leaf_edges.push_back(edge.u);
            }
            if (v_left == nullptr and v_right == nullptr)
            {
                leaf_edges.push_back(edge.v);
            }
        }
        return leaf_edges;
    }

    auto insert(const int data, Link& root, Link parent) -> void
    {
        if (root == nullptr)
        {
            root = make_shared<Node>(data);
            root->parent = parent;
            color_edge_parent_to_child(root, parent);
            return;
        }
        if (data < root->id)
        {
            insert(data, root->left, root);
            return;
        }
        if (data > root->id)
        {
            insert(data, root->right, root);
        }
    }

    auto insert(const vector<int>& ids, int start, int end) -> void
    {
        if (start > end)
        {
            return;
        }
        auto mid = (start + end) / 2;
        insert(ids[mid]);
        insert(ids, start, mid - 1);
        insert(ids, mid + 1, end);
    }

    [[nodiscard]] string as_string(const Link& root) const
    {
        const std::string left_str = (root->left == nullptr) ? "{}" : as_string(root->left);
        const std::string right_str = (root->right == nullptr) ? "{}" : as_string(root->right);
        std::string result =
            "{" + std::to_string(root->id) + ", " + left_str + ", " + right_str + "}";
        return result;
    }

    auto find_node(Link& root, int id) -> Link
    {
        if (root == nullptr or root->id == id)
        {
            return root;
        }
        if (id < root->id)
        {
            return find_node(root->left, id);
        }
        return find_node(root->right, id);
    }

    auto color_edge_parent_to_child(Link leaf, const Link parent, Color excluded_color = Noop)
        -> ColorEdge
    {
        if (parent == nullptr)
        {
            return ColorEdge{nullptr, nullptr, Noop};
        }
        auto color_result = Color::Noop;
        auto parent_color = get_color_edge(parent, parent->parent);
        if (is_left_child(leaf, parent) and has_right_bro(parent))
        {
            auto right_bro_color = get_color_edge(parent, parent->right);
            color_result = get_available_color({parent_color, right_bro_color});
        }
        if (is_left_child(leaf, parent) and !has_right_bro(parent))
        {
            color_result = get_available_color({parent_color});
        }
        if (is_right_child(leaf, parent) and has_left_bro(parent))
        {
            auto left_bro_color = get_color_edge(parent, parent->left);
            color_result = get_available_color({parent_color, left_bro_color});
        }
        if (is_right_child(leaf, parent) and !has_left_bro(parent))
        {
            color_result = get_available_color({parent_color});
        }
        if (color_result == Noop)
        {
            color_result = get_available_color({parent_color});
        }
        auto color_edge = ColorEdge(leaf, parent, color_result);
        edges_.push_back(color_edge);

        return color_edge;
    }

    auto get_color_edge(Link u, Link v) -> Color
    {
        const auto foundIt = find(edges_.begin(), edges_.end(), Edge{u, v, Color::Noop});
        return foundIt != edges_.end() ? foundIt->property : Color::Noop;
    }

    static auto is_left_child(const Link& root, const Link& parent) -> bool
    {
        return parent != nullptr && parent->left == root;
    }

    static auto is_right_child(const Link& root, const Link& parent) -> bool
    {
        return parent != nullptr && parent->right == root;
    }

    static auto has_left_bro(const Link& root) -> bool
    {
        return root->left != nullptr;
    }

    static auto has_right_bro(const Link& root) -> bool
    {
        return root->right != nullptr;
    }

    Link root_{};
    vector<ColorEdge> edges_{};
};

class BWT
{
public:
    explicit BWT(const uint8_t depth, int start_id, const uint8_t number_of_colors = 4)
        : depth_{depth}
        , number_of_colors_{number_of_colors}
    {
        feel_tree(get_generated_ids(start_id), first_tree_);
        leafs_ = first_tree_.get_leafs();
        feel_tree(get_generated_ids(first_tree_.get_leafs().back()->id + 2), second_tree_);
        connect();
    }

    auto connect() -> void
    {
        auto first_leafs = first_tree_.get_leafs();
        auto second_leafs = second_tree_.get_leafs();

        uniform_int_distribution<mt19937_64::result_type> distribution(0, second_leafs.size() - 1);
        for (auto& leaf : first_leafs)
        {
            const auto& bindable_leaf1 = second_leafs[distribution(rng)];
            const auto& bindable_leaf2 = second_leafs[distribution(rng)];
            leaf->left = bindable_leaf1;
            leaf->right = bindable_leaf2;
            const auto edge1 = first_tree_.color_edge_parent_to_child(bindable_leaf1, leaf);
            auto edge2 =
                first_tree_.color_edge_parent_to_child(bindable_leaf2, leaf, edge1.property);
        }
    }

    auto random_walk(int id) -> Node::Link
    {
        auto first = first_tree_.find_node(id);
        if (first != nullptr)
        {
            return first;
        }
        uniform_int_distribution<mt19937_64::result_type> distribution1(0, leafs_.size() - 1);
        auto some_leafs = leafs_[distribution1(rng)];

        uniform_int_distribution<mt19937_64::result_type> distribution2(0, 1);
        auto second_tree_branch = distribution2(rng) == 0 ? some_leafs->left : some_leafs->right;

        while (second_tree_branch and second_tree_branch->id != id)
        {
            second_tree_branch = second_tree_branch->parent;
        }
        return second_tree_branch;
    }

    auto find_by_id(int id)
    {
        auto first = first_tree_.find_node(id);
        auto second = second_tree_.find_node(id);
        return first != nullptr ? first : (second != nullptr ? second : nullptr);
    }

private:
    static auto feel_tree(const vector<int>& ids, BTree& tree) -> void
    {
        tree.insert(ids);
    }

    [[nodiscard]] auto get_generated_ids(int step) const -> vector<int>
    {
        const auto count_elems_in_tree = static_cast<int>(pow(2, depth_ + 1) - 1);
        vector<int> ids(count_elems_in_tree);
        iota(ids.begin(), ids.end(), step);
        return ids;
    }
    int depth_;
    int number_of_colors_;
    BTree first_tree_;
    BTree second_tree_;
    vector<Node::Link> leafs_;
};

int main()
{
    BWT bwt{/*depth=*/2, /*start id=*/1};
    auto res = bwt.random_walk(12);

    cout << res->id << endl;

    return 0;
}