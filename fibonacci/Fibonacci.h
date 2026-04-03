#pragma once

#include <cassert>

uint64_t exp_fib(int n) {
  assert(n >= 0 && "n must be non-negative");

  if (n <= 1) {
    return n;
  }

  return exp_fib(n - 1) + exp_fib(n - 2);
}

uint64_t lin_fib(int n) {
  assert(n >= 0 && "n must be non-negative");

  if (n == 0) {
    return 0;
  }

  uint64_t fib_n_minus_two = 0;
  uint64_t fib_n_minus_one = 1;
  uint64_t fib_n = fib_n_minus_one + fib_n_minus_two;
  for (int x = 2; x < n; x++) {
    fib_n_minus_two = fib_n_minus_one;
    fib_n_minus_one = fib_n;
    fib_n = fib_n_minus_one + fib_n_minus_two;
  }

  return fib_n;
}


