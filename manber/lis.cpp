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

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

size_t longest_increasing_subsequence(const std::vector<int>& arr) {
    if (arr.empty()) {
        return 0;
    }

    // dp[i] = length of LIS ending at position i
    std::vector<size_t> dp(arr.size(), 1);

    // For each position i, find the LIS ending at i
    for (size_t i = 1; i < arr.size(); ++i) {
        // Variable max_prev has the max LIS for all j < i
        size_t prev_max = 0;
        for (size_t j = 0; j < i; ++j) {
            if (arr[j] < arr[i]) {
                // Extend the LIS ending at position j by including arr[i]
                prev_max = std::max(prev_max, dp[j] + 1);
            }
        }
        dp[i] = std::max(dp[i], prev_max);
    }

    return *std::max_element(dp.begin(), dp.end());
}

// TODO: Implement a O(n log n) version of the solution (Manber page 167-169).

// Tests

void test_empty_array() {
    std::vector<int> arr = {};
    size_t expected = 0;
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_empty_array passed\n";
}

void test_single_element() {
    std::vector<int> arr = {5};
    size_t expected = 1;
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_single_element passed\n";
}

void test_all_increasing() {
    std::vector<int> arr = {1, 2, 3, 4, 5};
    size_t expected = 5;
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_all_increasing passed\n";
}

void test_all_decreasing() {
    std::vector<int> arr = {5, 4, 3, 2, 1};
    size_t expected = 1;
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_all_decreasing passed\n";
}

void test_all_equal() {
    std::vector<int> arr = {3, 3, 3, 3};
    size_t expected = 1;
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_all_equal passed\n";
}

void test_mixed_sequence() {
    std::vector<int> arr = {10, 9, 2, 5, 3, 7, 101, 18};
    size_t expected = 4; // [2, 3, 7, 18] or [2, 5, 7, 18] or [2, 3, 7, 101]
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_mixed_sequence passed\n";
}

void test_duplicates_with_increasing() {
    std::vector<int> arr = {1, 3, 6, 7, 9, 4, 10, 5, 6};
    size_t expected = 6; // [1, 3, 4, 5, 6, 10] or [1, 3, 6, 7, 9, 10]
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_duplicates_with_increasing passed\n";
}

void test_two_element_increasing() {
    std::vector<int> arr = {1, 2};
    size_t expected = 2;
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_two_element_increasing passed\n";
}

void test_two_element_decreasing() {
    std::vector<int> arr = {2, 1};
    size_t expected = 1;
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_two_element_decreasing passed\n";
}

void test_alternating_pattern() {
    std::vector<int> arr = {1, 4, 2, 3, 5, 1, 6};
    size_t expected = 5; // [1, 2, 3, 5, 6]
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_alternating_pattern passed\n";
}

void test_negative_numbers() {
    std::vector<int> arr = {-10, -3, 0, 5, -1, 2, 8};
    size_t expected = 5; // [-10, -3, 0, 2, 8]
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_negative_numbers passed\n";
}

void test_classic_example() {
    std::vector<int> arr = {0, 1, 0, 3, 2, 3};
    size_t expected = 4; // [0, 1, 2, 3]
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_classic_example passed\n";
}

void test_large_numbers() {
    std::vector<int> arr = {10, 22, 9, 33, 21, 50, 41, 60};
    size_t expected = 5; // [10, 22, 33, 50, 60]
    assert(longest_increasing_subsequence(arr) == expected);
    std::cout << "test_large_numbers passed\n";
}

int main() {
    test_empty_array();
    test_single_element();
    test_all_increasing();
    test_all_decreasing();
    test_all_equal();
    test_mixed_sequence();
    test_duplicates_with_increasing();
    test_two_element_increasing();
    test_two_element_decreasing();
    test_alternating_pattern();
    test_negative_numbers();
    test_classic_example();
    test_large_numbers();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
