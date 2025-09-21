#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "SingleThreadRingBuffer.h"


TEST(SingleThreadRingBufferTest, EmptyInitially) {
    SingleThreadRingBuffer<int> ring(10);
    EXPECT_TRUE(ring.empty());
    EXPECT_FALSE(ring.full());
    EXPECT_EQ(ring.sizeEstimate(), 0U);
    EXPECT_EQ(ring.capacity(), 9U);
}

TEST(SingleThreadRingBufferTest, SinglePushPop) {
    SingleThreadRingBuffer<int> ring(10);
    EXPECT_TRUE(ring.push(42));
    EXPECT_FALSE(ring.empty());
    EXPECT_FALSE(ring.full());
    EXPECT_EQ(ring.sizeEstimate(), 1U);

    int value;
    EXPECT_TRUE(ring.pop(value));
    EXPECT_EQ(value, 42);
    EXPECT_TRUE(ring.empty());
    EXPECT_EQ(ring.sizeEstimate(), 0U);
}

TEST(SingleThreadRingBufferTest, PopFromEmpty) {
    SingleThreadRingBuffer<int> ring(10);
    int value;
    EXPECT_FALSE(ring.pop(value));
}

TEST(SingleThreadRingBufferTest, FillToCapacity) {
    SingleThreadRingBuffer<int> ring(10);
    for (int i = 0; i < 9; ++i) {
        EXPECT_TRUE(ring.push(i));
        EXPECT_EQ(ring.sizeEstimate(), static_cast<size_t>(i + 1));
    }

    EXPECT_TRUE(ring.full());
    EXPECT_FALSE(ring.push(999));
}

TEST(SingleThreadRingBufferTest, EmptyAfterFill) {
    SingleThreadRingBuffer<int> ring(10);
    for (int i = 0; i < 9; ++i) {
        ring.push(i);
    }

    for (int i = 0; i < 9; ++i) {
        int value;
        EXPECT_TRUE(ring.pop(value));
        EXPECT_EQ(value, i);
        EXPECT_EQ(ring.sizeEstimate(), static_cast<size_t>(9 - i - 1));
    }

    EXPECT_TRUE(ring.empty());
}

TEST(SingleThreadRingBufferTest, FrontMethod) {
    SingleThreadRingBuffer<int> ring(10);
    EXPECT_EQ(ring.front(), nullptr);

    ring.push(123);
    int* front_ptr = ring.front();
    ASSERT_NE(front_ptr, nullptr);
    EXPECT_EQ(*front_ptr, 123);

    ring.push(456);
    front_ptr = ring.front();
    ASSERT_NE(front_ptr, nullptr);
    EXPECT_EQ(*front_ptr, 123);

    int value;
    ring.pop(value);
    front_ptr = ring.front();
    ASSERT_NE(front_ptr, nullptr);
    EXPECT_EQ(*front_ptr, 456);

    ring.pop(value);
    EXPECT_EQ(ring.front(), nullptr);
}

TEST(SingleThreadRingBufferTest, Wraparound) {
    SingleThreadRingBuffer<int> ring(10);

    // Fill ring with [0, 1, 2, 3, 4]
    for (int i = 0; i < 5; ++i) {
        ring.push(i);
    }

    // Pop first 3 elements [0, 1, 2], leaving [3, 4]
    for (int i = 0; i < 3; ++i) {
        int value;
        ring.pop(value);
        EXPECT_EQ(value, i);
    }

    // Add more elements to fill capacity: [3, 4, 5, 6, 7, 8, 9, 10, 11]
    for (int i = 5; i < 12; ++i) {
        EXPECT_TRUE(ring.push(i)) << "Failed to push element " << i;
    }

    EXPECT_TRUE(ring.full());

    // Should get remaining elements in FIFO order: [3, 4, 5, 6, 7, 8, 9, 10, 11]
    std::vector<int> expected = {3, 4, 5, 6, 7, 8, 9, 10, 11};
    std::vector<int> actual;

    int value;
    while (ring.pop(value)) {
        actual.push_back(value);
    }

    EXPECT_EQ(actual, expected);
}

TEST(SingleThreadRingBufferTest, AlternatingPushPop) {
    SingleThreadRingBuffer<int> ring(10);
    for (int i = 0; i < 100; ++i) {
        EXPECT_TRUE(ring.push(i));

        int value;
        EXPECT_TRUE(ring.pop(value));
        EXPECT_EQ(value, i);
        EXPECT_TRUE(ring.empty());
    }
}

TEST(SingleThreadRingBufferTest, LargeOperations) {
    SingleThreadRingBuffer<int> large_ring(1001);

    for (size_t i = 0; i < 1000; ++i) {
        EXPECT_TRUE(large_ring.push(static_cast<int>(i)));
    }

    for (size_t i = 0; i < 1000; ++i) {
        int value;
        EXPECT_TRUE(large_ring.pop(value));
        EXPECT_EQ(value, static_cast<int>(i));
    }
}

TEST(SingleThreadRingBufferTest, ComplexType) {
    auto string_ring = SingleThreadRingBuffer<std::string>(10);

    std::vector<std::string> test_strings = {
        "hello", "world", "lock", "free", "ring", "buffer", "test", "string", "data"
    };

    for (const auto& str : test_strings) {
        EXPECT_TRUE(string_ring.push(str));
    }

    for (const auto& expected : test_strings) {
        std::string actual;
        EXPECT_TRUE(string_ring.pop(actual));
        EXPECT_EQ(actual, expected);
    }

    EXPECT_TRUE(string_ring.empty());
}

