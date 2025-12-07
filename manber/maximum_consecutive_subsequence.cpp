/*
Problem:
    Given an array of integers (possibly containing negative numbers), find a
    subsequence (from index i to index j) of consecutive elements such that the
    sum of the numbers in it is maximum over all subsequences of consecutive
    elements.

    For example, in the array [-2, 1, -3, 4, -1, 2, 1, -5, 4], the maximum
    consecutive subsequence is [4, -1, 2, 1] with sum 6.

Solution:
    Base case:
        The sum of the maximum consecutive subsequence of an empty sequence is
        zero by convention.

    Induction hypothesis (first attempt):
        We know how to find the maximum subsequence in sequences of size less
        than n.

        Our goal is to find S_n, the sum of the maximum subsequence in sequences
        of size n. Let S_{n - 1} be the maximum consecutive subsequence found
        using the induction hypothesis above, and assume the maximum subsequence
        starts at index i and ends at index j, with i <= j <= n - 1.
        If j = n - 1 (namely, the maximum subsequence is a suffix), then it is
        easy to extent the solution.
        If x_n is positive, then it extends S_{n - 1}, otherwise S_{n - 1}
        remains maximum.

        However, if j < n - 1, then there are two possibilities:
          a) S_{n - 1} remains maximum, or
          b) there is another subsequence, which is not maximum in S_{n - 1},
          but is maximum in S_n when x_n is added to it.
          The key idea here is to strengthen the induction hypothesis.

    Stronger induction hypothesis:
        We know how to find, in sequences of size less than n, a maximum
        subsequence overall, and the maximum subsequence that is a suffix.

    Induction step:
        If we know how to find both subsequences, the algorithm becomes clear.
        We add x_n to the maximum suffix. If the sum is more than the global
        maximum subsequence, then we have a new maximum subsequence (as well
        as a new suffix). Otherwise, we retain the previous maximum subsequence.

        We are not done yet.

        We also need to find the new maximum suffix. It is not true that we
        always simply add x_n to the previous maximum suffix. It could be that
        the maximum suffix ending at x_n is negative. In that case, it is better
        to take the empty sequence as the maximum suffix (such that later
        x_{n + 1} will be considered by itself).
*/

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

int max_consecutive_subsequence_naive(const std::vector<int>& nums) {
  int global_max = 0;
  for (size_t i = 0; i < nums.size(); ++i) {
    int partial_sum = 0;
    for (size_t j = i; j < nums.size(); ++j) {
      partial_sum += nums[j];
      global_max = std::max(partial_sum, global_max);
    }
  }
  return global_max;
}

int max_consecutive_subsequence(const std::vector<int>& nums) {
  int global_max = 0;
  int suffix_max = 0;
  for (size_t i = 0; i < nums.size(); ++i) {
    int x = nums[i];
    if (suffix_max + x > global_max) {
      global_max = suffix_max + x;
      suffix_max += x;
    } else if (suffix_max + x > 0) {
      suffix_max += x;
    } else {
      // Start a new suffix
      suffix_max = 0;
    }
  }

  return global_max;
}

// Tests

void test_empty_array() {
  std::vector<int> nums = {};
  int expected = 0;
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_empty_array passed\n";
}

void test_single_positive_element() {
  std::vector<int> nums = {5};
  int expected = 5;
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_single_positive_element passed\n";
}

void test_single_negative_element() {
  std::vector<int> nums = {-3};
  int expected = 0;  // empty subsequence
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_single_negative_element passed\n";
}

void test_all_positive_elements() {
  std::vector<int> nums = {1, 2, 3, 4, 5};
  int expected = 15;
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_all_positive_elements passed\n";
}

void test_all_negative_elements() {
  std::vector<int> nums = {-5, -2, -8, -1};
  int expected = 0;  // empty subsequence
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_all_negative_elements passed\n";
}

void test_mixed_positive_negative() {
  std::vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
  int expected = 6;  // [4, -1, 2, 1]
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_mixed_positive_negative passed\n";
}

void test_negative_followed_by_positive() {
  std::vector<int> nums = {-1, -2, 3, 4};
  int expected = 7;  // [3, 4]
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_negative_followed_by_positive passed\n";
}

void test_positive_followed_by_negative() {
  std::vector<int> nums = {3, 4, -1, -2};
  int expected = 7;  // [3, 4]
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_positive_followed_by_negative passed\n";
}

void test_alternating_pattern() {
  std::vector<int> nums = {1, -3, 2, 1, -1};
  int expected = 3;  // [2, 1]
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_alternating_pattern passed\n";
}

void test_zero_included() {
  std::vector<int> nums = {-1, 0, 2, -1, 3};
  int expected = 4;  // [0, 2, -1, 3]
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_zero_included passed\n";
}

void test_large_negative_gap() {
  std::vector<int> nums = {5, -10, 3, 4};
  int expected = 7;  // [3, 4]
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_large_negative_gap passed\n";
}

void test_kadane_classic_example() {
  std::vector<int> nums = {-2, -3, 4, -1, -2, 1, 5, -3};
  int expected = 7;  // [4, -1, -2, 1, 5]
  assert(max_consecutive_subsequence(nums) == expected);
  assert(max_consecutive_subsequence_naive(nums) == expected);
  std::cout << "test_kadane_classic_example passed\n";
}

int main() {
  test_empty_array();
  test_single_positive_element();
  test_single_negative_element();
  test_all_positive_elements();
  test_all_negative_elements();
  test_mixed_positive_negative();
  test_negative_followed_by_positive();
  test_positive_followed_by_negative();
  test_alternating_pattern();
  test_zero_included();
  test_large_negative_gap();
  test_kadane_classic_example();

  std::cout << "\nAll tests passed!\n";
  return 0;
}
