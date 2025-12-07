/*
Problem:
    Compute the balance factors of a binary tree.
    The balance factor of a node is the height of its left subtree minus the height of its right subtree.

Solution:
    Base case (empty tree):
        An empty tree has no nodes, so return an empty list of balance factors.

    Induction hypothesis:
        We know how to compute the balance factors and heights for all nodes in binary trees that have less than n nodes.

    Induction step:
        If we consider the root, we can determine its balance factor easily by calculating the difference between the height of its children. Forthermore, we can also determine the height of the root (it's the maximal height of the two children plus one).
*/

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

struct TreeNode {
    int val;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;

    explicit TreeNode(int v) : val(v), left(nullptr), right(nullptr) {}

    TreeNode(int v, std::unique_ptr<TreeNode> l, std::unique_ptr<TreeNode> r)
        : val(v), left(std::move(l)), right(std::move(r)) {}
};

int compute_heights_and_balance_factors(const std::unique_ptr<TreeNode>& node, std::vector<int>& result) {
    if (!node) {
        return 0;
    }

    int left_height = compute_heights_and_balance_factors(node->left, result);
    int right_height = compute_heights_and_balance_factors(node->right, result);

    int balance_factor = left_height - right_height;
    result.push_back(balance_factor);

    return 1 + std::max(left_height, right_height);
}

std::vector<int> compute_balance_factors(const std::unique_ptr<TreeNode>& root) {
    std::vector<int> result;
    compute_heights_and_balance_factors(root, result);
    return result;
}

// Helper function to create a leaf node
std::unique_ptr<TreeNode> make_node(int val) {
    return std::make_unique<TreeNode>(val);
}

// Helper function to create a node with children
std::unique_ptr<TreeNode> make_node(int val, std::unique_ptr<TreeNode> left, std::unique_ptr<TreeNode> right) {
    return std::make_unique<TreeNode>(val, std::move(left), std::move(right));
}

// Tests

void test_empty_tree() {
    std::unique_ptr<TreeNode> root = nullptr;
    auto balance_factors = compute_balance_factors(root);
    assert(balance_factors.empty());
    std::cout << "test_empty_tree passed\n";
}

void test_single_node() {
    auto root = make_node(1);
    auto balance_factors = compute_balance_factors(root);
    assert((balance_factors == std::vector<int>{0}));
    std::cout << "test_single_node passed\n";
}

void test_left_heavy_tree() {
    // Tree:
    //     2
    //    /
    //   1
    auto root = make_node(2, make_node(1), nullptr);
    auto balance_factors = compute_balance_factors(root);
    // Post-order traversal: left child (1) first, then root (2)
    // Balance factors: [0, 1] (leaf=0, root=1)
    assert((balance_factors == std::vector<int>{0, 1}));
    std::cout << "test_left_heavy_tree passed\n";
}

void test_right_heavy_tree() {
    // Tree:
    //   1
    //    \
    //     2
    auto root = make_node(1, nullptr, make_node(2));
    auto balance_factors = compute_balance_factors(root);
    // Post-order traversal: right child (2) first, then root (1)
    // Balance factors: [0, -1] (leaf=0, root=-1)
    assert((balance_factors == std::vector<int>{0, -1}));
    std::cout << "test_right_heavy_tree passed\n";
}

void test_balanced_tree() {
    // Tree:
    //     2
    //    / \
    //   1   3
    auto root = make_node(2, make_node(1), make_node(3));
    auto balance_factors = compute_balance_factors(root);
    // Balance factor for node 2: height(left) - height(right) = 1 - 1 = 0
    // Balance factors for nodes 1 and 3: 0 - 0 = 0
    assert((balance_factors == std::vector<int>{0, 0, 0}));
    std::cout << "test_balanced_tree passed\n";
}

void test_complex_tree() {
    // Tree:
    //       4
    //      / \
    //     2   6
    //    / \   \
    //   1   3   7
    auto root = make_node(4,
        make_node(2,
            make_node(1),
            make_node(3)),
        make_node(6,
            nullptr,
            make_node(7)));
    auto balance_factors = compute_balance_factors(root);
    // Balance factors in pre-order traversal:
    // Node 4: left_height=2, right_height=2, balance=0
    // Node 2: left_height=1, right_height=1, balance=0
    // Node 1: left_height=0, right_height=0, balance=0
    // Node 3: left_height=0, right_height=0, balance=0
    // Node 6: left_height=0, right_height=1, balance=-1
    // Node 7: left_height=0, right_height=0, balance=0
    assert((balance_factors == std::vector<int>{0, 0, 0, 0, -1, 0}));
    std::cout << "test_complex_tree passed\n";
}

