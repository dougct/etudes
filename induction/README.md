# Induction in Algorithm Design

A collection of algorithms leveraging mathematical induction. Each implementation demonstrates the inductive approach: establishing a base case and building solutions by extending smaller instances.

## Problem list

| Problem | File | Description |
|:--|:--|:--|
| [**Binary number generation**](generate_binary_numbers.py) | Python, C++ | Generate all binary numbers of n digits using recursive concatenation |
| [**Word permutations**](generate_permutations.py) | Python, C++ | Generate all permutations of a word using character interpolation technique |
| [**N-digit numbers summing to K**](generate_n_digit_numbers.py) | Python | Recursively generate all n-digit numbers that sum to K |

## Running Tests

```bash
# Run individual Python files
python3 generate_binary_numbers.py
python3 generate_permutations.py
python3 generate_n_digit_numbers.py

# Or run all Python files at once
make python

# Run C++ tests
make test

# Build C++ test runner
make all

# Clean build artifacts
make clean
```

## TODO: Future Problems

- [ ] Generate all increasing sequences of length k from numbers 1 to n
- [ ] Generate all possible dice roll combinations that sum to a target value
- [ ] Generate all k-element subsets from a set of n elements (combinations)
- [ ] Generate all subsets of a given set (power set generation)
- [ ] Generate all ways to partition a number into k parts
- [ ] Generate all valid parentheses combinations of length n
