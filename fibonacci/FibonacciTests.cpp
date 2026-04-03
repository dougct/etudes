#include <cstdint>

#include "Fibonacci.h"
#include "gtest/gtest.h"

TEST(Fibonacci, BaseCase0) {
  EXPECT_EQ(fibonacci(0), 0);
}

TEST(Fibonacci, BaseCase1) {
  EXPECT_EQ(fibonacci(1), 1);
}

TEST(Fibonacci, SmallValues) {
  EXPECT_EQ(fibonacci(2), 1);
  EXPECT_EQ(fibonacci(3), 2);
  EXPECT_EQ(fibonacci(4), 3);
  EXPECT_EQ(fibonacci(5), 5);
  EXPECT_EQ(fibonacci(6), 8);
  EXPECT_EQ(fibonacci(10), 55);
}

TEST(Fibonacci, LargerValues) {
  EXPECT_EQ(fibonacci(20), 6765);
  EXPECT_EQ(fibonacci(30), 832040);
  EXPECT_EQ(fibonacci(50), 12586269025LL);
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
