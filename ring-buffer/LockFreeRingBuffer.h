
#pragma once

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

// Lock-free single producer single consumer queue
template <class T>
struct LockFreeRingBuffer {
 private:
#ifdef __cpp_lib_hardware_interference_size
  static constexpr size_t kCacheLineSize =
      std::hardware_destructive_interference_size;
#else
  static constexpr size_t kCacheLineSize = 64;
#endif
  using AtomicIndex = std::atomic<size_t>;

  // Layout:
  // Cache Line 0: [records_ + size_ + padding...]
  //   - records_ (8 bytes) + size_ (4 bytes) = 12  bytes total
  //   - This is much less than a cache line (64 bytes)
  // Cache Line 1: [writeIndex_ + pad_producer_...]
  //   - WriteIndex_ is aligned at the beginning of a cache line
  //   - We add padding so that it doesn't share a cache line with readIndex_
  // Cache Line 2: [readIndex_ + pad_consumer_...]

  T* const records_;
  const uint32_t size_;

  // Producer-only data (hot for producer thread)
  alignas(kCacheLineSize) AtomicIndex writeIndex_;
  char pad_producer_[kCacheLineSize - sizeof(AtomicIndex)];

  // Consumer-only data (hot for consumer thread)
  alignas(kCacheLineSize) AtomicIndex readIndex_;
  char pad_consumer_[kCacheLineSize - sizeof(AtomicIndex)];

 public:
  typedef T value_type;

  // Avoid copying
  LockFreeRingBuffer(const LockFreeRingBuffer&) = delete;
  LockFreeRingBuffer& operator=(const LockFreeRingBuffer&) = delete;

  // The number of usable slots in the queue at any given time
  // is actually (size-1), so if you start with an empty queue,
  // isFull() will return true after size-1 insertions.
  explicit LockFreeRingBuffer(uint32_t size)
      : records_(static_cast<T*>(std::malloc(sizeof(T) * size))),
        size_(size),
        writeIndex_(0),
        readIndex_(0) {
    assert(size >= 2);
    if (!records_) {
      throw std::bad_alloc();
    }
  }

  ~LockFreeRingBuffer() {
    // Only one thread can be doing this, no synchronization needed
    if (!std::is_trivially_destructible<T>::value) {
      size_t readIndex = readIndex_;
      size_t endIndex = writeIndex_;
      while (readIndex != endIndex) {
        records_[readIndex].~T();
        if (++readIndex == size_) {
          readIndex = 0;
        }
      }
    }

    std::free(records_);
  }

  bool empty() const {
    return readIndex_.load(std::memory_order_acquire) ==
           writeIndex_.load(std::memory_order_acquire);
  }

  bool full() const {
    auto nextRecord = writeIndex_.load(std::memory_order_acquire) + 1;
    if (nextRecord == size_) {
      nextRecord = 0;
    }
    if (nextRecord != readIndex_.load(std::memory_order_acquire)) {
      return false;
    }

    return true;
  }

  // * If called by consumer, then true size may be more (because producer may
  //   be adding items concurrently).
  // * If called by producer, then true size may be less (because consumer may
  //   be removing items concurrently).
  // * It is undefined to call this from any other thread.
  size_t sizeEstimate() const {
    int ret = writeIndex_.load(std::memory_order_acquire) -
              readIndex_.load(std::memory_order_acquire);
    if (ret < 0) {
      ret += size_;
    }
    return ret;
  }

  // Maximum number of items in the queue.
  size_t capacity() const { return size_ - 1; }

  template <class... Args>
  bool push(Args&&... recordArgs) {
    const auto currentWrite = writeIndex_.load(std::memory_order_relaxed);
    auto nextRecord = currentWrite + 1;
    if (nextRecord == size_) {
      nextRecord = 0;
    }

    if (nextRecord != readIndex_.load(std::memory_order_acquire)) {
      new (&records_[currentWrite]) T(std::forward<Args>(recordArgs)...);
      writeIndex_.store(nextRecord, std::memory_order_release);
      return true;
    }

    // The queue is full
    return false;
  }

  // The queue must not be empty
  bool pop(T& record) {
    if (empty()) {
      return false;
    }

    const auto currentRead = readIndex_.load(std::memory_order_relaxed);
    auto nextRecord = currentRead + 1;
    if (nextRecord == size_) {
      nextRecord = 0;
    }
    record = std::move(records_[currentRead]);
    records_[currentRead].~T();
    readIndex_.store(nextRecord, std::memory_order_release);
    return true;
  }

  // Returns a pointer to the value at the front of the queue (for use in-place)
  T* front() {
    auto const currentRead = readIndex_.load(std::memory_order_relaxed);
    if (currentRead == writeIndex_.load(std::memory_order_acquire)) {
      // The queue is empty
      return nullptr;
    }
    return &records_[currentRead];
  }
};
