#ifndef GENERATE_BINARY_NUMS_H
#define GENERATE_BINARY_NUMS_H

#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> generate_binary_nums(int n) {
  if (n == 0) {
    return {""};
  }

  auto binary_nums = generate_binary_nums(n - 1);

  std::vector<std::string> res;
  for (const auto& num : binary_nums) {
    for (const auto& suffix : {"0", "1"}) {
      res.push_back(num + suffix);
    }
  }

  return res;
}

void print_binary_nums(int n) {
  auto nums = generate_binary_nums(n);
  for (const auto& n : nums) {
    std::cout << n << std::endl;
  }
  std::cout << std::endl;
}

#endif  // GENERATE_BINARY_NUMS_H
