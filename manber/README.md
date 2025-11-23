# Manber: Inductive Algorithm Design

A collection of algorithms leveraging mathematical induction, implemented in Rust. Each implementation demonstrates the inductive approach: establishing a base case and building solutions by extending smaller instances.

## Problem list

| Problem | File | Description |
|:--|:--|:--|
| [**Binary number generation**](src/generate_binary_nums.rs) | Rust | Generate all binary numbers of n digits using recursive concatenation |
| [**Word permutations**](src/generate_permutations.rs) | Rust | Generate all permutations of a word using character interpolation technique |
| [**Polynomial evaluation**](src/evaluate_polynomials.rs) | Rust | Evaluate polynomials using Horner's rule with inductive approach |
| [**Binary tree balance factors**](src/balance_factors.rs) | Rust | Compute balance factors for all nodes in a binary tree |
| [**Maximum consecutive subsequence**](src/maximum_consecutive_subsequence.rs) | Rust | Find the maximum sum of consecutive elements using Kadane's algorithm |

## Running Tests

```bash
# Run all tests
cargo test

# Run tests for a specific module. Example:
cargo test generate_binary_nums

# Run tests in verbose mode
cargo test -- --nocapture

# Check code without running tests
cargo check

# Format code
cargo fmt

# Run with optimizations
cargo test --release
```

## Running Individual Modules

```bash
# Run the binary (shows help message)
cargo run

# Build the project
cargo build

# Build with optimizations
cargo build --release
```
