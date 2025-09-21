# Etudes

A collection of implementations exploring data structures, algorithms, and concurrent programming. Each implementation focuses on exercising particular programming skills and understanding real-world performance challenges.

## Table of Contents

| Implementation | Language | Category | Description |
|:--|:--|:--|:--|
| [**Ring Buffer**](ring-buffer/) | C++ | Data Structures, Concurrency | Single-threaded and lock-free multi-threaded ring buffer implementations with comprehensive benchmarks |
| [**Concurrent Counters**](concurrent-counters/) | C++ | Concurrency | Exact and approximate counter implementations comparing contention costs in multi-threaded scenarios |
| [**Thread Pool**](threadpool/) | C++ | Concurrency | Work-stealing thread pool implementation for efficient task distribution |
| [**Mutex Implementation**](mutex/) | C++ | Concurrency | Custom mutex implementations using futex-based and portable atomic primitives |
| [**std::rotate**](std-rotate/) | C++ | Standard Library | Performance analysis and implementation study of the C++ standard library's rotate algorithm |
| [**Thread Creation**](thread-creation/) | C++ | Performance | Benchmarking the overhead and performance characteristics of thread creation |

## Building and Testing

Each subdirectory contains its own `Makefile` with standard targets:

```bash
# Run tests for a specific implementation
cd ring-buffer && make test-all

# Run benchmarks
cd ring-buffer && make bench

# Clean build artifacts
make clean
```

## Philosophy

Like Peter Norvig's pytudes, this collection treats programming as a craft requiring deliberate practice. Each implementation serves as:

- **An exploration** of fundamental computer science concepts
- **A performance laboratory** for understanding real-world trade-offs
- **A reference implementation** demonstrating best practices
- **A learning tool** for mastering concurrent programming patterns

The focus is on understanding *why* certain approaches work, not just *how* they work.
