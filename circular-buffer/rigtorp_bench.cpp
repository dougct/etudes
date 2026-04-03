// Rigtorp-style ring buffer benchmark
// Measures throughput in operations per second for producer-consumer pattern

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <thread>

#include "AtomicCircularBuffer.h"
#include "CircularBuffer.h"
#include "LockFreeCircularBuffer.h"
#include "SimpleCircularBuffer.h"

constexpr size_t kIterations = 100000000;  // 100 million operations
constexpr int kRuns = 3;                   // Number of runs to average

template <typename BufferType>
double bench_single_run() {
  BufferType buffer;

  auto t1 = std::chrono::steady_clock::now();

  std::thread consumer([&] {
    for (size_t i = 0; i < kIterations; ++i) {
      int val;
      while (!buffer.try_pop(val)) {
        // spin
      }
    }
  });

  for (size_t i = 0; i < kIterations; ++i) {
    while (!buffer.try_push(static_cast<int>(i))) {
      // spin
    }
  }

  consumer.join();

  auto t2 = std::chrono::steady_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

  return static_cast<double>(duration) / 1e9;  // Return seconds
}

template <typename BufferType>
void bench(const char* name) {
  double total_seconds = 0.0;

  for (int run = 0; run < kRuns; ++run) {
    total_seconds += bench_single_run<BufferType>();
  }

  double avg_seconds = total_seconds / kRuns;
  double ops_per_sec = kIterations / avg_seconds;
  double ns_per_op = (avg_seconds * 1e9) / kIterations;

  std::cout << "  " << name << ": " << std::fixed << std::setprecision(1)
            << (ops_per_sec / 1e6) << "M ops/sec (" << static_cast<int>(ns_per_op)
            << " ns/op)\n";
}

template <size_t QueueSize>
void run_all_benchmarks() {
  std::cout << "\nQueue size: " << QueueSize << "\n";
  std::cout << std::string(40, '-') << "\n";

  bench<SimpleCircularBuffer<int, QueueSize>>("SimpleCircularBuffer");
  bench<CircularBuffer<int, QueueSize>>("CircularBuffer");
  bench<AtomicCircularBuffer<int, QueueSize>>("AtomicCircularBuffer");
  bench<LockFreeCircularBuffer<int, QueueSize>>("LockFreeCircularBuffer");
}

int main() {
  std::cout << "Ring Buffer Benchmark (rigtorp-style)\n";
  std::cout << "Operations per test: " << kIterations << " (100 million)\n";
  std::cout << "Runs per benchmark: " << kRuns << " (averaged)\n";
  std::cout << "====================================================================\n";

  run_all_benchmarks<1024>();
  run_all_benchmarks<10000>();
  run_all_benchmarks<20000>();
  run_all_benchmarks<100000>();

  return 0;
}
