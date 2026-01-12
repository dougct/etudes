#pragma once

#include <cstddef>
#include <cstdlib>
#include <mutex>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

// Single-producer, single-consumer circular buffer.
//
// Synchronization: Mutex protects all shared data.
// Storage: Raw array, elements are added using placement new.
//
// Constraints:
//   - Consumer cannot read from an empty buffer
//   - Producer cannot add to a full buffer
template <typename T, size_t Capacity>
class CircularBuffer {
  static_assert(Capacity >= 2, "Capacity must be at least 2");

 private:
  static constexpr size_t kSize = Capacity + 1;

  // Raw memory storage - elements constructed via placement new
  T* const buffer_;

  // Read index: points to next element to consume
  size_t readIndex_{0};

  // Write index: points to next slot to fill
  size_t writeIndex_{0};

  // Mutex protects the entire buffer (mutable for use in const methods)
  mutable std::mutex mutex_;

  // Compute next index with wraparound
  static constexpr size_t nextIndex(size_t index) {
    return (index + 1) % kSize;
  }

 public:
  explicit CircularBuffer() : buffer_(static_cast<T*>(std::malloc(sizeof(T) * kSize))) {
    if (!buffer_) {
      throw std::bad_alloc();
    }
  }

  // Disable copy and move
  CircularBuffer(const CircularBuffer&) = delete;
  CircularBuffer& operator=(const CircularBuffer&) = delete;

  ~CircularBuffer() {
    // Destruct any remaining elements
    if constexpr (!std::is_trivially_destructible_v<T>) {
      while (readIndex_ != writeIndex_) {
        buffer_[readIndex_].~T();
        readIndex_ = nextIndex(readIndex_);
      }
    }
    std::free(buffer_);
  }

  // Query if empty (approximate)
  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return readIndex_ == writeIndex_;
  }

  // Query if full (approximate)
  bool full() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return nextIndex(writeIndex_) == readIndex_;
  }

  // Query current size (approximate)
  size_t size() const {
    std::lock_guard<std::mutex> lock(mutex_);

    // No wraparound (writeIndex_ >= readIndex_)
    //   [---R###W---]  where # = filled slots
    //   Size = writeIndex_ - readIndex_
    if (writeIndex_ >= readIndex_) {
      return writeIndex_ - readIndex_;
    }

    // Wraparound (writeIndex_ < readIndex_)
    //   [###W---R###]  where # = filled slots
    //   Size = (slots from R to end) + (slots from start to W)
    return kSize - readIndex_ + writeIndex_;
  }

  static constexpr size_t capacity() {
    return Capacity;
  }

  // Peek at front element without removing
  T* front() {
    std::lock_guard<std::mutex> lock(mutex_);

    // Constraint: Consumer cannot read from an empty buffer
    if (readIndex_ == writeIndex_) {
      return nullptr;
    }

    return &buffer_[readIndex_];
  }

  // Non-blocking push: try to add item to buffer
  // Returns false if buffer is full
  template <class... Args>
  bool try_push(Args&&... args) {
    std::lock_guard<std::mutex> lock(mutex_);

    const size_t nextWrite = nextIndex(writeIndex_);
    // Constraint: Producer cannot add to a full buffer
    if (nextWrite == readIndex_) {
      return false;
    }

    new (&buffer_[writeIndex_]) T(std::forward<Args>(args)...);
    writeIndex_ = nextWrite;

    return true;
  }

  // Non-blocking pop: try to remove item from buffer
  // Returns nullopt if buffer is empty
  std::optional<T> try_pop() {
    std::lock_guard<std::mutex> lock(mutex_);

    // Constraint: Consumer cannot read from an empty buffer
    if (readIndex_ == writeIndex_) {
      return std::nullopt;
    }

    T item = std::move(buffer_[readIndex_]);
    buffer_[readIndex_].~T();
    readIndex_ = nextIndex(readIndex_);

    return item;
  }
};
