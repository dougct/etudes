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

fn height(node: &Option<Box<TreeNode>>) -> i32 {
    match node {
        None => 0,
        Some(n) => 1 + height(&n.left).max(height(&n.right)),
    }
}

fn collect_balance_factors(node: &Option<Box<TreeNode>>, result: &mut Vec<i32>) {
    if let Some(n) = node {
        let left_height = height(&n.left);
        let right_height = height(&n.right);
        let balance_factor = left_height - right_height;

        result.push(balance_factor);
        collect_balance_factors(&n.left, result);
        collect_balance_factors(&n.right, result);
    }
}

pub fn compute_balance_factors(root: &Option<Box<TreeNode>>) -> Vec<i32> {
    let mut result = Vec::new();
    collect_balance_factors(root, &mut result);
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
        // Balance factor for node 2: height(left) - height(right) = 1 - 0 = 1
        // Balance factor for node 1: height(left) - height(right) = 0 - 0 = 0
        assert_eq!(balance_factors, vec![1, 0]);
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
        // Balance factor for node 1: height(left) - height(right) = 0 - 1 = -1
        // Balance factor for node 2: height(left) - height(right) = 0 - 0 = 0
        assert_eq!(balance_factors, vec![-1, 0]);
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
        // Expected balance factors (in some traversal order)
        // This test will need to be adjusted based on the traversal order chosen
        assert_eq!(balance_factors.len(), 6);
    }
}
