/*
Problem:
    Generate all permutations of a given word

Solution:
    Base case (pos=0): word of size one
        Return the word as a single permutation

    Induction hypothesis:
        We know how to generate all permutations of a word with one less
character

    Induction step:
        Remove the character at position pos from the word.
        Generate all permutations of the word with the character removed (using
        induction hypothesis).
        For each permutation, interpolate the removed character at every
        possible position.
        Return the concatenation of all these interpolated results.
*/

#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <vector>

/*
Insert character ch at every possible position in word.

We need to iterate up to len(word) + 1 because we want to insert the character
ch at:
- Before the first character (position 0)
- Between each pair of characters (positions 1, 2, ..., len(word)-1)
- After the last character (position len(word))

For example, with word = "ab" (length 2), we need 3 positions:
- i = 0: "" + ch + "ab" --> "cab"
- i = 1: "a" + ch + "b" --> "acb"
- i = 2: "ab" + ch + "" --> "abc"
*/
std::vector<std::string> interpolate(char ch, const std::string& word) {
  std::vector<std::string> res;
  res.reserve(word.length() + 1);
  for (size_t i = 0; i <= word.length(); ++i) {
    // word.substr(0, i) will be empty when i == 0
    // word.substr(i) will be empty when i == word.length()
    auto interpolation = word.substr(0, i) + ch + word.substr(i);
    res.push_back(interpolation);
  }
  return res;
}

std::vector<std::string> generate_permutations(const std::string& word,
                                               size_t pos) {
  if (pos == 0) {
    return {word};
  }

  // Word with ch removed where ch is the char at word[pos]
  std::string smaller_word = word.substr(0, pos) + word.substr(pos + 1);

  // All permutations of word without ch
  auto perms = generate_permutations(smaller_word, pos - 1);

  std::vector<std::string> res;

  // Interpolate ch back into every position of every permutation of the word
  char ch = word[pos];
  for (const auto& perm : perms) {
    auto interpolations = interpolate(ch, perm);
    res.insert(res.end(), interpolations.begin(), interpolations.end());
  }

  return res;
}

void print_permutations(const std::string& word) {
  if (word.empty()) {
    return;
  }

  auto perms =
      generate_permutations(word, word.length() - 1);
  for (const auto& perm : perms) {
    std::cout << perm << "\n";
  }
  std::cout << "\n";
}

// Tests

void test_generate_permutations_a() {
  auto result = generate_permutations("A", 0);
  std::vector<std::string> expected = {"A"};
  assert(result == expected);
  std::cout << "test_generate_permutations_a passed\n";
}

void test_generate_permutations_ab() {
  auto result = generate_permutations("AB", 1);
  std::vector<std::string> expected = {"BA", "AB"};
  assert(result == expected);
  std::cout << "test_generate_permutations_ab passed\n";
}

void test_generate_permutations_abc() {
  auto result = generate_permutations("ABC", 2);
  std::vector<std::string> expected = {"CBA", "BCA", "BAC",
                                       "CAB", "ACB", "ABC"};
  assert(result == expected);
  std::cout << "test_generate_permutations_abc passed\n";
}

void test_generate_permutations_abcd() {
  auto result = generate_permutations("ABCD", 3);

  // ABCD has 24 permutations
  assert(result.size() == 24);

  // Check that all permutations are unique
  std::set<std::string> unique_perms(result.begin(), result.end());
  assert(unique_perms.size() == 24);

  // Check that each permutation contains all original characters
  for (const auto& perm : result) {
    assert(perm.length() == 4);
    assert(perm.find('A') != std::string::npos);
    assert(perm.find('B') != std::string::npos);
    assert(perm.find('C') != std::string::npos);
    assert(perm.find('D') != std::string::npos);
  }
  std::cout << "test_generate_permutations_abcd passed\n";
}

void test_interpolate() {
  auto result = interpolate('X', "AB");
  std::vector<std::string> expected = {"XAB", "AXB", "ABX"};
  assert(result == expected);
  std::cout << "test_interpolate passed\n";
}

int main() {
  test_generate_permutations_a();
  test_generate_permutations_ab();
  test_generate_permutations_abc();
  test_generate_permutations_abcd();
  test_interpolate();

  std::cout << "\nAll tests passed!\n";
  return 0;
}
