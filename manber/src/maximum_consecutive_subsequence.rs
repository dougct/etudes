/*
Problem:
    Given an array of integers (possibly containing negative numbers), find a subsequence (from index i to index j) of consecutive elements such that the sum of the numbers in it is maximum over all subsequences of consecutive elements.

    For example, in the array [-2, 1, -3, 4, -1, 2, 1, -5, 4], the maximum consecutive subsequence is [4, -1, 2, 1] with sum 6.

Solution:
    Base case:
        The sum of the maximum consecutive subsequence of an empty sequence is zero by convention.

    Induction hypothesis (first attempt):
        We know how to find the maximum subsequence in sequences of size less than n.

        Our goal is to find S_n, the sum of the maximum subsequence in sequences of size n.
        Let S_{n - 1} be the maximum consecutive subsequence found using the induction hypothesis above, and assume the maximum subsequence starts at index i and ends at index j, with i <= j <= n - 1.
        If j = n - 1 (namely, the maximum subsequence is a suffix), then it is easy to extent the solution. If x_n is positive, then it extends S_{n - 1}, otherwise S_{n - 1} remains maximum. However, if j < n - 1, then there are two possibilities:
          a) S_{n - 1} remains maximum, or
          b) there is another subsequence, which is not maximum in S_{n - 1}, but is maximum in S_n when x_n is added to it.
        The key idea here is to strengthen the induction hypothesis.

    Stronger induction hypothesis:
        We know how to find, in sequences of size less than n, a maximum subsequence overall, and the maximum subsequence that is a suffix.

    Induction step:
        If we know how to find both subsequences, the algorithm becomes clear. We add x_n to the maximum suffix. If the sum is more than the global maximum subsequence, then we have a new maximum subsequence (as well as a new suffix). Otherwise, we retain the previous maximum subsequence.

        We are not done yet.

        We also need to find the new maximum suffix. It is not true that we always simply add x_n to the previous maximum suffix. It could be that the maximum suffix ending at x_n is negative. In that case, it is better to take the empty sequence as the maximum suffix (such that later x_{n + 1} will be considered by itself).
*/

pub fn max_consecutive_subsequence_naive(arr: &[i32]) -> i32 {
    let mut global_max = 0;
    for i in 0..arr.len() {
        let mut partial_sum = 0;
        for j in i..arr.len() {
            partial_sum += arr[j];
            global_max = partial_sum.max(global_max);
        }
    }
    return global_max;
}

pub fn max_consecutive_subsequence(arr: &[i32]) -> i32 {
    let mut global_max = 0;
    let mut suffix_max = 0;
    for i in 0..arr.len() {
        let x = arr[i];
        if suffix_max + x > global_max {
            global_max = suffix_max + x;
            suffix_max += x;
        } else if suffix_max + x > 0 {
            suffix_max += x;
        } else {
            // Start a new suffix
            suffix_max = 0;
        }
    }

    return global_max;
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_empty_array() {
        let arr = [];
        let expected = 0;
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }

    #[test]
    fn test_single_positive_element() {
        let arr = [5];
        let expected = 5;
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }

    #[test]
    fn test_single_negative_element() {
        let arr = [-3];
        let expected = 0; // empty subsequence
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }

    #[test]
    fn test_all_positive_elements() {
        let arr = [1, 2, 3, 4, 5];
        let expected = 15;
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }

    #[test]
    fn test_all_negative_elements() {
        let arr = [-5, -2, -8, -1];
        let expected = 0; // empty subsequence
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }

    #[test]
    fn test_mixed_positive_negative() {
        let arr = [-2, 1, -3, 4, -1, 2, 1, -5, 4];
        let expected = 6; // [4, -1, 2, 1]
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }

    #[test]
    fn test_negative_followed_by_positive() {
        let arr = [-1, -2, 3, 4];
        let expected = 7; // [3, 4]
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }

    #[test]
    fn test_positive_followed_by_negative() {
        let arr = [3, 4, -1, -2];
        let expected = 7; // [3, 4]
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }

    #[test]
    fn test_alternating_pattern() {
        let arr = [1, -3, 2, 1, -1];
        let expected = 3; // [2, 1]
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }

    #[test]
    fn test_zero_included() {
        let arr = [-1, 0, 2, -1, 3];
        let expected = 4; // [0, 2, -1, 3]
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }

    #[test]
    fn test_large_negative_gap() {
        let arr = [5, -10, 3, 4];
        let expected = 7; // [3, 4]
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }

    #[test]
    fn test_kadane_classic_example() {
        let arr = [-2, -3, 4, -1, -2, 1, 5, -3];
        let expected = 7; // [4, -1, -2, 1, 5]
        assert_eq!(max_consecutive_subsequence(&arr), expected);
        assert_eq!(max_consecutive_subsequence_naive(&arr), expected);
    }
}
