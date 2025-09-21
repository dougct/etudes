#include <atomic>
#include <chrono>
#include <numeric>
#include <thread>
#include <vector>

#include <benchmark/benchmark.h>

#include "LockFreeRingBuffer.h"
#include "SingleThreadRingBuffer.h"

template <typename BufferType>
static void BM_RingBuffer(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    const size_t iter = state.range(0);
    BufferType ring(iter / 1000 + 1);
    std::atomic<bool> flag(false);
    long sum = 0;

    state.ResumeTiming();

    std::thread producer([&] {
      while (!flag) {
        std::this_thread::yield();
      }

      size_t i = 0;
      while (i < iter) {
        if (ring.push(i)) {
          i++;
        }
      }
    });

    flag = true;
    for (size_t i = 0; i < iter; ++i) {
      while (!ring.front()) {
        std::this_thread::yield();
      }
      size_t value;
      ring.pop(value);
      sum += value;
    }

    producer.join();
    benchmark::DoNotOptimize(sum);
    benchmark::ClobberMemory();
  }
}

// Single-threaded push/pop performance
template <typename BufferType>
static void BM_SingleThreaded_PushPop(benchmark::State& state) {
  const size_t capacity = state.range(0);
  BufferType ring(capacity + 1);

  for (auto _ : state) {
    // Fill the buffer
    for (size_t i = 0; i < capacity; ++i) {
      benchmark::DoNotOptimize(ring.push(i));
    }

    // Empty the buffer
    size_t value;
    for (size_t i = 0; i < capacity; ++i) {
      benchmark::DoNotOptimize(ring.pop(value));
      benchmark::DoNotOptimize(value);
    }
  }

  state.SetItemsProcessed(state.iterations() * capacity * 2);
}

// Alternating push/pop pattern
template <typename BufferType>
static void BM_Alternating_PushPop(benchmark::State& state) {
  const size_t operations = state.range(0);
  BufferType ring(10);

  for (auto _ : state) {
    for (size_t i = 0; i < operations; ++i) {
      benchmark::DoNotOptimize(ring.push(i));
      size_t value;
      benchmark::DoNotOptimize(ring.pop(value));
      benchmark::DoNotOptimize(value);
    }
  }

  state.SetItemsProcessed(state.iterations() * operations * 2);
}

// Memory contention test with varying queue sizes
template <typename BufferType>
static void BM_MemoryContention(benchmark::State& state) {
  const size_t queue_size = state.range(0);
  const size_t iterations = 10000;

  for (auto _ : state) {
    state.PauseTiming();
    BufferType ring(queue_size);
    std::atomic<bool> start_flag(false);
    std::atomic<size_t> produced(0);
    std::atomic<size_t> consumed(0);

    state.ResumeTiming();

    std::thread producer([&] {
      while (!start_flag)
        std::this_thread::yield();

      for (size_t i = 0; i < iterations; ++i) {
        while (!ring.push(i)) {
          std::this_thread::yield();
        }
        produced++;
      }
    });

    start_flag = true;
    size_t value;
    while (consumed < iterations) {
      if (ring.pop(value)) {
        consumed++;
        benchmark::DoNotOptimize(value);
      } else {
        std::this_thread::yield();
      }
    }

    producer.join();
    benchmark::DoNotOptimize(produced.load());
    benchmark::DoNotOptimize(consumed.load());
  }

  state.SetItemsProcessed(state.iterations() * iterations);
}

// Burst traffic pattern
template <typename BufferType>
static void BM_BurstTraffic(benchmark::State& state) {
  const size_t burst_size = state.range(0);
  const size_t capacity = burst_size * 2;

  for (auto _ : state) {
    state.PauseTiming();
    BufferType ring(capacity + 1);
    std::atomic<bool> start_flag(false);

    state.ResumeTiming();

    std::thread producer([&] {
      while (!start_flag)
        std::this_thread::yield();

      // Send bursts of data
      for (size_t burst = 0; burst < 100; ++burst) {
        for (size_t i = 0; i < burst_size; ++i) {
          while (!ring.push(burst * burst_size + i)) {
            std::this_thread::yield();
          }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(10));
      }
    });

    start_flag = true;
    size_t total_consumed = 0;
    size_t value;

    while (total_consumed < 100 * burst_size) {
      if (ring.pop(value)) {
        total_consumed++;
        benchmark::DoNotOptimize(value);
      } else {
        std::this_thread::yield();
      }
    }

    producer.join();
    benchmark::DoNotOptimize(total_consumed);
  }
}

// Correctness validation benchmark
template <typename BufferType>
static void BM_CorrectnessValidation(benchmark::State& state) {
  const size_t operations = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();
    BufferType ring(1000);
    std::atomic<bool> start_flag(false);
    std::atomic<bool> producer_done(false);
    std::vector<size_t> expected_values(operations);
    std::vector<size_t> received_values;
    received_values.reserve(operations);

    // Generate expected sequence
    std::iota(expected_values.begin(), expected_values.end(), 0);

    state.ResumeTiming();

    std::thread producer([&] {
      while (!start_flag)
        std::this_thread::yield();

      for (size_t i = 0; i < operations; ++i) {
        while (!ring.push(i)) {
          std::this_thread::yield();
        }
      }
      producer_done = true;
    });

    start_flag = true;
    size_t value;

    while (received_values.size() < operations) {
      if (ring.pop(value)) {
        received_values.push_back(value);
      } else if (producer_done && ring.empty()) {
        break;
      } else {
        std::this_thread::yield();
      }
    }

    producer.join();

    // Validate correctness
    bool correct = (received_values == expected_values);
    benchmark::DoNotOptimize(correct);
    if (!correct) {
      state.SkipWithError("Correctness validation failed");
    }
  }

  state.SetItemsProcessed(state.iterations() * operations);
}

// Register benchmarks
BENCHMARK_TEMPLATE(BM_RingBuffer, SingleThreadRingBuffer<size_t>)
    ->Range(1 << 16, 1 << 24)
    ->RangeMultiplier(2)
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_RingBuffer, LockFreeRingBuffer<size_t>)
    ->Range(1 << 16, 1 << 24)
    ->RangeMultiplier(2)
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_SingleThreaded_PushPop, LockFreeRingBuffer<size_t>)
    ->Range(100, 10000)
    ->RangeMultiplier(10);

BENCHMARK_TEMPLATE(BM_Alternating_PushPop, LockFreeRingBuffer<size_t>)
    ->Range(1000, 100000)
    ->RangeMultiplier(10);

BENCHMARK_TEMPLATE(BM_MemoryContention, LockFreeRingBuffer<size_t>)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1000)
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_BurstTraffic, LockFreeRingBuffer<size_t>)
    ->Range(10, 1000)
    ->RangeMultiplier(10)
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_CorrectnessValidation, LockFreeRingBuffer<size_t>)
    ->Arg(1000)
    ->Arg(10000)
    ->UseRealTime();

BENCHMARK_MAIN();
