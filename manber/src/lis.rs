/*
Problem:
    Given an array of integers, find the length of the longest increasing subsequence (LIS).
    A subsequence is a sequence that can be derived from the array by deleting some or no elements without changing the order of the remaining elements. An increasing subsequence has each element strictly greater than the previous one.

    For example, in the array [10, 9, 2, 5, 3, 7, 101, 18], the longest increasing subsequence
    is [2, 3, 7, 18] with length 4.

Solution:
    Base case (empty array):
        An empty array has no elements, so the LIS length is 0.
        For a single element, the LIS length is 1.

    Induction hypothesis:
        We know how to compute the length of the longest increasing subsequence for all subarrays arr[0..i] where i < n-1.

    Induction step:
        To find the LIS length for arr[0..n-1], we consider the element arr[n-1]:

        1. We maintain dp[i] = length of LIS ending exactly at position i. Initialization phase: set every element of dp as 1 (length of single element LIS).

        2. For each position j from 0 to n-2, if arr[j] < arr[n-1], then we can extend the LIS ending at position j by including arr[n-1]. Let Prev_Max be the max LIS ending at position j by including arr[n - 1].

        3. The LIS length ending at position n-1 is max(1, Prev_Max).

*/

pub fn longest_increasing_subsequence(arr: &[i32]) -> usize {
    if arr.is_empty() {
        return 0;
    }

    // dp[i] = length of LIS ending at position i
    let mut dp = vec![1; arr.len()];

    // For each position i, find the LIS ending at i
    for i in 1..arr.len() {
        // Variable max_prev has the max LIS for all j < i
        let mut prev_max = 0;
        for j in 0..i {
            if arr[j] < arr[i] {
                // Extend the LIS ending at position j by including arr[i]
                prev_max = prev_max.max(dp[j] + 1);
            }
        }
        dp[i] = dp[i].max(prev_max);
    }

    return *dp.iter().max().unwrap();
}

// TODO: Implement a O(n log n) version of the solution (Manber page 167-169).

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_empty_array() {
        let arr = [];
        let expected = 0;
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_single_element() {
        let arr = [5];
        let expected = 1;
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_all_increasing() {
        let arr = [1, 2, 3, 4, 5];
        let expected = 5;
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_all_decreasing() {
        let arr = [5, 4, 3, 2, 1];
        let expected = 1;
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_all_equal() {
        let arr = [3, 3, 3, 3];
        let expected = 1;
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_mixed_sequence() {
        let arr = [10, 9, 2, 5, 3, 7, 101, 18];
        let expected = 4; // [2, 3, 7, 18] or [2, 5, 7, 18] or [2, 3, 7, 101]
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_duplicates_with_increasing() {
        let arr = [1, 3, 6, 7, 9, 4, 10, 5, 6];
        let expected = 6; // [1, 3, 4, 5, 6, 10] or [1, 3, 6, 7, 9, 10]
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_two_element_increasing() {
        let arr = [1, 2];
        let expected = 2;
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_two_element_decreasing() {
        let arr = [2, 1];
        let expected = 1;
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_alternating_pattern() {
        let arr = [1, 4, 2, 3, 5, 1, 6];
        let expected = 5; // [1, 2, 3, 5, 6]
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_negative_numbers() {
        let arr = [-10, -3, 0, 5, -1, 2, 8];
        let expected = 5; // [-10, -3, 0, 2, 8]
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_classic_example() {
        let arr = [0, 1, 0, 3, 2, 3];
        let expected = 4; // [0, 1, 2, 3]
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }

    #[test]
    fn test_large_numbers() {
        let arr = [10, 22, 9, 33, 21, 50, 41, 60];
        let expected = 5; // [10, 22, 33, 50, 60]
        assert_eq!(longest_increasing_subsequence(&arr), expected);
    }
}
