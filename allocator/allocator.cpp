#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <limits>

#include "allocator.h"

block_meta* allocator::request_space(size_t size) {
  block_meta* block;
  block = static_cast<block_meta*>(mmap(nullptr, size + META_SIZE,
                                        PROT_READ | PROT_WRITE,
                                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  if (block == MAP_FAILED) {
    return nullptr;  // mmap failed
  }

  block->size = size;
  block->free = false;

  return block;
}

block_meta* allocator::get_block_ptr(void* ptr) {
  return static_cast<block_meta*>(ptr) - 1;
}

// Allocator class method implementations

allocator::allocator() = default;

allocator::~allocator() {
  // Free all allocated blocks
  for (auto* block : block_list_) {
    munmap(block, block->size + META_SIZE);
  }
}

void* allocator::malloc(size_t size) {
  if (size <= 0) {
    return nullptr;
  }

  // Try to find a free block with sufficient size
  auto it = std::find_if(block_list_.begin(), block_list_.end(),
                         [size](const block_meta* block) {
                           return block->free && block->size >= size;
                         });

  block_meta* block;
  if (it != block_list_.end()) {
    block = *it;
    block->free = false;
  } else {  // No suitable free block found, request new space
    block = request_space(size);
    if (!block) {
      return nullptr;
    }
    block_list_.push_back(block);
  }

  return (block + 1);
}

void allocator::free(void* ptr) {
  if (!ptr) {
    return;
  }

  block_meta* block_ptr = get_block_ptr(ptr);
  assert(block_ptr->free == false);
  block_ptr->free = true;
}

void* allocator::realloc(void* ptr, size_t size) {
  if (!ptr) {
    // nullptr, realloc should act like malloc
    return this->malloc(size);
  }

  block_meta* block_ptr = get_block_ptr(ptr);
  if (block_ptr->size >= size) {
    return ptr;
  }

  // Malloc new space, free old space, then copy data to new space
  void* new_ptr = this->malloc(size);
  if (!new_ptr) {
    return nullptr;
  }
  memcpy(new_ptr, ptr, block_ptr->size);
  this->free(ptr);

  return new_ptr;
}

void* allocator::calloc(size_t nmemb, size_t size) {
  if (size == 0) {
    return nullptr;
  }

  if (nmemb > std::numeric_limits<size_t>::max() / size) {
    return nullptr;  // Overflow would occur
  }

  const size_t total_size = nmemb * size;
  void* ptr = this->malloc(total_size);
  if (ptr) {
    memset(ptr, 0, total_size);
  }

  return ptr;
}