TEST(SingleThreadRingBufferTest, MoveSemantics) {
    auto movable_ring = SingleThreadRingBuffer<std::unique_ptr<int>>(5);

    std::vector<int> original_values = {1, 2, 3, 4};

    for (int val : original_values) {
        auto ptr = std::make_unique<int>(val);
        EXPECT_TRUE(movable_ring.push(std::move(ptr)));
    }

    for (int expected : original_values) {
        std::unique_ptr<int> ptr;
        EXPECT_TRUE(movable_ring.pop(ptr));
        ASSERT_NE(ptr, nullptr);
        EXPECT_EQ(*ptr, expected);
    }

    EXPECT_TRUE(movable_ring.empty());
}

TEST(SingleThreadRingBufferTest, EmplaceBehavior) {
    auto pair_ring = SingleThreadRingBuffer<std::pair<int, std::string>>(5);

    EXPECT_TRUE(pair_ring.push(42, "answer"));
    EXPECT_TRUE(pair_ring.push(std::make_pair(100, "century")));

    std::pair<int, std::string> result;
    EXPECT_TRUE(pair_ring.pop(result));
    EXPECT_EQ(result.first, 42);
    EXPECT_EQ(result.second, "answer");

    EXPECT_TRUE(pair_ring.pop(result));
    EXPECT_EQ(result.first, 100);
    EXPECT_EQ(result.second, "century");

    EXPECT_TRUE(pair_ring.empty());
}

TEST(SingleThreadRingBufferTest, BoundaryConditions) {
    auto tiny_ring = SingleThreadRingBuffer<int>(2);
    EXPECT_EQ(tiny_ring.capacity(), 1U);

    EXPECT_TRUE(tiny_ring.push(42));
    EXPECT_TRUE(tiny_ring.full());
    EXPECT_FALSE(tiny_ring.push(43));

    int value;
    EXPECT_TRUE(tiny_ring.pop(value));
    EXPECT_EQ(value, 42);
    EXPECT_TRUE(tiny_ring.empty());

    EXPECT_TRUE(tiny_ring.push(100));
    EXPECT_TRUE(tiny_ring.full());
}

TEST(SingleThreadRingBufferTest, StateConsistency) {
    SingleThreadRingBuffer<int> ring(10);
    for (int cycle = 0; cycle < 10; ++cycle) {
        for (int i = 0; i < 5; ++i) {
            EXPECT_TRUE(ring.push(cycle * 10 + i));
        }

        EXPECT_EQ(ring.sizeEstimate(), 5U);
        EXPECT_FALSE(ring.empty());
        EXPECT_FALSE(ring.full());

        for (int i = 0; i < 5; ++i) {
            int value;
            EXPECT_TRUE(ring.pop(value));
            EXPECT_EQ(value, cycle * 10 + i);
        }

        EXPECT_TRUE(ring.empty());
        EXPECT_EQ(ring.sizeEstimate(), 0U);
    }
}

TEST(SingleThreadRingBufferTest, SizeEstimateAccuracy) {
    SingleThreadRingBuffer<int> ring(10);
    for (int i = 0; i < 9; ++i) {
        ring.push(i);
        EXPECT_EQ(ring.sizeEstimate(), static_cast<size_t>(i + 1));
    }

    for (int i = 0; i < 9; ++i) {
        int value;
        ring.pop(value);
        EXPECT_EQ(ring.sizeEstimate(), static_cast<size_t>(9 - i - 1));
    }
}

TEST(SingleThreadRingBufferTest, FrontPointerStability) {
    SingleThreadRingBuffer<int> ring(10);
    ring.push(42);
    int* ptr1 = ring.front();
    int* ptr2 = ring.front();
    EXPECT_EQ(ptr1, ptr2);

    *ptr1 = 999;
    EXPECT_EQ(*ring.front(), 999);

    int value;
    ring.pop(value);
    EXPECT_EQ(value, 999);
}

TEST(SingleThreadRingBufferTest, FullBufferOperations) {
    SingleThreadRingBuffer<int> ring(10);
    for (int i = 0; i < 9; ++i) {
        ring.push(i);
    }

    EXPECT_TRUE(ring.full());
    EXPECT_FALSE(ring.push(999));
    EXPECT_EQ(ring.sizeEstimate(), 9U);

    int* front = ring.front();
    ASSERT_NE(front, nullptr);
    EXPECT_EQ(*front, 0);

    int value;
    EXPECT_TRUE(ring.pop(value));
    EXPECT_EQ(value, 0);
    EXPECT_FALSE(ring.full());
    EXPECT_TRUE(ring.push(999));
    EXPECT_TRUE(ring.full());
}

TEST(SingleThreadRingBufferTest, StressTest) {
    auto stress_ring = SingleThreadRingBuffer<int>(1000);

    for (int round = 0; round < 100; ++round) {
        for (int i = 0; i < 999; ++i) {
            EXPECT_TRUE(stress_ring.push(round * 1000 + i));
        }

        EXPECT_TRUE(stress_ring.full());

        for (int i = 0; i < 999; ++i) {
            int value;
            EXPECT_TRUE(stress_ring.pop(value));
            EXPECT_EQ(value, round * 1000 + i);
        }

        EXPECT_TRUE(stress_ring.empty());
    }
}

TEST(SingleThreadRingBufferTest, PartialFillCycles) {
    SingleThreadRingBuffer<int> ring(10);
    for (int cycle = 0; cycle < 20; ++cycle) {
        int items_to_add = (cycle % 5) + 1;

        for (int i = 0; i < items_to_add; ++i) {
            EXPECT_TRUE(ring.push(cycle * 100 + i));
        }

        EXPECT_EQ(ring.sizeEstimate(), static_cast<size_t>(items_to_add));

        for (int i = 0; i < items_to_add; ++i) {
            int value;
            EXPECT_TRUE(ring.pop(value));
            EXPECT_EQ(value, cycle * 100 + i);
        }

        EXPECT_TRUE(ring.empty());
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
