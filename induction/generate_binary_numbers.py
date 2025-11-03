#!/usr/bin/env python3

"""
Problem:
    Generate a list of all binary number of n digits

Solution:
    Base case (n=0):
        Empty list

    Induction hypothesis:
        We know how to generate S, the list of all binary numbers of n-1 digits

    Induction step:
        Generate all binary numbers of n-1 digits (using induction hypothesis).
        Let S0 be the list with '0' appended to all numbers in S.
        Let S1 be the list with '1' appended to all numbers in S.
        Return the concatenation S0 and S1.
"""


def generate_binary_nums(n):
    if n == 0:
        return [""]

    binary_nums = generate_binary_nums(n - 1)

    return [num + suffix for num in binary_nums for suffix in ["0", "1"]]


def test_generate_binary_nums():
    # Basic cases
    assert generate_binary_nums(0) == [""]
    assert generate_binary_nums(1) == ["0", "1"]
    assert generate_binary_nums(2) == ["00", "01", "10", "11"]

    # Check count for larger cases
    assert len(generate_binary_nums(3)) == 8
    assert len(generate_binary_nums(4)) == 16
    assert len(generate_binary_nums(5)) == 32

    # Check specific values for n=3
    actual = generate_binary_nums(3)
    expected = ["000", "001", "010", "011", "100", "101", "110", "111"]
    assert actual == expected

    # Verify all strings have correct length
    for n in range(6):
        result = generate_binary_nums(n)
        for binary_str in result:
            assert len(binary_str) == n

    print("All tests passed: generate_binary_nums")


def main():
    test_generate_binary_nums()


if __name__ == "__main__":
    main()
