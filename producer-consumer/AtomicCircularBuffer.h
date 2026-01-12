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
// Synchronization: Atomic operations with sequential consistency (seq_cst).
// Storage: Raw array, elements are added using placement new.
//
// Constraints:
//   - Consumer cannot read from an empty buffer
//   - Producer cannot add to a full buffer
template <typename T, size_t Capacity>
class AtomicCircularBuffer {
  static_assert(Capacity >= 2, "Capacity must be at least 2");

 private:
  static constexpr size_t kSize = Capacity + 1;

  // Raw memory storage - elements constructed via placement new
  T* const buffer_;

  // Atomic indices - each owned by one thread, read by the other
  std::atomic<size_t> readIndex_{0};
  std::atomic<size_t> writeIndex_{0};

  // Compute next index with wraparound
  static constexpr size_t nextIndex(size_t index) {
    return (index + 1) % kSize;
  }

 public:
  explicit AtomicCircularBuffer()
      : buffer_(static_cast<T*>(std::malloc(sizeof(T) * kSize))) {
    if (!buffer_) {
      throw std::bad_alloc();
    }
  }

  // Disable copy and move
  AtomicCircularBuffer(const AtomicCircularBuffer&) = delete;
  AtomicCircularBuffer& operator=(const AtomicCircularBuffer&) = delete;

  ~AtomicCircularBuffer() {
    // Destruct any remaining elements
    if constexpr (!std::is_trivially_destructible_v<T>) {
      size_t read = readIndex_.load();
      size_t write = writeIndex_.load();
      while (read != write) {
        buffer_[read].~T();
        read = nextIndex(read);
      }
    }
    std::free(buffer_);
  }

  // Query if empty (approximate)
  bool empty() const {
    return readIndex_.load() == writeIndex_.load();
  }

  // Query if full (approximate)
  bool full() const {
    return nextIndex(writeIndex_.load()) == readIndex_.load();
  }

  // Query current size (approximate)
  size_t size() const {
    const size_t write = writeIndex_.load();
    const size_t read = readIndex_.load();

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
    const size_t currentRead = readIndex_.load();

    // Constraint: Consumer cannot read from an empty buffer
    if (currentRead == writeIndex_.load()) {
      return nullptr;
    }

    return &buffer_[currentRead];
  }

  // Non-blocking push: try to add item to buffer
  // Returns false if buffer is full
  template <class... Args>
  bool try_push(Args&&... args) {
    const size_t currentWrite = writeIndex_.load();
    const size_t nextWrite = nextIndex(currentWrite);

    // Constraint: Producer cannot add to a full buffer
    if (nextWrite == readIndex_.load()) {
      return false;
    }

    new (&buffer_[currentWrite]) T(std::forward<Args>(args)...);
    writeIndex_.store(nextWrite);

    return true;
  }

  // Non-blocking pop: try to remove item from buffer
  // Returns nullopt if buffer is empty
  std::optional<T> try_pop() {
    const size_t currentRead = readIndex_.load();

    // Constraint: Consumer cannot read from an empty buffer
    if (currentRead == writeIndex_.load()) {
      return std::nullopt;
    }

    T item = std::move(buffer_[currentRead]);
    buffer_[currentRead].~T();
    readIndex_.store(nextIndex(currentRead));

    return item;
  }
};
