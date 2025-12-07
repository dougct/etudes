/*
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
*/

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> generate_binary_nums(size_t n) {
    if (n == 0) {
        return {""};
    }

    std::vector<std::string> binary_nums = generate_binary_nums(n - 1);

    std::vector<std::string> res;
    for (const auto& num : binary_nums) {
        for (const char* suffix : {"0", "1"}) {
            res.push_back(num + suffix);
        }
    }

    return res;
}

void print_binary_nums(size_t n) {
    std::vector<std::string> nums = generate_binary_nums(n);
    for (const auto& num : nums) {
        std::cout << num << "\n";
    }
    std::cout << "\n";
}

// Tests

void test_generate_binary_nums_0() {
    auto result = generate_binary_nums(0);
    std::vector<std::string> expected = {""};
    assert(result == expected);
    std::cout << "test_generate_binary_nums_0 passed\n";
}

void test_generate_binary_nums_1() {
    auto result = generate_binary_nums(1);
    std::vector<std::string> expected = {"0", "1"};
    assert(result == expected);
    std::cout << "test_generate_binary_nums_1 passed\n";
}

void test_generate_binary_nums_2() {
    auto result = generate_binary_nums(2);
    std::vector<std::string> expected = {"00", "01", "10", "11"};
    assert(result == expected);
    std::cout << "test_generate_binary_nums_2 passed\n";
}

void test_generate_binary_nums_3() {
    auto result = generate_binary_nums(3);
    std::vector<std::string> expected = {"000", "001", "010", "011", "100", "101", "110", "111"};
    assert(result == expected);
    std::cout << "test_generate_binary_nums_3 passed\n";
}

int main() {
    test_generate_binary_nums_0();
    test_generate_binary_nums_1();
    test_generate_binary_nums_2();
    test_generate_binary_nums_3();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
