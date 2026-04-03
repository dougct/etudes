#include <string>
#include <vector>

#include "Ip2Cidr.h"
#include "gtest/gtest.h"

using std::string;
using std::vector;

TEST(Ip2Cidr, Ip2Int) {
  EXPECT_EQ(ip2int("0.0.0.0"), 0u);
  EXPECT_EQ(ip2int("0.0.0.1"), 1u);
  EXPECT_EQ(ip2int("192.168.1.0"), 0xC0A80100u);
  EXPECT_EQ(ip2int("255.255.255.255"), 0xFFFFFFFFu);
  EXPECT_EQ(ip2int("255.0.0.7"), 0xFF000007u);
}

TEST(Ip2Cidr, Int2Ip) {
  EXPECT_EQ(int2ip(0), "0.0.0.0");
  EXPECT_EQ(int2ip(1), "0.0.0.1");
  EXPECT_EQ(int2ip(0xC0A80100u), "192.168.1.0");
  EXPECT_EQ(int2ip(0xFFFFFFFFu), "255.255.255.255");
}

TEST(Ip2Cidr, TrailingZeroes) {
  EXPECT_EQ(trailing_zeroes(0), 32);
  EXPECT_EQ(trailing_zeroes(1), 0);
  EXPECT_EQ(trailing_zeroes(8), 3);
  EXPECT_EQ(trailing_zeroes(16), 4);
  EXPECT_EQ(trailing_zeroes(7), 0);
}

TEST(Ip2Cidr, SingleAddress) {
  // n=1 should always produce a /32
  vector<string> expected = {"255.0.0.7/32"};
  EXPECT_EQ(ipToCIDR("255.0.0.7", 1), expected);
}

TEST(Ip2Cidr, AlignedBlock) {
  // Starting at a perfectly aligned address
  vector<string> expected = {"192.168.1.0/24"};
  EXPECT_EQ(ipToCIDR("192.168.1.0", 256), expected);
}

TEST(Ip2Cidr, ExampleFromSolution) {
  // Walkthrough from the solution doc: ip="255.0.0.7", n=10
  vector<string> expected = {"255.0.0.7/32", "255.0.0.8/29", "255.0.0.16/32"};
  EXPECT_EQ(ipToCIDR("255.0.0.7", 10), expected);
}

TEST(Ip2Cidr, StartAtZero) {
  // Address 0 is aligned to every power of 2
  vector<string> expected = {"0.0.0.0/24"};
  EXPECT_EQ(ipToCIDR("0.0.0.0", 256), expected);
}

TEST(Ip2Cidr, PowerOfTwoCount) {
  // n is a power of 2 but start is odd (alignment 1)
  vector<string> expected = {"0.0.0.1/32", "0.0.0.2/31"};
  EXPECT_EQ(ipToCIDR("0.0.0.1", 3), expected);
}

TEST(Ip2Cidr, LeetCodeExample) {
  // LeetCode 751 example: ip="255.0.0.7", n=10
  // 255.0.0.7/32  -> covers 1 IP  (255.0.0.7)
  // 255.0.0.8/29  -> covers 8 IPs (255.0.0.8 - 255.0.0.15)
  // 255.0.0.16/32 -> covers 1 IP  (255.0.0.16)
  // Total: 1 + 8 + 1 = 10
  vector<string> expected = {"255.0.0.7/32", "255.0.0.8/29", "255.0.0.16/32"};
  EXPECT_EQ(ipToCIDR("255.0.0.7", 10), expected);
}

TEST(Ip2Cidr, ExactPowerOfTwoAligned) {
  // ip="1.0.0.0", n=1 -> single /32
  vector<string> expected = {"1.0.0.0/32"};
  EXPECT_EQ(ipToCIDR("1.0.0.0", 1), expected);
}

TEST(Ip2Cidr, TwoAddresses) {
  // ip="1.1.1.0", n=2 -> one /31 block
  vector<string> expected = {"1.1.1.0/31"};
  EXPECT_EQ(ipToCIDR("1.1.1.0", 2), expected);
}

TEST(Ip2Cidr, UnalignedMultipleBlocks) {
  // ip="10.0.0.1", n=2 -> odd start forces two /32 blocks
  vector<string> expected = {"10.0.0.1/32", "10.0.0.2/32"};
  EXPECT_EQ(ipToCIDR("10.0.0.1", 2), expected);
}

TEST(Ip2Cidr, LargerRange) {
  // ip="117.145.102.62", n=8
  // 62 = ...111110 -> 1 trailing zero, alignment allows /31 (2 IPs)
  // 62/31 covers 62-63 (2), 64/26 covers 64-127 (64) — too many, need 6
  // 64/30 covers 64-67 (4), 68/31 covers 68-69 (2) -> total 2+4+2 = 8
  vector<string> expected = {"117.145.102.62/31", "117.145.102.64/30",
                             "117.145.102.68/31"};
  EXPECT_EQ(ipToCIDR("117.145.102.62", 8), expected);
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
