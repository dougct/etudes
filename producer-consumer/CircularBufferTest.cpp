#include "AtomicCircularBuffer.h"
#include "CircularBuffer.h"
#include "LockFreeCircularBuffer.h"
#include "SimpleCircularBuffer.h"

#include <gtest/gtest.h>

#include <string>
#include <thread>
#include <vector>

// ============================================================================
// Type-parameterized tests for both buffer implementations
// ============================================================================

// Wrapper to provide a uniform interface for typed tests
template <typename T, size_t Capacity>
struct SimpleCircularBufferWrapper {
  using BufferType = SimpleCircularBuffer<T, Capacity>;
};

template <typename T, size_t Capacity>
struct CircularBufferWrapper {
  using BufferType = CircularBuffer<T, Capacity>;
};

template <typename T, size_t Capacity>
struct AtomicCircularBufferWrapper {
  using BufferType = AtomicCircularBuffer<T, Capacity>;
};

template <typename T, size_t Capacity>
struct LockFreeCircularBufferWrapper {
  using BufferType = LockFreeCircularBuffer<T, Capacity>;
};

// Define the types to test
using BufferTypes = ::testing::Types<SimpleCircularBufferWrapper<int, 10>,
                                     CircularBufferWrapper<int, 10>,
                                     AtomicCircularBufferWrapper<int, 10>,
                                     LockFreeCircularBufferWrapper<int, 10>>;

template <typename T>
class BufferTest : public ::testing::Test {
 protected:
  typename T::BufferType buffer;
};

TYPED_TEST_SUITE(BufferTest, BufferTypes);

TYPED_TEST(BufferTest, InitiallyEmpty) {
  EXPECT_TRUE(this->buffer.empty());
  EXPECT_FALSE(this->buffer.full());
  EXPECT_EQ(this->buffer.size(), 0u);
}

TYPED_TEST(BufferTest, PushOneItem) {
  EXPECT_TRUE(this->buffer.try_push(42));
  EXPECT_FALSE(this->buffer.empty());
  EXPECT_EQ(this->buffer.size(), 1u);
}

TYPED_TEST(BufferTest, PopOneItem) {
  this->buffer.try_push(42);
  int item;
  ASSERT_TRUE(this->buffer.try_pop(item));
  EXPECT_EQ(item, 42);
  EXPECT_TRUE(this->buffer.empty());
}

TYPED_TEST(BufferTest, PopFromEmptyReturnsFalse) {
  int item;
  EXPECT_FALSE(this->buffer.try_pop(item));
}

TYPED_TEST(BufferTest, FrontReturnsNullptrWhenEmpty) {
  EXPECT_EQ(this->buffer.front(), nullptr);
}

TYPED_TEST(BufferTest, FrontReturnsPointerToFirstItem) {
  this->buffer.try_push(42);
  this->buffer.try_push(43);
  int* front = this->buffer.front();
  ASSERT_NE(front, nullptr);
  EXPECT_EQ(*front, 42);
}

TYPED_TEST(BufferTest, FillToCapacity) {
  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(this->buffer.try_push(i));
  }
  EXPECT_TRUE(this->buffer.full());
  EXPECT_EQ(this->buffer.size(), 10u);
}

TYPED_TEST(BufferTest, PushWhenFullReturnsFalse) {
  for (int i = 0; i < 10; ++i) {
    this->buffer.try_push(i);
  }
  EXPECT_FALSE(this->buffer.try_push(100));
}

TYPED_TEST(BufferTest, FIFOOrder) {
  for (int i = 0; i < 5; ++i) {
    this->buffer.try_push(i);
  }
  for (int i = 0; i < 5; ++i) {
    int item;
    ASSERT_TRUE(this->buffer.try_pop(item));
    EXPECT_EQ(item, i);
  }
}

TYPED_TEST(BufferTest, WrapAround) {
  // Fill buffer
  for (int i = 0; i < 10; ++i) {
    this->buffer.try_push(i);
  }
  // Pop half
  for (int i = 0; i < 5; ++i) {
    int item;
    this->buffer.try_pop(item);
  }
  // Push more (should wrap around)
  for (int i = 10; i < 15; ++i) {
    EXPECT_TRUE(this->buffer.try_push(i));
  }
  // Verify order
  for (int i = 5; i < 15; ++i) {
    int item;
    ASSERT_TRUE(this->buffer.try_pop(item));
    EXPECT_EQ(item, i);
  }
}

TYPED_TEST(BufferTest, Capacity) {
  EXPECT_EQ(this->buffer.capacity(), 10u);
}

TYPED_TEST(BufferTest, ProducerConsumerThreads) {
  constexpr int NUM_ITEMS = 100;

  std::thread producer([this]() {
    for (int i = 0; i < NUM_ITEMS; ++i) {
      while (!this->buffer.try_push(i * 2)) {
        // Spin until we can push
      }
    }
  });

  std::vector<int> consumed;
  consumed.reserve(NUM_ITEMS);

  std::thread consumer([this, &consumed]() {
    for (int i = 0; i < NUM_ITEMS; ++i) {
      int item;
      while (!this->buffer.try_pop(item)) {
        // Spin until we can pop
      }
      consumed.push_back(item);
    }
  });

  producer.join();
  consumer.join();

  // Verify all items were consumed in order
  ASSERT_EQ(consumed.size(), static_cast<size_t>(NUM_ITEMS));
  for (int i = 0; i < NUM_ITEMS; ++i) {
    EXPECT_EQ(consumed[i], i * 2);
  }
}

// Stress Tests (larger scale)

template <typename BufferType>
void RunHighThroughputTest() {
  BufferType buffer;
  constexpr int NUM_ITEMS = 100000;

  std::thread producer([&buffer]() {
    for (int i = 0; i < NUM_ITEMS; ++i) {
      while (!buffer.try_push(i)) {
      }
    }
  });

  int lastPopped = -1;
  std::thread consumer([&buffer, &lastPopped]() {
    for (int i = 0; i < NUM_ITEMS; ++i) {
      int item;
      while (!buffer.try_pop(item)) {
      }
      // Verify ordering
      EXPECT_EQ(item, lastPopped + 1);
      lastPopped = item;
    }
  });

  producer.join();
  consumer.join();
  EXPECT_EQ(lastPopped, NUM_ITEMS - 1);
}

TEST(StressTest, SimpleCircularBufferHighThroughput) {
  RunHighThroughputTest<SimpleCircularBuffer<int, 1024>>();
}

TEST(StressTest, CircularBufferHighThroughput) {
  RunHighThroughputTest<CircularBuffer<int, 1024>>();
}

TEST(StressTest, AtomicCircularBufferHighThroughput) {
  RunHighThroughputTest<AtomicCircularBuffer<int, 1024>>();
}

TEST(StressTest, LockFreeCircularBufferHighThroughput) {
  RunHighThroughputTest<LockFreeCircularBuffer<int, 1024>>();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
