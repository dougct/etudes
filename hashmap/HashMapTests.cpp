#include <optional>
#include <string>

#include "HashMap.h"
#include "gtest/gtest.h"

TEST(HashMap, PutAndGet) {
  HashMap<std::string, int> map;

  map.put("one", 1);
  map.put("two", 2);
  map.put("three", 3);

  EXPECT_EQ(map.get("one"), 1);
  EXPECT_EQ(map.get("two"), 2);
  EXPECT_EQ(map.get("three"), 3);
}

TEST(HashMap, GetMissingKey) {
  HashMap<std::string, int> map;

  EXPECT_EQ(map.get("missing"), std::nullopt);
}

TEST(HashMap, PutOverwritesExistingKey) {
  HashMap<std::string, int> map;

  map.put("key", 1);
  EXPECT_EQ(map.get("key"), 1);

  map.put("key", 2);
  EXPECT_EQ(map.get("key"), 2);
}

TEST(HashMap, RemoveExistingKey) {
  HashMap<std::string, int> map;

  map.put("key", 42);
  EXPECT_EQ(map.get("key"), 42);

  map.remove("key");
  EXPECT_EQ(map.get("key"), std::nullopt);
}

TEST(HashMap, RemoveMissingKey) {
  HashMap<std::string, int> map;

  // Should not throw or crash.
  map.remove("nonexistent");
}

TEST(HashMap, IntKeys) {
  HashMap<int, std::string> map;

  map.put(1, "one");
  map.put(2, "two");

  EXPECT_EQ(map.get(1), "one");
  EXPECT_EQ(map.get(2), "two");
  EXPECT_EQ(map.get(3), std::nullopt);

  map.remove(1);
  EXPECT_EQ(map.get(1), std::nullopt);
}

TEST(HashMap, ManyInsertions) {
  HashMap<int, int> map;

  for (int i = 0; i < 1000; ++i) {
    map.put(i, i * 10);
  }

  for (int i = 0; i < 1000; ++i) {
    EXPECT_EQ(map.get(i), i * 10);
  }
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
