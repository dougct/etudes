#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

// Single-producer, single-consumer circular buffer.
//
// Synchronization: Lock-free with acquire/release memory ordering and cache
// line padding.
// Storage: Raw array, elements are added using placement new.
//
// Constraints:
//   - Consumer cannot read from an empty buffer
//   - Producer cannot add to a full buffer
template <typename T, size_t Capacity>
class LockFreeCircularBuffer {
  static_assert(Capacity >= 2, "Capacity must be at least 2");

 public:
  // Cache line size for padding to prevent false sharing.
  //
  // We prefer std::hardware_destructive_interference_size when available, as
  // it provides the platform-specific value.
  //
  // Fallback uses 128 bytes (a cache line pair) to be defensive against
  // adjacent cache line prefetching in some architectures.
  //
  // See: folly/lang/Align.h for similar rationale.
#if defined(__cpp_lib_hardware_interference_size)
  static constexpr size_t kCacheLineSize =
      std::hardware_destructive_interference_size;
#elif defined(__aarch64__) || defined(__arm__)
  static constexpr size_t kCacheLineSize = 64;
#else
  static constexpr size_t kCacheLineSize = 128;
#endif

 private:
  static constexpr size_t kSize = Capacity + 1;

  // Prevents false sharing with members of a containing struct that precede
  // this object in memory.
  char padLeading_[kCacheLineSize];

  // Raw memory storage - elements constructed via placement new
  T* const buffer_;

  // readIndex_ aligned to cache line.
  alignas(kCacheLineSize) std::atomic<uint32_t> readIndex_{0};

  // writeIndex_ aligned to cache line. Padding after prevents false sharing
  // with members of a containing struct that follow this object in memory.
  alignas(kCacheLineSize) std::atomic<uint32_t> writeIndex_{0};
  char padWriteIndex_[kCacheLineSize - sizeof(std::atomic<uint32_t>)];

  // Compute next index with wraparound
  static constexpr uint32_t nextIndex(uint32_t index) {
    return (index + 1) % kSize;
  }

 public:
  explicit LockFreeCircularBuffer()
      : buffer_(static_cast<T*>(std::malloc(sizeof(T) * kSize))) {
    if (!buffer_) {
      throw std::bad_alloc();
    }
  }

  // Disable copy and move
  LockFreeCircularBuffer(const LockFreeCircularBuffer&) = delete;
  LockFreeCircularBuffer& operator=(const LockFreeCircularBuffer&) = delete;
  LockFreeCircularBuffer(LockFreeCircularBuffer&&) = delete;
  LockFreeCircularBuffer& operator=(LockFreeCircularBuffer&&) = delete;

  ~LockFreeCircularBuffer() {
    // Destruct any remaining elements
    if constexpr (!std::is_trivially_destructible_v<T>) {
      uint32_t read = readIndex_.load(std::memory_order_relaxed);
      uint32_t write = writeIndex_.load(std::memory_order_relaxed);
      while (read != write) {
        buffer_[read].~T();
        read = nextIndex(read);
      }
    }
    std::free(buffer_);
  }

  // Query if empty (approximate)
  bool empty() const {
    return readIndex_.load(std::memory_order_acquire) ==
           writeIndex_.load(std::memory_order_acquire);
  }

  // Query if full (approximate)
  bool full() const {
    return nextIndex(writeIndex_.load(std::memory_order_acquire)) ==
           readIndex_.load(std::memory_order_acquire);
  }

  // Query current size (approximate)
  uint32_t size() const {
    const uint32_t write = writeIndex_.load(std::memory_order_acquire);
    const uint32_t read = readIndex_.load(std::memory_order_acquire);

    // No wraparound (write >= read)
    //   [---R###W---]  where # = filled slots
    //   Size = write - read
    if (write >= read) {
      return write - read;
    }

    // Wraparound (write < read)
    //   [###W---R###]  where # = filled slots
    //   Size = (slots from R to end) + (slots from start to W)
    return kSize - read + write;
  }

  static constexpr uint32_t capacity() {
    return Capacity;
  }

  // Peek at front element without removing
  T* front() {
    const uint32_t currentRead = readIndex_.load(std::memory_order_relaxed);

    // Constraint: Consumer cannot read from an empty buffer
    if (currentRead == writeIndex_.load(std::memory_order_acquire)) {
      return nullptr;
    }

    return &buffer_[currentRead];
  }

  // Consume front element after peeking with front()
  // Precondition: buffer must not be empty (caller should have checked via front())
  void popFront() {
    const uint32_t currentRead = readIndex_.load(std::memory_order_relaxed);
    buffer_[currentRead].~T();
    readIndex_.store(nextIndex(currentRead), std::memory_order_release);
  }

  // Non-blocking push: try to add item to buffer
  // Returns false if buffer is full
  template <class... Args>
  bool try_push(Args&&... args) {
    const uint32_t currentWrite = writeIndex_.load(std::memory_order_relaxed);
    const uint32_t nextWrite = nextIndex(currentWrite);

    // Constraint: Producer cannot add to a full buffer
    if (nextWrite == readIndex_.load(std::memory_order_acquire)) {
      return false;
    }

    new (&buffer_[currentWrite]) T(std::forward<Args>(args)...);
    writeIndex_.store(nextWrite, std::memory_order_release);

    return true;
  }

  // Non-blocking pop: try to remove item from buffer
  // Returns false if buffer is empty
  bool try_pop(T& out) {
    const uint32_t currentRead = readIndex_.load(std::memory_order_relaxed);

    // Constraint: Consumer cannot read from an empty buffer
    if (currentRead == writeIndex_.load(std::memory_order_acquire)) {
      return false;
    }

    out = std::move(buffer_[currentRead]);
    buffer_[currentRead].~T();
    readIndex_.store(nextIndex(currentRead), std::memory_order_release);

    return true;
  }
};
