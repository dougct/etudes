# Induction in Algorithm Design

A collection of algorithms leveraging mathematical induction. Each implementation demonstrates the inductive approach: establishing a base case and building solutions by extending smaller instances.

## Problem list

| Problem | File | Category | Description |
|:--|:--|:--|:--|
| [**Binary Number Generation**](generate_binary_numbers.py) | Python | String Generation | Generate all binary numbers of n digits using recursive concatenation |
| [**Word Permutations**](generate_permutations.py) | Python | Combinatorics | Generate all permutations of a word using character interpolation technique |
| [**N-Digit Numbers Summing to K**](generate_n_digit_numbers.py) | Python | Number Generation | Recursively generate all n-digit numbers that sum to K |

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

**Subset/Selection Problems:**
- [ ] Generate all k-element subsets from a set of n elements (combinations)
- [ ] Generate all subsets of a given set (power set generation)

**Sequence Generation:**
- [ ] Generate all increasing sequences of length k from numbers 1 to n
- [ ] Generate all possible dice roll combinations that sum to a target value

**Additional Combinatorial Problems:**
- [ ] Generate all ways to partition a number into k parts
- [ ] Generate all valid parentheses combinations of length n
- [ ] Generate all paths in a grid from top-left to bottom-right
- [ ] Generate all ways to climb n stairs (with 1 or 2 steps at a time)
