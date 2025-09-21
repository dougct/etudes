#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

#include "gtest/gtest.h"

#include "LockFreeRingBuffer.h"

// Verifies simple push/pop operations work correctly
TEST(LockFreeRingBuffer, SimpleTest) {
    int numItems = 10;
    LockFreeRingBuffer<int> ring(numItems + 1);

    // Verify ring starts empty
    EXPECT_TRUE(ring.empty());
    EXPECT_FALSE(ring.full());
    EXPECT_EQ(ring.sizeEstimate(), 0U);

    // Push a single item and verify it's accessible
    EXPECT_TRUE(ring.push(1));
    EXPECT_FALSE(ring.empty());
    EXPECT_EQ(ring.sizeEstimate(), 1U);
    EXPECT_EQ(*ring.front(), 1);

    // Pop the item and verify ring is empty again
    int value;
    EXPECT_TRUE(ring.pop(value));
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(ring.empty());
    EXPECT_EQ(ring.sizeEstimate(), 0U);
}

// Verifies the ring can be completely filled and emptied
TEST(LockFreeRingBuffer, PopulateTest) {
    int numItems = 10;
    LockFreeRingBuffer<int> ring(numItems + 1);

    // Fill the ring to capacity
    for (int i = 0; i < numItems; i++) {
        EXPECT_TRUE(ring.push(i));
        EXPECT_EQ(ring.sizeEstimate(), static_cast<size_t>(i + 1));
    }

    // Verify ring is full and cannot accept more items
    EXPECT_TRUE(ring.full());
    EXPECT_EQ(ring.sizeEstimate(), ring.capacity());
    EXPECT_FALSE(ring.push(999));

    // Drain the ring completely, verifying FIFO order
    for (int i = 0; i < numItems; i++) {
        int value;
        EXPECT_TRUE(ring.pop(value));
        EXPECT_EQ(value, i);
        EXPECT_EQ(ring.sizeEstimate(), static_cast<size_t>(numItems - i - 1));
    }

    // Verify ring is completely empty
    EXPECT_TRUE(ring.empty());
    EXPECT_EQ(ring.sizeEstimate(), 0U);
}

// Verifies repeated push/pop cycles work correctly
TEST(LockFreeRingBuffer, AlternatingPushPopTest) {
    int numItems = 10;
    LockFreeRingBuffer<int> ring(numItems + 1);

    // Perform many alternating push/pop operations
    for (int i = 0; i < numItems; i++) {
        EXPECT_TRUE(ring.push(i));
        EXPECT_FALSE(ring.empty());
        EXPECT_EQ(ring.sizeEstimate(), 1U);

        int value;
        EXPECT_TRUE(ring.pop(value));
        EXPECT_EQ(value, i);
        EXPECT_TRUE(ring.empty());
        EXPECT_EQ(ring.sizeEstimate(), 0U);
    }
}

// Verifies front() returns correct pointer without consuming item
TEST(LockFreeRingBuffer, FrontPtrTest) {
    int numItems = 100;
    LockFreeRingBuffer<int> ring(numItems + 1);

    // Test front() behavior with alternating push/pop operations
    for (int i = 0; i < numItems; i++) {
        EXPECT_TRUE(ring.push(i));
        const int* frontPtr = ring.front();
        ASSERT_NE(frontPtr, nullptr);
        EXPECT_EQ(*frontPtr, i);

        // Verify size unchanged after front() call
        EXPECT_EQ(ring.sizeEstimate(), 1U);

        // Pop and verify we get the same value as front()
        int value;
        EXPECT_TRUE(ring.pop(value));
        EXPECT_EQ(value, i);
        EXPECT_TRUE(ring.empty());
    }
}

// Verifies front() can be used for in-place operations
TEST(LockFreeRingBuffer, InPlaceModificationTest) {
    int numItems = 100;
    LockFreeRingBuffer<int> ring(numItems + 1);

    // Test in-place access and modification through front()
    for (int i = 0; i < numItems; i++) {
        EXPECT_TRUE(ring.push(i));
        int* front = ring.front();
        ASSERT_NE(front, nullptr);
        EXPECT_EQ(*front, i);

        // Modify value in-place
        *front = i * 2;
        EXPECT_EQ(*front, i * 2);

        int value;
        EXPECT_TRUE(ring.pop(value));
        EXPECT_EQ(value, i * 2);
        EXPECT_TRUE(ring.empty());
    }
}

// Verifies ring works with smallest possible size
TEST(LockFreeRingBuffer, MinimumSizeTest) {
    // Test with minimum valid size (capacity of 1)
    LockFreeRingBuffer<int> ring(2);
    EXPECT_EQ(ring.capacity(), 1U);

    // Verify single item operations
    EXPECT_TRUE(ring.push(42));
    EXPECT_TRUE(ring.full());
    EXPECT_FALSE(ring.push(43));

    int value;
    EXPECT_TRUE(ring.pop(value));
    EXPECT_EQ(value, 42);
    EXPECT_TRUE(ring.empty());
}

