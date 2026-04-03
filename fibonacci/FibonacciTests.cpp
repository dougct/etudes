#include <cstdint>

#include "Fibonacci.h"
#include "gtest/gtest.h"

TEST(Fibonacci, BaseCase0) {
  EXPECT_EQ(exp_fib(0), 0ULL);
  EXPECT_EQ(lin_fib(0), 0ULL);
}

TEST(Fibonacci, BaseCase1) {
  EXPECT_EQ(exp_fib(1), 1);
  EXPECT_EQ(lin_fib(1), 1);
}

TEST(Fibonacci, SmallValues) {
  EXPECT_EQ(exp_fib(2), 1);
  EXPECT_EQ(exp_fib(3), 2);
  EXPECT_EQ(exp_fib(4), 3);
  EXPECT_EQ(exp_fib(5), 5);
  EXPECT_EQ(exp_fib(6), 8);
  EXPECT_EQ(exp_fib(7), 13);
  EXPECT_EQ(exp_fib(8), 21);
  EXPECT_EQ(exp_fib(9), 34);
  EXPECT_EQ(exp_fib(10), 55);
  EXPECT_EQ(exp_fib(11), 89);
  EXPECT_EQ(exp_fib(12), 144);

  EXPECT_EQ(lin_fib(2), 1);
  EXPECT_EQ(lin_fib(3), 2);
  EXPECT_EQ(lin_fib(4), 3);
  EXPECT_EQ(lin_fib(5), 5);
  EXPECT_EQ(lin_fib(6), 8);
  EXPECT_EQ(lin_fib(7), 13);
  EXPECT_EQ(lin_fib(8), 21);
  EXPECT_EQ(lin_fib(9), 34);
  EXPECT_EQ(lin_fib(10), 55);
  EXPECT_EQ(lin_fib(11), 89);
  EXPECT_EQ(lin_fib(12), 144);
}

TEST(Fibonacci, LargerValues) {
  EXPECT_EQ(lin_fib(20), 6765ULL);
  EXPECT_EQ(lin_fib(30), 832040ULL);
  EXPECT_EQ(lin_fib(40), 102334155ULL);
  EXPECT_EQ(lin_fib(50), 12586269025ULL);
  EXPECT_EQ(lin_fib(60), 1548008755920ULL);
  EXPECT_EQ(lin_fib(70), 190392490709135ULL);
  EXPECT_EQ(lin_fib(80), 23416728348467685ULL);
  EXPECT_EQ(lin_fib(90), 2880067194370816120ULL);
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
