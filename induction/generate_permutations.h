#ifndef GENERATE_PERMUTATIONS_H
#define GENERATE_PERMUTATIONS_H

#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> interpolate(char ch, const std::string& word) {
  std::vector<std::string> res;
  res.reserve(word.size() + 1);
  for (size_t i = 0; i <= word.size(); i++) {
    // word.substr(0, i) will be empty when i == 0
    // word.substr(i) will be empty when i == word.size()
    auto interpolation = word.substr(0, i) + ch + word.substr(i);
    res.push_back(std::move(interpolation));
  }
  return res;
}

std::vector<std::string> generate_permutations(const std::string& word,
                                               int pos) {
  if (pos == 0) {
    return {word};
  }

  // Word with ch removed where ch is the char at word[pos]
  auto smaller_word = word.substr(0, pos) + word.substr(pos + 1);

  // All permutations of word without ch
  auto perms = generate_permutations(smaller_word, pos - 1);

  // Interpolate ch back into every position of every permutation of the word
  auto ch = word[pos];
  std::vector<std::string> res;
  for (const auto& p : perms) {
    auto interpolations = interpolate(ch, p);
    for (auto&& p : interpolations) {
      res.emplace_back(std::move(p));
    }
  }

  return res;
}

void print_permutations(std::string word) {
  auto perms = generate_permutations(word, word.size() - 1);
  for (const auto& p : perms) {
    std::cout << p << std::endl;
  }
  std::cout << std::endl;
}

#endif  // GENERATE_PERMUTATIONS_H
