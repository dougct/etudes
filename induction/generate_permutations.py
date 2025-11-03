#!/usr/bin/env python3

"""
Problem:
    Generate all permutations of a given word

Solution:
    Base case (pos=0): word of size one
        Return the word as a single permutation

    Induction hypothesis:
        We know how to generate all permutations of a word with one less character

    Induction step:
        Remove the character at position pos from the word.
        Generate all permutations of the word with the character removed (using induction hypothesis).
        For each permutation, interpolate the removed character at every possible position.
        Return the concatenation of all these interpolated results.
"""

import math


def interpolate(ch, word):
    """
    Insert character ch at every possible position in word.

    We need to iterate up to len(word) + 1 because we want to insert the character ch at:
    - Before the first character (position 0)
    - Between each pair of characters (positions 1, 2, ..., len(word)-1)
    - After the last character (position len(word))

    For example, with word = "ab" (length 2), we need 3 positions:
    - i = 0: "" + ch + "ab" --> "cab"
    - i = 1: "a" + ch + "b" --> "acb"
    - i = 2: "ab" + ch + "" --> "abc"
    """
    return [word[:pos] + ch + word[pos:] for pos in range(len(word) + 1)]


def generate_permutations(word, pos):
    if pos == 0:
        return [word]

    # Word with ch removed where ch is the char at word[pos]
    curr_word = word[:pos] + word[pos + 1 :]

    # All permutations of word without ch
    perms = generate_permutations(curr_word, pos - 1)

    # Interpolate ch back into every position of every permutation of the word
    ch = word[pos]
    return [interpolation for perm in perms for interpolation in interpolate(ch, perm)]


def test_interpolate():
    # Basic cases
    assert interpolate("x", "") == ["x"]
    assert interpolate("a", "b") == ["ab", "ba"]
    assert interpolate("x", "ab") == ["xab", "axb", "abx"]
    assert interpolate("c", "ab") == ["cab", "acb", "abc"]

    # Single character word
    assert interpolate("z", "a") == ["za", "az"]

    # Longer words
    assert interpolate("x", "abc") == ["xabc", "axbc", "abxc", "abcx"]
    assert interpolate("y", "hello") == [
        "yhello",
        "hyello",
        "heyllo",
        "helylo",
        "hellyo",
        "helloy",
    ]

    # Check length property: should always be len(word) + 1
    test_cases = [
        ("a", ""),
        ("b", "x"),
        ("c", "xy"),
        ("d", "xyz"),
        ("e", "abcd"),
        ("f", "abcde"),
    ]
    for ch, word in test_cases:
        result = interpolate(ch, word)
        assert len(result) == len(word) + 1

    # Check that all results contain the character
    result = interpolate("z", "test")
    for item in result:
        assert "z" in item

    print("All tests passed: interpolate")


def test_generate_permutations():
    # Base cases
    assert generate_permutations("a", 0) == ["a"]

    # Two character strings
    assert set(generate_permutations("ab", 1)) == {"ab", "ba"}
    assert len(generate_permutations("ab", 1)) == 2

    # Three character strings
    result_abc = generate_permutations("abc", 2)
    expected_abc = {"abc", "acb", "bac", "bca", "cab", "cba"}
    assert set(result_abc) == expected_abc
    assert len(result_abc) == 6

    # Four character strings
    result_abcd = generate_permutations("abcd", 3)
    assert len(result_abcd) == 24  # 4!
    assert len(set(result_abcd)) == 24  # All should be unique

    # Check factorial property: n! permutations for n characters
    for word_len in range(1, 6):
        word = "".join(chr(ord("a") + i) for i in range(word_len))
        result = generate_permutations(word, word_len - 1)
        expected_count = math.factorial(word_len)
        assert len(result) == expected_count
        # All permutations should be unique
        assert len(set(result)) == expected_count

    # Check that all permutations contain same characters
    result = generate_permutations("xyz", 2)
    for perm in result:
        assert sorted(perm) == ["x", "y", "z"]

    # Test with repeated characters
    result_repeated = generate_permutations("aa", 1)
    # Should still generate 2 permutations even if identical
    assert len(result_repeated) == 2
    assert result_repeated == ["aa", "aa"]

    print("All tests passed: generate_permutations")


def main():
    test_interpolate()
    test_generate_permutations()


if __name__ == "__main__":
    main()
