#pragma once

#include <array>
#include <cstddef>
#include <mutex>
#include <optional>

// Single-producer, single-consumer circular buffer.
//
// Synchronization: Mutex protects all shared data.
// Storage: std::array.
//
// Constraints:
//   - Consumer cannot read from an empty buffer
//   - Producer cannot add to a full buffer
template <typename T, size_t Capacity>
class SimpleCircularBuffer {
  static_assert(Capacity >= 2, "Capacity must be at least 2");

 private:
  static constexpr size_t kSize = Capacity + 1;

  // Fixed-size storage - elements are default-constructed
  std::array<T, kSize> buffer_;

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
  SimpleCircularBuffer() = default;

  // Disable copy and move
  SimpleCircularBuffer(const SimpleCircularBuffer&) = delete;
  SimpleCircularBuffer& operator=(const SimpleCircularBuffer&) = delete;

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
  bool try_push(const T& item) {
    std::lock_guard<std::mutex> lock(mutex_);

    const size_t nextWrite = nextIndex(writeIndex_);
    // Constraint: Producer cannot add to a full buffer
    if (nextWrite == readIndex_) {
      return false;
    }

    buffer_[writeIndex_] = item;
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

    T item = buffer_[readIndex_];
    readIndex_ = nextIndex(readIndex_);

    return item;
  }
};
