# Producer-Consumer Ring Buffers

This project implements several variations of single-producer, single-consumer (SPSC) ring buffers in C++17, progressing from a simple mutex-based implementation to a high-performance lock-free version.

## Implementations

| Class | Synchronization | Description |
|-------|----------------|-------------|
| `SimpleCircularBuffer` | Mutex | Uses `std::array` with simple copy assignment. Good for built-in types. |
| `CircularBuffer` | Mutex | Uses `malloc` with placement new. Supports move semantics. |
| `AtomicCircularBuffer` | `seq_cst` atomics | Lock-free using sequential consistency (default memory order). |
| `LockFreeCircularBuffer` | `acquire/release` atomics | Optimized lock-free with weaker memory ordering and cache line padding. |

## API

All implementations provide the same interface:

```cpp
bool try_push(const T& item);  // Returns false if full
bool try_pop(T& out);          // Returns false if empty

// Query operations
bool empty() const;
bool full() const;
uint32_t size() const;
static constexpr uint32_t capacity();
T* front();                    // Peek at front element
void popFront();               // Consume front element (after checking with front())
```

### Blocking Wrappers

The buffer only provides non-blocking operations. If you need blocking behavior, implement it at the call site:

```cpp
// Blocking push - spins until space is available
template <typename Buffer, typename T>
void push(Buffer& buffer, const T& item) {
    while (!buffer.try_push(item)) {
        // Spin, yield, or wait on condition variable
    }
}

// Blocking pop - spins until data is available
template <typename Buffer, typename T>
T pop(Buffer& buffer) {
    T item;
    while (!buffer.try_pop(item)) {
        // Spin, yield, or wait on condition variable
    }
    return item;
}
```

## Building

### Prerequisites

- C++17 compatible compiler (g++ or clang++)
- GoogleTest (for tests)
- Google Benchmark (for benchmarks)

On macOS with Homebrew:
```bash
brew install googletest google-benchmark
```

### Commands

```bash
# Build and run tests
make test

# Build and run benchmarks
make bench

# Clean build artifacts
make clean
```

## Usage Example

```cpp
#include "LockFreeCircularBuffer.h"
#include <thread>

int main() {
    LockFreeCircularBuffer<int, 1024> buffer;

    std::thread producer([&buffer]() {
        for (int i = 0; i < 1000; ++i) {
            while (!buffer.try_push(i)) {
                // Spin until space available
            }
        }
    });

    std::thread consumer([&buffer]() {
        for (int i = 0; i < 1000; ++i) {
            int value;
            while (!buffer.try_pop(value)) {
                // Spin until data available
            }
            // process value...
        }
    });

    producer.join();
    consumer.join();
}
```

## Performance

Benchmarks run on Apple M1 (8 cores), compiled with `-O2`:

### Producer-Consumer Throughput (1M items)

| Implementation | Time | Relative |
|---------------|------|----------|
| SimpleCircularBuffer | 67.7 ms | 3.3x slower |
| CircularBuffer | 99.0 ms | 4.8x slower |
| AtomicCircularBuffer | 44.6 ms | 2.2x slower |
| LockFreeCircularBuffer | **20.5 ms** | **1.0x (fastest)** |

## Design Notes

Please check the accompanying [blog post](https://dougct.github.io/blog/producer-consumer/) for a deep-dive on the design and implementation of the circular buffers in this repository.
