#include <atomic>
#include <chrono>
#include <numeric>
#include <thread>
#include <vector>

#include <benchmark/benchmark.h>

#include "AtomicCircularBuffer.h"
#include "CircularBuffer.h"
#include "LockFreeCircularBuffer.h"
#include "SimpleCircularBuffer.h"

// Producer-consumer throughput benchmark
template <typename BufferType>
static void BM_ProducerConsumer(benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    const size_t iter = state.range(0);
    BufferType buffer;
    std::atomic<bool> flag(false);
    long sum = 0;

    state.ResumeTiming();

    std::thread producer([&] {
      while (!flag) {
        std::this_thread::yield();
      }

      size_t i = 0;
      while (i < iter) {
        if (buffer.try_push(i)) {
          i++;
        }
      }
    });

    flag = true;
    for (size_t i = 0; i < iter; ++i) {
      while (!buffer.front()) {
        std::this_thread::yield();
      }
      size_t value;
      buffer.try_pop(value);
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
  BufferType buffer;

  for (auto _ : state) {
    // Fill the buffer
    for (size_t i = 0; i < capacity; ++i) {
      benchmark::DoNotOptimize(buffer.try_push(i));
    }

    // Empty the buffer
    for (size_t i = 0; i < capacity; ++i) {
      size_t value;
      buffer.try_pop(value);
      benchmark::DoNotOptimize(value);
    }
  }

  state.SetItemsProcessed(state.iterations() * capacity * 2);
}

// Alternating push/pop pattern
template <typename BufferType>
static void BM_Alternating_PushPop(benchmark::State& state) {
  const size_t operations = state.range(0);
  BufferType buffer;

  for (auto _ : state) {
    for (size_t i = 0; i < operations; ++i) {
      benchmark::DoNotOptimize(buffer.try_push(i));
      size_t value;
      buffer.try_pop(value);
      benchmark::DoNotOptimize(value);
    }
  }

  state.SetItemsProcessed(state.iterations() * operations * 2);
}

// Memory contention test with varying queue sizes
template <typename BufferType>
static void BM_MemoryContention(benchmark::State& state) {
  const size_t iterations = 10000;

  for (auto _ : state) {
    state.PauseTiming();
    BufferType buffer;
    std::atomic<bool> start_flag(false);
    std::atomic<size_t> produced(0);
    std::atomic<size_t> consumed(0);

    state.ResumeTiming();

    std::thread producer([&] {
      while (!start_flag)
        std::this_thread::yield();

      for (size_t i = 0; i < iterations; ++i) {
        while (!buffer.try_push(i)) {
          std::this_thread::yield();
        }
        produced++;
      }
    });

    start_flag = true;
    while (consumed < iterations) {
      size_t value;
      if (buffer.try_pop(value)) {
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

  for (auto _ : state) {
    state.PauseTiming();
    BufferType buffer;
    std::atomic<bool> start_flag(false);

    state.ResumeTiming();

    std::thread producer([&] {
      while (!start_flag)
        std::this_thread::yield();

      // Send bursts of data
      for (size_t burst = 0; burst < 100; ++burst) {
        for (size_t i = 0; i < burst_size; ++i) {
          while (!buffer.try_push(burst * burst_size + i)) {
            std::this_thread::yield();
          }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(10));
      }
    });

    start_flag = true;
    size_t total_consumed = 0;

    while (total_consumed < 100 * burst_size) {
      size_t value;
      if (buffer.try_pop(value)) {
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
    BufferType buffer;
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
        while (!buffer.try_push(i)) {
          std::this_thread::yield();
        }
      }
      producer_done = true;
    });

    start_flag = true;

    while (received_values.size() < operations) {
      size_t value;
      if (buffer.try_pop(value)) {
        received_values.push_back(value);
      } else if (producer_done && buffer.empty()) {
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

// Register benchmarks for all buffer types

// SimpleCircularBuffer benchmarks
BENCHMARK_TEMPLATE(BM_ProducerConsumer, SimpleCircularBuffer<size_t, 1024>)
    ->Range(1 << 16, 1 << 20)
    ->RangeMultiplier(4)
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_SingleThreaded_PushPop,
                   SimpleCircularBuffer<size_t, 1024>)
    ->Range(100, 1000)
    ->RangeMultiplier(10);

BENCHMARK_TEMPLATE(BM_Alternating_PushPop, SimpleCircularBuffer<size_t, 1024>)
    ->Range(1000, 100000)
    ->RangeMultiplier(10);

BENCHMARK_TEMPLATE(BM_MemoryContention, SimpleCircularBuffer<size_t, 1024>)
    ->UseRealTime();

// CircularBuffer benchmarks
BENCHMARK_TEMPLATE(BM_ProducerConsumer, CircularBuffer<size_t, 1024>)
    ->Range(1 << 16, 1 << 20)
    ->RangeMultiplier(4)
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_SingleThreaded_PushPop, CircularBuffer<size_t, 1024>)
    ->Range(100, 1000)
    ->RangeMultiplier(10);

BENCHMARK_TEMPLATE(BM_Alternating_PushPop, CircularBuffer<size_t, 1024>)
    ->Range(1000, 100000)
    ->RangeMultiplier(10);

BENCHMARK_TEMPLATE(BM_MemoryContention, CircularBuffer<size_t, 1024>)
    ->UseRealTime();

// AtomicCircularBuffer benchmarks
BENCHMARK_TEMPLATE(BM_ProducerConsumer, AtomicCircularBuffer<size_t, 1024>)
    ->Range(1 << 16, 1 << 20)
    ->RangeMultiplier(4)
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_SingleThreaded_PushPop,
                   AtomicCircularBuffer<size_t, 1024>)
    ->Range(100, 1000)
    ->RangeMultiplier(10);

BENCHMARK_TEMPLATE(BM_Alternating_PushPop, AtomicCircularBuffer<size_t, 1024>)
    ->Range(1000, 100000)
    ->RangeMultiplier(10);

BENCHMARK_TEMPLATE(BM_MemoryContention, AtomicCircularBuffer<size_t, 1024>)
    ->UseRealTime();

// LockFreeCircularBuffer benchmarks
BENCHMARK_TEMPLATE(BM_ProducerConsumer, LockFreeCircularBuffer<size_t, 1024>)
    ->Range(1 << 16, 1 << 20)
    ->RangeMultiplier(4)
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_SingleThreaded_PushPop,
                   LockFreeCircularBuffer<size_t, 1024>)
    ->Range(100, 1000)
    ->RangeMultiplier(10);

BENCHMARK_TEMPLATE(BM_Alternating_PushPop, LockFreeCircularBuffer<size_t, 1024>)
    ->Range(1000, 100000)
    ->RangeMultiplier(10);

BENCHMARK_TEMPLATE(BM_MemoryContention, LockFreeCircularBuffer<size_t, 1024>)
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_BurstTraffic, LockFreeCircularBuffer<size_t, 1024>)
    ->Range(10, 100)
    ->RangeMultiplier(10)
    ->UseRealTime();

BENCHMARK_TEMPLATE(BM_CorrectnessValidation,
                   LockFreeCircularBuffer<size_t, 1024>)
    ->Arg(1000)
    ->Arg(10000)
    ->UseRealTime();

BENCHMARK_MAIN();
