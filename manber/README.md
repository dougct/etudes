# Manber: Inductive Algorithm Design

A collection of algorithms leveraging mathematical induction, implemented in C++. Each implementation demonstrates the inductive approach: establishing a base case and building solutions by extending smaller instances. These are inspired by the approach described by Udi Manber in his book "Introduction to Algorithms: A creative approach".

## Problem list

| Problem | File |
|:--|:--|
| **Binary number generation** | [generate_binary_nums.cpp](generate_binary_nums.cpp) |
| **Word permutations** | [generate_permutations.cpp](generate_permutations.cpp) |
| **Polynomial evaluation** | [evaluate_polynomials.cpp](evaluate_polynomials.cpp) |
| **Binary tree balance factors** | [balance_factors.cpp](balance_factors.cpp) |
| **Maximum consecutive subsequence** | [maximum_consecutive_subsequence.cpp](maximum_consecutive_subsequence.cpp) |
| **Longest increasing subsequence** | [lis.cpp](lis.cpp) |

## Running Tests

```bash
# Compile and run a specific file
make balance_factors
make lis
make generate_permutations

# Clean build artifacts
make clean

# Or compile and run separately
clang++ -std=c++17 -Wall -Wextra -O2 -o lis.bin lis.cpp
./lis.bin
```
