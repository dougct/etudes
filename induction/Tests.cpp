#include <gtest/gtest.h>
#include "generate_binary_nums.h"
#include "generate_permutations.h"

class BinaryNumsTest : public ::testing::Test {};

TEST_F(BinaryNumsTest, TestGenerateBinaryNums0) {
  auto result = generate_binary_nums(0);
  std::vector<std::string> expected = {""};
  EXPECT_EQ(result, expected);
}

TEST_F(BinaryNumsTest, TestGenerateBinaryNums1) {
  auto result = generate_binary_nums(1);
  std::vector<std::string> expected = {"0", "1"};
  EXPECT_EQ(result, expected);
}

TEST_F(BinaryNumsTest, TestGenerateBinaryNums2) {
  auto result = generate_binary_nums(2);
  std::vector<std::string> expected = {"00", "01", "10", "11"};
  EXPECT_EQ(result, expected);
}

TEST_F(BinaryNumsTest, TestGenerateBinaryNums3) {
  auto result = generate_binary_nums(3);
  std::vector<std::string> expected = {"000", "001", "010", "011",
                                       "100", "101", "110", "111"};
  EXPECT_EQ(result, expected);
}

class PermutationsTest : public ::testing::Test {};

TEST_F(PermutationsTest, TestGeneratePermutationsA) {
  auto result = generate_permutations("A", 0);
  std::vector<std::string> expected = {"A"};
  EXPECT_EQ(result, expected);
}

TEST_F(PermutationsTest, TestGeneratePermutationsAB) {
  auto result = generate_permutations("AB", 1);
  std::vector<std::string> expected = {"BA", "AB"};
  EXPECT_EQ(result, expected);
}

TEST_F(PermutationsTest, TestGeneratePermutationsABC) {
  auto result = generate_permutations("ABC", 2);
  std::vector<std::string> expected = {"CBA", "BCA", "BAC", "CAB", "ACB", "ABC"};
  EXPECT_EQ(result, expected);
}

TEST_F(PermutationsTest, TestGeneratePermutationsABCD) {
  auto result = generate_permutations("ABCD", 3);
  // ABCD has 24 permutations
  EXPECT_EQ(result.size(), 24);

  // Check that all permutations are unique
  std::set<std::string> unique_perms(result.begin(), result.end());
  EXPECT_EQ(unique_perms.size(), 24);

  // Check that each permutation contains all original characters
  for (const auto& perm : result) {
    EXPECT_EQ(perm.size(), 4);
    EXPECT_NE(perm.find('A'), std::string::npos);
    EXPECT_NE(perm.find('B'), std::string::npos);
    EXPECT_NE(perm.find('C'), std::string::npos);
    EXPECT_NE(perm.find('D'), std::string::npos);
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}