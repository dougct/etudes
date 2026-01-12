# Producer-Consumer Ring Buffers

This project implements several variations of single-producer, single-consumer (SPSC) ring buffers in C++17, progressing from a simple mutex-based implementation to a high-performance lock-free version.

## Implementations

| Class | Synchronization | Description |
|-------|----------------|-------------|
| `SimpleCircularBuffer` | Mutex | Uses `std::array` with simple copy assignment. Good for built-in types. |
| `CircularBuffer` | Mutex | Uses `malloc` with placement new. Supports move semantics. |
| `AtomicCircularBuffer` | `seq_cst` atomics | Lock-free using sequential consistency (default memory order). |
| `LockFreeCircularBuffer` | `acquire/release` atomics | Optimized lock-free with weaker memory ordering and cache line padding. |

## Performance

Benchmarks run on Apple M1 (8 cores), compiled with `-O2`:

### Producer-Consumer Throughput (1M items)

| Implementation | Time | Relative |
|---------------|------|----------|
| SimpleCircularBuffer | 66.9 ms | 1.0x |
| CircularBuffer | 72.6 ms | 0.92x |
| AtomicCircularBuffer | 44.6 ms | 1.50x |
| LockFreeCircularBuffer | **20.6 ms** | **3.25x** |

### Single-Threaded Operations

| Implementation | Throughput |
|---------------|------------|
| SimpleCircularBuffer | ~106 M items/s |
| CircularBuffer | ~113 M items/s |
| AtomicCircularBuffer | ~382 M items/s |
| LockFreeCircularBuffer | ~379 M items/s |

### Alternating Push/Pop

| Implementation | Throughput |
|---------------|------------|
| SimpleCircularBuffer | ~107 M items/s |
| CircularBuffer | ~111 M items/s |
| AtomicCircularBuffer | ~681 M items/s |
| LockFreeCircularBuffer | ~673 M items/s |

### Why Are Some Implementations Faster?

**Mutex vs Atomics (3.5x single-threaded speedup)**

The mutex-based implementations (`SimpleCircularBuffer`, `CircularBuffer`) must acquire and release a lock on every operation. Even when uncontended, this involves atomic compare-and-swap operations and memory barriers. The atomic implementations avoid this overhead entirely—a simple atomic load/store is much cheaper than a mutex lock/unlock cycle.

**seq_cst vs acquire/release (similar single-threaded, 2x multi-threaded speedup)**

In single-threaded benchmarks, `AtomicCircularBuffer` (seq_cst) and `LockFreeCircularBuffer` (acquire/release) perform nearly identically. The difference appears under contention:

- **Sequential consistency** (`seq_cst`) enforces a global total order of all atomic operations across all threads. This requires expensive memory barriers that synchronize with all other seq_cst operations system-wide.
- **Acquire/release** only synchronizes between paired operations on the same variable. The producer's `release` store synchronizes with the consumer's `acquire` load, but doesn't need to order with unrelated operations.

**Cache line padding (key to multi-threaded performance)**

`LockFreeCircularBuffer` places `readIndex` and `writeIndex` on separate cache lines. Without this:

1. Producer writes `writeIndex`, invalidating the cache line on all other cores
2. Consumer reads `writeIndex` from memory (cache miss)
3. Consumer writes `readIndex` on the *same* cache line, invalidating it on the producer's core
4. Producer reads `readIndex` from memory (cache miss)

This "ping-pong" effect can dominate performance. With padding, each thread's index stays in its own cache, and only needs to fetch the other index occasionally.

## API

All implementations provide the same interface:

```cpp
bool try_push(const T& item);  // Returns false if full
std::optional<T> try_pop();    // Returns nullopt if empty

// Query operations
bool empty() const;
bool full() const;
size_t size() const;
static constexpr size_t capacity();
T* front();                    // Peek at front element
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
template <typename Buffer>
auto pop(Buffer& buffer) {
    while (true) {
        if (auto item = buffer.try_pop()) {
            return *item;
        }
        // Spin, yield, or wait on condition variable
    }
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
            std::optional<int> value;
            while (!(value = buffer.try_pop())) {
                // Spin until data available
            }
            // process *value...
        }
    });

    producer.join();
    consumer.join();
}
```

## Design Notes

### Why sacrifice one slot?

All implementations use `Capacity + 1` slots internally, sacrificing one slot to distinguish between full and empty states. This avoids the need for a separate counter or flag, which would require additional synchronization.

- **Empty**: `readIndex == writeIndex`
- **Full**: `(writeIndex + 1) % size == readIndex`

### Memory Ordering (LockFreeCircularBuffer)

The lock-free implementation uses careful memory ordering:

- **Producer** reads `writeIndex` with `relaxed`, reads `readIndex` with `acquire`, writes `writeIndex` with `release`
- **Consumer** reads `readIndex` with `relaxed`, reads `writeIndex` with `acquire`, writes `readIndex` with `release`

This ensures that:
1. Data written by the producer is visible to the consumer before the index update
2. The consumer's index update is visible to the producer before reusing the slot

### Cache Line Padding

`LockFreeCircularBuffer` aligns `readIndex` and `writeIndex` to separate cache lines (64 bytes) to prevent false sharing, which would otherwise cause cache invalidation traffic between the producer and consumer cores.

## References

1. [folly's ProducerConsumerQueue](https://github.com/facebook/folly/blob/main/folly/ProducerConsumerQueue.h)
2. [Optimizing a ring buffer for throughput](https://rigtorp.se/ringbuffer/)
3. [Cache coherency primer](https://fgiesen.wordpress.com/2014/07/07/cache-coherency/)