// Verifies index wraparound works correctly
TEST(LockFreeRingBuffer, SimpleWrapAroundTest) {
    LockFreeRingBuffer<int> ring(4);  // Capacity of 3

    // Fill ring completely
    EXPECT_TRUE(ring.push(1));
    EXPECT_TRUE(ring.push(2));
    EXPECT_TRUE(ring.push(3));
    EXPECT_TRUE(ring.full());

    // Pop one item to make space
    int value;
    EXPECT_TRUE(ring.pop(value));
    EXPECT_EQ(value, 1);
    EXPECT_FALSE(ring.full());

    EXPECT_TRUE(ring.push(4));
    EXPECT_TRUE(ring.full());

    // Verify remaining items are in correct FIFO order
    EXPECT_TRUE(ring.pop(value));
    EXPECT_EQ(value, 2);
    EXPECT_TRUE(ring.pop(value));
    EXPECT_EQ(value, 3);
    EXPECT_TRUE(ring.pop(value));
    EXPECT_EQ(value, 4);
    EXPECT_TRUE(ring.empty());
}

// Verifies wrap around behavior
TEST(LockFreeRingBuffer, WrapAroundTest) {
    LockFreeRingBuffer<int> ring(10);  // Capacity of 9
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
        EXPECT_TRUE(ring.push(i));
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


// Verifies correct behavior when ring is empty
TEST(LockFreeRingBuffer, FrontOnEmptyTest) {
    LockFreeRingBuffer<int> ring(10);

    // Verify front() returns null on empty ring
    EXPECT_EQ(ring.front(), nullptr);

    // Add item and verify front() works
    EXPECT_TRUE(ring.push(42));
    int* frontPtr = ring.front();
    ASSERT_NE(frontPtr, nullptr);
    EXPECT_EQ(*frontPtr, 42);

    // Remove item and verify front() returns null again
    int value;
    EXPECT_TRUE(ring.pop(value));
    EXPECT_EQ(ring.front(), nullptr);
}

// Verifies proper failure when popping empty ring
TEST(LockFreeRingBuffer, PopOnEmptyTest) {
    LockFreeRingBuffer<int> ring(10);

    // Attempt to pop from empty ring
    int value = 999;  // Sentinel value to ensure it's not modified
    EXPECT_FALSE(ring.pop(value));
    EXPECT_EQ(value, 999);

    // Verify ring state unchanged
    EXPECT_TRUE(ring.empty());
    EXPECT_EQ(ring.sizeEstimate(), 0U);
}

// Verifies sizeEstimate() returns correct values
TEST(LockFreeRingBuffer, SizeEstimateTest) {
    LockFreeRingBuffer<int> ring(5);  // Capacity of 4

    // Test size progression while filling
    EXPECT_EQ(ring.sizeEstimate(), 0U);

    ring.push(1);
    EXPECT_EQ(ring.sizeEstimate(), 1U);

    ring.push(2);
    ring.push(3);
    EXPECT_EQ(ring.sizeEstimate(), 3U);

    // Test size progression while draining
    int value;
    ring.pop(value);
    EXPECT_EQ(ring.sizeEstimate(), 2U);

    // Fill to capacity and verify
    ring.push(4);
    ring.push(5);
    EXPECT_EQ(ring.sizeEstimate(), 4U);
    EXPECT_EQ(ring.capacity(), ring.sizeEstimate());
}

// Verifies proper handling of move-only and moveable types
TEST(LockFreeRingBuffer, MoveSemanticTest) {
    LockFreeRingBuffer<std::string> ring(5);

    // Test moving strings into ring
    std::string str1 = "hello_world_test_string";
    std::string str2 = "another_test_string";

    // Move strings into ring (should not copy)
    EXPECT_TRUE(ring.push(std::move(str1)));
    EXPECT_TRUE(ring.push(std::move(str2)));

    // Moved-from state
    EXPECT_TRUE(str1.empty());
    EXPECT_TRUE(str2.empty());

    EXPECT_EQ(ring.sizeEstimate(), 2U);

    // Pop and verify contents
    std::string result;
    EXPECT_TRUE(ring.pop(result));
    EXPECT_EQ(result, "hello_world_test_string");

    EXPECT_TRUE(ring.pop(result));
    EXPECT_EQ(result, "another_test_string");

    EXPECT_TRUE(ring.empty());
}

// Verifies ring handles many fill/empty cycles correctly
TEST(LockFreeRingBuffer, MultipleCyclesTest) {
    LockFreeRingBuffer<int> ring(5);  // Capacity of 4
    const int cycles = 1000;

    // Perform many complete fill/empty cycles
    for (int cycle = 0; cycle < cycles; ++cycle) {
        // Fill the ring completely
        for (int i = 0; i < 4; ++i) {
            int pushValue = cycle * 10 + i;
            EXPECT_TRUE(ring.push(pushValue));
        }
        EXPECT_TRUE(ring.full());

        // Empty the ring completely, verifying order
        for (int i = 0; i < 4; ++i) {
            int value;
            EXPECT_TRUE(ring.pop(value));
            int expectedValue = cycle * 10 + i;
            EXPECT_EQ(value, expectedValue);
        }
        EXPECT_TRUE(ring.empty());
    }
}

// Verifies performance and correctness at scale
TEST(LockFreeRingBuffer, LargeOperationsTest) {
    const int ringSize = 1000;
    const int numOperations = 10000;
    LockFreeRingBuffer<int> ring(ringSize);

    // Perform large number of alternating operations
    for (int i = 0; i < numOperations; ++i) {
        EXPECT_TRUE(ring.push(i));
        EXPECT_EQ(ring.sizeEstimate(), 1U);

        int* front = ring.front();
        ASSERT_NE(front, nullptr);
        EXPECT_EQ(*front, i);

        int value;
        EXPECT_TRUE(ring.pop(value));
        EXPECT_EQ(value, i);
        EXPECT_TRUE(ring.empty());
    }
}

// Concurrent producer/consumer correctness test
TEST(LockFreeRingBuffer, ConcurrentCorrectnessTest) {
    const size_t num_items = 10000;
    LockFreeRingBuffer<size_t> ring(1000);

    std::atomic<bool> start_flag(false);
    std::vector<size_t> produced_values;
    std::vector<size_t> consumed_values;
    std::mutex consumed_mutex;

    // Generate expected sequence
    for (size_t i = 0; i < num_items; ++i) {
        produced_values.push_back(i);
    }

    std::thread producer([&] {
        while (!start_flag) std::this_thread::yield();

        for (size_t i = 0; i < num_items; ++i) {
            while (!ring.push(i)) {
                std::this_thread::yield();
            }
        }
    });

    std::thread consumer([&] {
        while (!start_flag) std::this_thread::yield();

        size_t consumed = 0;
        size_t value;
        while (consumed < num_items) {
            if (ring.pop(value)) {
                std::lock_guard<std::mutex> lock(consumed_mutex);
                consumed_values.push_back(value);
                consumed++;
            } else {
                std::this_thread::yield();
            }
        }
    });

    start_flag = true;
    producer.join();
    consumer.join();

    // Verify FIFO ordering
    EXPECT_EQ(consumed_values.size(), num_items);
    EXPECT_EQ(consumed_values, produced_values);
}

// Test producer faster than consumer
TEST(LockFreeRingBuffer, ProducerFasterTest) {
    LockFreeRingBuffer<size_t> ring(100);
    const size_t num_items = 1000;

    std::atomic<bool> start_flag(false);
    std::atomic<size_t> items_produced(0);
    std::vector<size_t> consumed_values;
    std::mutex consumed_mutex;

    std::thread fast_producer([&] {
        while (!start_flag) std::this_thread::yield();

        for (size_t i = 0; i < num_items; ++i) {
            while (!ring.push(i)) {
                std::this_thread::yield();
            }
            items_produced++;
        }
    });

    std::thread slow_consumer([&] {
        while (!start_flag) std::this_thread::yield();

        size_t value;
        while (consumed_values.size() < num_items) {
            if (ring.pop(value)) {
                std::lock_guard<std::mutex> lock(consumed_mutex);
                consumed_values.push_back(value);
            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(1)); // Slow consumer
            }
        }
    });

    start_flag = true;
    fast_producer.join();
    slow_consumer.join();

    EXPECT_EQ(items_produced.load(), num_items);
    EXPECT_EQ(consumed_values.size(), num_items);

    // Verify ordering
    for (size_t i = 0; i < num_items; ++i) {
        EXPECT_EQ(consumed_values[i], i);
    }
}

// Memory ordering stress test
TEST(LockFreeRingBuffer, MemoryOrderingStressTest) {
    LockFreeRingBuffer<size_t> ring(2);  // Very small buffer to force contention
    const size_t num_items = 10000;

    std::atomic<bool> start_flag(false);
    std::vector<size_t> consumed_values;
    std::mutex consumed_mutex;

    std::thread producer([&] {
        while (!start_flag) std::this_thread::yield();

        for (size_t i = 0; i < num_items; ++i) {
            while (!ring.push(i)) {
                // High contention - should stress memory ordering
            }
        }
    });

    std::thread consumer([&] {
        while (!start_flag) std::this_thread::yield();

        size_t consumed = 0;
        size_t value;
        while (consumed < num_items) {
            if (ring.pop(value)) {
                std::lock_guard<std::mutex> lock(consumed_mutex);
                consumed_values.push_back(value);
                consumed++;
            }
        }
    });

    start_flag = true;
    producer.join();
    consumer.join();

    // Verify all items consumed in order
    EXPECT_EQ(consumed_values.size(), num_items);
    for (size_t i = 0; i < num_items; ++i) {
        EXPECT_EQ(consumed_values[i], i);
    }
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
