#!/usr/bin/env python3

"""
Problem:
    Generate all n-digit numbers without zeros.

Solution:
    Base case (n=1):
        The result is [1, 2, 3, 4, 5, 6, 7, 8, 9]

    Induction hypothesis:
        We know how to generate all (n-1)-digit numbers

    Induction step:
        Generate S, the list of all (n-1)-digit numbers, using the induction hypothesis.
        For each digit d in [1, 9] and each number in S, create a new number by
        placing d as the first digit followed by the existing number.

Extended problem:
    Use the previous solution to find all n-digit numbers whose digits sum to k.
    Zero doesn't contribute to the sum, so we don't need to worry about it.
    This fact simplifies the solution considerably.
"""


def generate_n_digit_numbers_no_zeros_recursive(n):
    if n == 0:
        return []

    if n == 1:
        return [1, 2, 3, 4, 5, 6, 7, 8, 9]

    prev_numbers = generate_n_digit_numbers_no_zeros_recursive(n - 1)

    result = []
    for digit in range(1, 10):
        for prev_num in prev_numbers:
            base_num = 10 ** (n - 1)
            curr_num = digit * base_num + prev_num
            result.append(curr_num)

    return result


def filter_numbers_by_digit_sum(numbers, k):
    """Filter numbers to keep only those whose digits sum to k"""
    result = []
    for num in numbers:
        digit_sum = 0
        n = num
        while n > 0:
            digit_sum += n % 10
            n //= 10
        if digit_sum == k:
            result.append(num)

    return result


def generate_n_digit_numbers_summing_to_k(n, k):
    numbers = generate_n_digit_numbers_no_zeros_recursive(n)
    return filter_numbers_by_digit_sum(numbers, k)


def test_generate_n_digit_numbers_recursive():
    # Test 0-digit numbers
    zero_digit_nums = generate_n_digit_numbers_no_zeros_recursive(0)
    assert zero_digit_nums == []

    # Test 1-digit numbers
    one_digit_nums = generate_n_digit_numbers_no_zeros_recursive(1)
    assert one_digit_nums == [1, 2, 3, 4, 5, 6, 7, 8, 9]
    assert len(one_digit_nums) == 9

    # Test 2-digit numbers
    two_digit_nums = generate_n_digit_numbers_no_zeros_recursive(2)
    # 9 choices for first digit * 9 choices for second digit
    assert len(two_digit_nums) == 81
    assert min(two_digit_nums) == 11
    assert max(two_digit_nums) == 99
    # Verify no zeros anywhere in the number
    for num in two_digit_nums:
        assert num >= 10, f"2-digit number {num} has leading zero"
        assert "0" not in str(num), f"2-digit number {num} contains zero"

    # Test 3-digit numbers
    three_digit_nums = generate_n_digit_numbers_no_zeros_recursive(3)
    assert len(three_digit_nums) == 729  # 9^3 = 729 (no zeros allowed)
    assert min(three_digit_nums) == 111
    assert max(three_digit_nums) == 999
    # Verify no zeros anywhere in the number
    for num in three_digit_nums:
        assert num >= 100, f"3-digit number {num} has leading zero"
        assert "0" not in str(num), f"3-digit number {num} contains zero"

    # Test that results are sorted
    for n in range(1, 5):
        numbers = generate_n_digit_numbers_no_zeros_recursive(n)
        assert numbers == sorted(numbers), f"Numbers not sorted for n={n}"

    # Test that all numbers contain no zeros
    for n in range(1, 4):
        numbers = generate_n_digit_numbers_no_zeros_recursive(n)
        for num in numbers:
            assert "0" not in str(num), f"Number {num} contains zero"

    print("All tests passed: generate_n_digit_numbers_recursive")


def test_generate_n_digit_numbers_summing_to_k():
    # Test 1-digit numbers (only digits 1-9, no zero)
    for k in range(1, 10):
        nums = generate_n_digit_numbers_summing_to_k(1, k)
        assert nums == [k]

    # Test impossible cases
    assert (
        generate_n_digit_numbers_summing_to_k(1, 0) == []
    )  # No 1-digit number sums to 0
    assert generate_n_digit_numbers_summing_to_k(1, 10) == []  # Max 1-digit sum is 9

    # Test edge cases - k=0 should always return empty (no zeros allowed)
    for n in range(1, 5):
        k = 0
        nums = generate_n_digit_numbers_summing_to_k(n, k)
        assert nums == []

    # Test specific cases: 2-digit numbers summing to 9
    result_two_digit = generate_n_digit_numbers_summing_to_k(2, 9)
    expected_two_digit = [18, 27, 36, 45, 54, 63, 72, 81]
    assert result_two_digit == expected_two_digit

    # Very large k (impossible sums)
    for n in range(1, 5):
        max_sum = 9 * n  # Maximum possible sum using only digits 1-9
        nums = generate_n_digit_numbers_summing_to_k(n, max_sum + 1)
        assert nums == []

    # Test that results are sorted and unique
    for n in [1, 2, 3]:
        for k in range(1, 15):
            result = generate_n_digit_numbers_summing_to_k(n, k)
            assert len(result) == len(set(result))
            assert result == sorted(result)

    print("All tests passed: generate_n_digit_numbers_summing_to_k")


# Simpler version of the solution, without recursion


def generate_n_digit_numbers_no_zeros(n):
    return list(range(10 ** (n - 1), 10**n))


def generate_n_digit_numbers_summing_to_k_simple(n, k):
    numbers = generate_n_digit_numbers_no_zeros(n)
    return filter_numbers_by_digit_sum(numbers, k)


def main():
    test_generate_n_digit_numbers_recursive()
    test_generate_n_digit_numbers_summing_to_k()


if __name__ == "__main__":
    main()