void test_deeply_left_unbalanced() {
    // Tree:
    //     4
    //    /
    //   3
    //  /
    // 2
    //  /
    // 1
    auto root = make_node(4,
        make_node(3,
            make_node(2,
                make_node(1),
                nullptr),
            nullptr),
        nullptr);
    auto balance_factors = compute_balance_factors(root);
    // Post-order traversal: 1, 2, 3, 4
    // Balance factors: [0, 1, 2, 3] (leaf to root)
    assert((balance_factors == std::vector<int>{0, 1, 2, 3}));
    std::cout << "test_deeply_left_unbalanced passed\n";
}

void test_deeply_right_unbalanced() {
    // Tree:
    // 1
    //  \
    //   2
    //    \
    //     3
    //      \
    //       4
    auto root = make_node(1,
        nullptr,
        make_node(2,
            nullptr,
            make_node(3,
                nullptr,
                make_node(4))));
    auto balance_factors = compute_balance_factors(root);
    // Post-order traversal: 4, 3, 2, 1
    // Balance factors: [0, -1, -2, -3] (leaf to root)
    assert((balance_factors == std::vector<int>{0, -1, -2, -3}));
    std::cout << "test_deeply_right_unbalanced passed\n";
}

void test_zigzag_tree() {
    // Tree:
    //     1
    //      \
    //       3
    //      /
    //     2
    //      \
    //       4
    auto root = make_node(1,
        nullptr,
        make_node(3,
            make_node(2,
                nullptr,
                make_node(4)),
            nullptr));
    auto balance_factors = compute_balance_factors(root);
    // Post-order traversal: 4, 2, 3, 1
    // Balance factors: [0, -1, 2, -3] (4=0, 2=-1, 3=2, 1=-3)
    assert((balance_factors == std::vector<int>{0, -1, 2, -3}));
    std::cout << "test_zigzag_tree passed\n";
}

void test_extreme_balance_factors() {
    // Create a tree with balance factor of 5
    auto node = make_node(6);
    for (int i = 5; i >= 1; --i) {
        node = make_node(i, std::move(node), nullptr);
    }
    auto balance_factors = compute_balance_factors(node);

    // Post-order traversal: 6, 5, 4, 3, 2, 1
    // Balance factors: [0, 1, 2, 3, 4, 5] (leaf to root)
    assert((balance_factors == std::vector<int>{0, 1, 2, 3, 4, 5}));
    std::cout << "test_extreme_balance_factors passed\n";
}

void test_perfect_binary_tree() {
    // Tree:
    //       4
    //     /   \
    //    2     6
    //   / \   / \
    //  1   3 5   7
    auto root = make_node(4,
        make_node(2,
            make_node(1),
            make_node(3)),
        make_node(6,
            make_node(5),
            make_node(7)));
    auto balance_factors = compute_balance_factors(root);
    // All nodes in a perfect binary tree should have balance factor 0
    assert((balance_factors == std::vector<int>{0, 0, 0, 0, 0, 0, 0}));
    std::cout << "test_perfect_binary_tree passed\n";
}

void test_single_child_nodes() {
    // Tree with mixed single-child scenarios:
    //     5
    //    /
    //   3
    //    \
    //     4
    //    /
    //   2
    //    \
    //     1
    auto root = make_node(5,
        make_node(3,
            nullptr,
            make_node(4,
                make_node(2,
                    nullptr,
                    make_node(1)),
                nullptr)),
        nullptr);
    auto balance_factors = compute_balance_factors(root);
    // Post-order traversal: 1, 2, 4, 3, 5
    // Balance factors: [0, -1, 2, -3, 4] (1=0, 2=-1, 4=2, 3=-3, 5=4)
    assert((balance_factors == std::vector<int>{0, -1, 2, -3, 4}));
    std::cout << "test_single_child_nodes passed\n";
}

int main() {
    test_empty_tree();
    test_single_node();
    test_left_heavy_tree();
    test_right_heavy_tree();
    test_balanced_tree();
    test_complex_tree();
    test_deeply_left_unbalanced();
    test_deeply_right_unbalanced();
    test_zigzag_tree();
    test_extreme_balance_factors();
    test_perfect_binary_tree();
    test_single_child_nodes();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
