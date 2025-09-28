#include <gtest/gtest.h>
#include <cstring>
#include <memory>
#include "allocator.h"

class AllocatorTest : public ::testing::Test {
 protected:
  void SetUp() override { mallocator = std::make_unique<allocator>(); }

  std::unique_ptr<allocator> mallocator;
};

TEST_F(AllocatorTest, BasicAllocation) {
  void* ptr = mallocator->malloc(100);
  ASSERT_NE(ptr, nullptr);
  // Write to the memory to ensure it's valid
  memset(ptr, 0xAA, 100);
  mallocator->free(ptr);
}

TEST_F(AllocatorTest, ZeroAllocation) {
  void* ptr = mallocator->malloc(0);
  EXPECT_EQ(ptr, nullptr);
}

TEST_F(AllocatorTest, NullFree) {
  // Should not crash
  EXPECT_NO_THROW(mallocator->free(nullptr));
}

TEST_F(AllocatorTest, MultipleAllocations) {
  const int num_allocs = 10;
  void* ptrs[num_allocs];

  // Allocate multiple blocks
  for (int i = 0; i < num_allocs; i++) {
    ptrs[i] = mallocator->malloc(50 + i * 10);
    ASSERT_NE(ptrs[i], nullptr);

    // Write pattern to each block
    memset(ptrs[i], i, 50 + i * 10);
  }

  // Verify patterns
  for (int i = 0; i < num_allocs; i++) {
    unsigned char* data = static_cast<unsigned char*>(ptrs[i]);
    for (int j = 0; j < 50 + i * 10; j++) {
      EXPECT_EQ(data[j], i);
    }
  }

  // Free all blocks
  for (int i = 0; i < num_allocs; i++) {
    mallocator->free(ptrs[i]);
  }
}

TEST_F(AllocatorTest, DifferentBlockSizes) {
  size_t sizes[] = {1, 8, 16, 32, 64, 128, 256, 512, 1024, 4096, 8192};
  int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

  for (int i = 0; i < num_sizes; i++) {
    void* ptr = mallocator->malloc(sizes[i]);
    ASSERT_NE(ptr, nullptr) << "Failed to allocate " << sizes[i] << " bytes";

    // Write and verify pattern
    memset(ptr, 0x55, sizes[i]);
    unsigned char* data = static_cast<unsigned char*>(ptr);
    for (size_t j = 0; j < sizes[i]; j++) {
      EXPECT_EQ(data[j], 0x55);
    }

    mallocator->free(ptr);
  }
}

TEST_F(AllocatorTest, FragmentationAndReuse) {
  // Allocate alternating pattern
  void* ptr1 = mallocator->malloc(100);
  void* ptr2 = mallocator->malloc(200);
  void* ptr3 = mallocator->malloc(100);
  void* ptr4 = mallocator->malloc(200);

  ASSERT_NE(ptr1, nullptr);
  ASSERT_NE(ptr2, nullptr);
  ASSERT_NE(ptr3, nullptr);
  ASSERT_NE(ptr4, nullptr);

  // Free alternating blocks
  mallocator->free(ptr1);
  mallocator->free(ptr3);

  // Allocate blocks that should reuse freed space
  void* ptr5 = mallocator->malloc(50);  // Should fit in ptr1's space
  void* ptr6 = mallocator->malloc(80);  // Should fit in ptr3's space

  EXPECT_NE(ptr5, nullptr);
  EXPECT_NE(ptr6, nullptr);

  // Clean up
  mallocator->free(ptr2);
  mallocator->free(ptr4);
  mallocator->free(ptr5);
  mallocator->free(ptr6);
}

TEST_F(AllocatorTest, Calloc) {
  int* arr = static_cast<int*>(mallocator->calloc(10, sizeof(int)));
  ASSERT_NE(arr, nullptr);

  // Verify all elements are zero
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(arr[i], 0);
  }

  // Write some data
  for (int i = 0; i < 10; i++) {
    arr[i] = i * 2;
  }

  // Verify data
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(arr[i], i * 2);
  }

  mallocator->free(arr);
}

TEST_F(AllocatorTest, Realloc) {
  // Test realloc with nullptr (should act like malloc)
  void* ptr = mallocator->realloc(nullptr, 100);
  ASSERT_NE(ptr, nullptr);
  memset(ptr, 0xBB, 100);

  // Test expanding
  ptr = mallocator->realloc(ptr, 200);
  ASSERT_NE(ptr, nullptr);

  // Verify original data is preserved
  unsigned char* data = static_cast<unsigned char*>(ptr);
  for (int i = 0; i < 100; i++) {
    EXPECT_EQ(data[i], 0xBB);
  }

  // Test shrinking
  ptr = mallocator->realloc(ptr, 50);
  EXPECT_NE(ptr, nullptr);

  mallocator->free(ptr);
}

TEST_F(AllocatorTest, LargeAllocation) {
  size_t large_size = 1024 * 1024;  // 1MB
  void* ptr = mallocator->malloc(large_size);
  ASSERT_NE(ptr, nullptr);

  // Write to first and last bytes
  unsigned char* data = static_cast<unsigned char*>(ptr);
  data[0] = 0xCC;
  data[large_size - 1] = 0xDD;

  // Verify
  EXPECT_EQ(data[0], 0xCC);
  EXPECT_EQ(data[large_size - 1], 0xDD);

  mallocator->free(ptr);
}

TEST_F(AllocatorTest, StressTest) {
  const int num_allocs = 1000;
  void* ptrs[num_allocs];

  // Allocate many small blocks
  for (int i = 0; i < num_allocs; i++) {
    ptrs[i] = mallocator->malloc(i % 100 + 1);
    ASSERT_NE(ptrs[i], nullptr);
  }

  // Free every other block
  for (int i = 0; i < num_allocs; i += 2) {
    mallocator->free(ptrs[i]);
    ptrs[i] = nullptr;
  }

  // Allocate new blocks in freed spaces
  for (int i = 0; i < num_allocs; i += 2) {
    ptrs[i] = mallocator->malloc(i % 50 + 1);
    ASSERT_NE(ptrs[i], nullptr);
  }

  // Free all remaining blocks
  for (int i = 0; i < num_allocs; i++) {
    if (ptrs[i]) {
      mallocator->free(ptrs[i]);
    }
  }
}

// Test multiple allocator instances
TEST(AllocatorMultipleInstancesTest, IndependentAllocators) {
  allocator alloc1;
  allocator alloc2;

  void* ptr1 = alloc1.malloc(100);
  void* ptr2 = alloc2.malloc(100);

  EXPECT_NE(ptr1, nullptr);
  EXPECT_NE(ptr2, nullptr);
  EXPECT_NE(ptr1, ptr2);  // Should be different memory regions

  alloc1.free(ptr1);
  alloc2.free(ptr2);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
