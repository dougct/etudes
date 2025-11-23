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

#[derive(Debug, Clone)]
pub struct TreeNode {
    #[allow(dead_code)]
    pub val: i32,
    pub left: Option<Box<TreeNode>>,
    pub right: Option<Box<TreeNode>>,
}

impl TreeNode {
    pub fn new(val: i32) -> Self {
        TreeNode {
            val,
            left: None,
            right: None,
        }
    }

    pub fn with_children(
        val: i32,
        left: Option<Box<TreeNode>>,
        right: Option<Box<TreeNode>>,
    ) -> Self {
        TreeNode { val, left, right }
    }
}

fn compute_heights_and_balance_factors(node: &Option<Box<TreeNode>>, result: &mut Vec<i32>) -> i32 {
    match node {
        None => 0,
        Some(n) => {
            let left_height = compute_heights_and_balance_factors(&n.left, result);
            let right_height = compute_heights_and_balance_factors(&n.right, result);

            let balance_factor = left_height - right_height;
            result.push(balance_factor);

            return 1 + left_height.max(right_height)
        }
    }
}

pub fn compute_balance_factors(root: &Option<Box<TreeNode>>) -> Vec<i32> {
    let mut result = Vec::new();
    compute_heights_and_balance_factors(root, &mut result);
    return result;
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_empty_tree() {
        let root = None;
        let balance_factors = compute_balance_factors(&root);
        assert_eq!(balance_factors, vec![]);
    }

    #[test]
    fn test_single_node() {
        let root = Some(Box::new(TreeNode::new(1)));
        let balance_factors = compute_balance_factors(&root);
        assert_eq!(balance_factors, vec![0]);
    }

    #[test]
    fn test_left_heavy_tree() {
        // Tree:
        //     2
        //    /
        //   1
        let mut root = TreeNode::new(2);
        root.left = Some(Box::new(TreeNode::new(1)));
        let balance_factors = compute_balance_factors(&Some(Box::new(root)));
        // Post-order traversal: left child (1) first, then root (2)
        // Balance factors: [0, 1] (leaf=0, root=1)
        assert_eq!(balance_factors, vec![0, 1]);
    }

    #[test]
    fn test_right_heavy_tree() {
        // Tree:
        //   1
        //    \
        //     2
        let mut root = TreeNode::new(1);
        root.right = Some(Box::new(TreeNode::new(2)));
        let balance_factors = compute_balance_factors(&Some(Box::new(root)));
        // Post-order traversal: right child (2) first, then root (1)
        // Balance factors: [0, -1] (leaf=0, root=-1)
        assert_eq!(balance_factors, vec![0, -1]);
    }

    #[test]
    fn test_balanced_tree() {
        // Tree:
        //     2
        //    / \
        //   1   3
        let root = TreeNode::with_children(
            2,
            Some(Box::new(TreeNode::new(1))),
            Some(Box::new(TreeNode::new(3))),
        );
        let balance_factors = compute_balance_factors(&Some(Box::new(root)));
        // Balance factor for node 2: height(left) - height(right) = 1 - 1 = 0
        // Balance factors for nodes 1 and 3: 0 - 0 = 0
        assert_eq!(balance_factors, vec![0, 0, 0]);
    }

    #[test]
    fn test_complex_tree() {
        // Tree:
        //       4
        //      / \
        //     2   6
        //    / \   \
        //   1   3   7
        let root = TreeNode::with_children(
            4,
            Some(Box::new(TreeNode::with_children(
                2,
                Some(Box::new(TreeNode::new(1))),
                Some(Box::new(TreeNode::new(3))),
            ))),
            Some(Box::new(TreeNode::with_children(
                6,
                None,
                Some(Box::new(TreeNode::new(7))),
            ))),
        );
        let balance_factors = compute_balance_factors(&Some(Box::new(root)));
        // Balance factors in pre-order traversal:
        // Node 4: left_height=2, right_height=2, balance=0
        // Node 2: left_height=1, right_height=1, balance=0
        // Node 1: left_height=0, right_height=0, balance=0
        // Node 3: left_height=0, right_height=0, balance=0
        // Node 6: left_height=0, right_height=1, balance=-1
        // Node 7: left_height=0, right_height=0, balance=0
        assert_eq!(balance_factors, vec![0, 0, 0, 0, -1, 0]);
    }

    #[test]
    fn test_deeply_left_unbalanced() {
        // Tree:
        //     4
        //    /
        //   3
        //  /
        // 2
        //  /
        // 1
        let root = Some(Box::new(TreeNode::with_children(
            4,
            Some(Box::new(TreeNode::with_children(
                3,
                Some(Box::new(TreeNode::with_children(
                    2,
                    Some(Box::new(TreeNode::new(1))),
                    None,
                ))),
                None,
            ))),
            None,
        )));
        let balance_factors = compute_balance_factors(&root);
        // Post-order traversal: 1, 2, 3, 4
        // Balance factors: [0, 1, 2, 3] (leaf to root)
        assert_eq!(balance_factors, vec![0, 1, 2, 3]);
    }

    #[test]
    fn test_deeply_right_unbalanced() {
        // Tree:
        // 1
        //  \
        //   2
        //    \
        //     3
        //      \
        //       4
        let root = Some(Box::new(TreeNode::with_children(
            1,
            None,
            Some(Box::new(TreeNode::with_children(
                2,
                None,
                Some(Box::new(TreeNode::with_children(
                    3,
                    None,
                    Some(Box::new(TreeNode::new(4))),
                ))),
            ))),
        )));
        let balance_factors = compute_balance_factors(&root);
        // Post-order traversal: 4, 3, 2, 1
        // Balance factors: [0, -1, -2, -3] (leaf to root)
        assert_eq!(balance_factors, vec![0, -1, -2, -3]);
    }

    #[test]
    fn test_zigzag_tree() {
        // Tree:
        //     1
        //      \
        //       3
        //      /
        //     2
        //      \
        //       4
        let root = Some(Box::new(TreeNode::with_children(
            1,
            None,
            Some(Box::new(TreeNode::with_children(
                3,
                Some(Box::new(TreeNode::with_children(
                    2,
                    None,
                    Some(Box::new(TreeNode::new(4))),
                ))),
                None,
            ))),
        )));
        let balance_factors = compute_balance_factors(&root);
        // Post-order traversal: 4, 2, 3, 1
        // Balance factors: [0, -1, 2, -3] (4=0, 2=-1, 3=2, 1=-3)
        assert_eq!(balance_factors, vec![0, -1, 2, -3]);
    }

    #[test]
    fn test_extreme_balance_factors() {
        // Create a tree with balance factor of 5
        let mut node = TreeNode::new(6);
        for i in (1..=5).rev() {
            node = TreeNode::with_children(i, Some(Box::new(node)), None);
        }
        let root = Some(Box::new(node));
        let balance_factors = compute_balance_factors(&root);

        // Post-order traversal: 6, 5, 4, 3, 2, 1
        // Balance factors: [0, 1, 2, 3, 4, 5] (leaf to root)
        assert_eq!(balance_factors, vec![0, 1, 2, 3, 4, 5]);
    }

    #[test]
    fn test_perfect_binary_tree() {
        // Tree:
        //       4
        //     /   \
        //    2     6
        //   / \   / \
        //  1   3 5   7
        let root = TreeNode::with_children(
            4,
            Some(Box::new(TreeNode::with_children(
                2,
                Some(Box::new(TreeNode::new(1))),
                Some(Box::new(TreeNode::new(3))),
            ))),
            Some(Box::new(TreeNode::with_children(
                6,
                Some(Box::new(TreeNode::new(5))),
                Some(Box::new(TreeNode::new(7))),
            ))),
        );
        let balance_factors = compute_balance_factors(&Some(Box::new(root)));
        // All nodes in a perfect binary tree should have balance factor 0
        assert_eq!(balance_factors, vec![0, 0, 0, 0, 0, 0, 0]);
    }

    #[test]
    fn test_single_child_nodes() {
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
        let root = Some(Box::new(TreeNode::with_children(
            5,
            Some(Box::new(TreeNode::with_children(
                3,
                None,
                Some(Box::new(TreeNode::with_children(
                    4,
                    Some(Box::new(TreeNode::with_children(
                        2,
                        None,
                        Some(Box::new(TreeNode::new(1))),
                    ))),
                    None,
                ))),
            ))),
            None,
        )));
        let balance_factors = compute_balance_factors(&root);
        // Post-order traversal: 1, 2, 4, 3, 5
        // Balance factors: [0, -1, 2, -3, 4] (1=0, 2=-1, 4=2, 3=-3, 5=4)
        assert_eq!(balance_factors, vec![0, -1, 2, -3, 4]);
    }
}
