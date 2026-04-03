#include <string>

#include "Trie.h"
#include "gtest/gtest.h"

TEST(Trie, InsertAndSearch) {
  Trie trie;

  trie.insert("apple");
  trie.insert("banana");

  EXPECT_TRUE(trie.search("apple"));
  EXPECT_TRUE(trie.search("banana"));
}

TEST(Trie, SearchMissingWord) {
  Trie trie;

  trie.insert("apple");

  EXPECT_FALSE(trie.search("app"));
  EXPECT_FALSE(trie.search("orange"));
  EXPECT_FALSE(trie.search(""));
}

TEST(Trie, hasPrefix) {
  Trie trie;

  trie.insert("apple");
  trie.insert("application");
  trie.insert("banana");

  EXPECT_TRUE(trie.hasPrefix("app"));
  EXPECT_TRUE(trie.hasPrefix("apple"));
  EXPECT_TRUE(trie.hasPrefix("ban"));
  EXPECT_FALSE(trie.hasPrefix("cat"));
  EXPECT_FALSE(trie.hasPrefix("bane"));
}

TEST(Trie, PrefixIsNotAWord) {
  Trie trie;

  trie.insert("apple");

  EXPECT_TRUE(trie.hasPrefix("app"));
  EXPECT_FALSE(trie.search("app"));
}

TEST(Trie, InsertDuplicates) {
  Trie trie;

  trie.insert("hello");
  trie.insert("hello");

  EXPECT_TRUE(trie.search("hello"));
}

TEST(Trie, SingleCharacterWords) {
  Trie trie;

  trie.insert("a");
  trie.insert("b");

  EXPECT_TRUE(trie.search("a"));
  EXPECT_TRUE(trie.search("b"));
  EXPECT_FALSE(trie.search("c"));
  EXPECT_TRUE(trie.hasPrefix("a"));
}

TEST(Trie, OverlappingWords) {
  Trie trie;

  trie.insert("car");
  trie.insert("card");
  trie.insert("care");
  trie.insert("cars");

  EXPECT_TRUE(trie.search("car"));
  EXPECT_TRUE(trie.search("card"));
  EXPECT_TRUE(trie.search("care"));
  EXPECT_TRUE(trie.search("cars"));
  EXPECT_FALSE(trie.search("ca"));
  EXPECT_TRUE(trie.hasPrefix("car"));
  EXPECT_TRUE(trie.hasPrefix("ca"));
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
