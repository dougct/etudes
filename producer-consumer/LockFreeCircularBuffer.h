#pragma once

#include <atomic>
#include <cstddef>
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
  // Apple Silicon and some ARM processors use 128-byte cache lines.
#if defined(__aarch64__) && defined(__APPLE__)
  static constexpr size_t kCacheLineSize = 128;
#elif defined(__cpp_lib_hardware_interference_size)
  static constexpr size_t kCacheLineSize =
      std::hardware_destructive_interference_size;
#else
  static constexpr size_t kCacheLineSize = 64;
#endif

 private:
  static constexpr size_t kSize = Capacity + 1;

  // Prevents false sharing with members of a containing struct that precede
  // this object in memory.
  char padLeading_[kCacheLineSize];

  // Raw memory storage - elements constructed via placement new
  T* const buffer_;

  // readIndex_ aligned to cache line. Padding after ensures writeIndex_ (which
  // is written by a different thread) doesn't share the same cache line.
  alignas(kCacheLineSize) std::atomic<size_t> readIndex_{0};
  char padReadIndex_[kCacheLineSize - sizeof(std::atomic<size_t>)];

  // writeIndex_ aligned to cache line. Padding after prevents false sharing
  // with members of a containing struct that follow this object in memory.
  alignas(kCacheLineSize) std::atomic<size_t> writeIndex_{0};
  char padWriteIndex_[kCacheLineSize - sizeof(std::atomic<size_t>)];

  // Compute next index with wraparound
  static constexpr size_t nextIndex(size_t index) {
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

  ~LockFreeCircularBuffer() {
    // Destruct any remaining elements
    if constexpr (!std::is_trivially_destructible_v<T>) {
      size_t read = readIndex_.load(std::memory_order_relaxed);
      size_t write = writeIndex_.load(std::memory_order_relaxed);
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
  size_t size() const {
    const size_t write = writeIndex_.load(std::memory_order_acquire);
    const size_t read = readIndex_.load(std::memory_order_acquire);

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

  static constexpr size_t capacity() {
    return Capacity;
  }

  // Peek at front element without removing
  T* front() {
    const size_t currentRead = readIndex_.load(std::memory_order_relaxed);

    // Constraint: Consumer cannot read from an empty buffer
    if (currentRead == writeIndex_.load(std::memory_order_acquire)) {
      return nullptr;
    }

    return &buffer_[currentRead];
  }

  // Non-blocking push: try to add item to buffer
  // Returns false if buffer is full
  template <class... Args>
  bool try_push(Args&&... args) {
    const size_t currentWrite = writeIndex_.load(std::memory_order_relaxed);
    const size_t nextWrite = nextIndex(currentWrite);

    // Constraint: Producer cannot add to a full buffer
    if (nextWrite == readIndex_.load(std::memory_order_acquire)) {
      return false;
    }

    new (&buffer_[currentWrite]) T(std::forward<Args>(args)...);
    writeIndex_.store(nextWrite, std::memory_order_release);

    return true;
  }

  // Non-blocking pop: try to remove item from buffer
  // Returns nullopt if buffer is empty
  std::optional<T> try_pop() {
    const size_t currentRead = readIndex_.load(std::memory_order_relaxed);

    // Constraint: Consumer cannot read from an empty buffer
    if (currentRead == writeIndex_.load(std::memory_order_acquire)) {
      return std::nullopt;
    }

    T item = std::move(buffer_[currentRead]);
    buffer_[currentRead].~T();
    readIndex_.store(nextIndex(currentRead), std::memory_order_release);

    return item;
  }
};
