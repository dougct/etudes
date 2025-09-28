#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <list>

// Stores meta information about each block of memory
struct block_meta {
  size_t size{0};
  bool free{true};
};

#define META_SIZE sizeof(struct block_meta)

class allocator {
private:
  std::list<block_meta*> block_list_;

  block_meta* request_space(size_t size);
  block_meta* get_block_ptr(void* ptr);

public:
  allocator();
  ~allocator();
  void* malloc(size_t size);
  void free(void* ptr);
  void* realloc(void* ptr, size_t size);
  void* calloc(size_t nelem, size_t elsize);
};

#endif
