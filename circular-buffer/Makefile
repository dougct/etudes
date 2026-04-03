CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -O2

# Library paths - these may vary by OS/installation method.
# To find paths on your system:
#   macOS (Homebrew): brew --prefix googletest && brew --prefix google-benchmark
#   Linux (apt):      pkg-config --cflags --libs gtest benchmark
GTEST_DIR = /opt/homebrew/opt/googletest
GBENCH_DIR = /opt/homebrew/opt/google-benchmark

TEST_BIN = circular_buffer_test
BENCH_BIN = circular_buffer_bench

.PHONY: all clean test bench

all: $(TEST_BIN)

$(TEST_BIN): CircularBufferTest.cpp *.h
	$(CXX) $(CXXFLAGS) -I$(GTEST_DIR)/include -o $@ $< -L$(GTEST_DIR)/lib -lgtest -lgtest_main

$(BENCH_BIN): ProducerConsumerBench.cpp *.h
	$(CXX) $(CXXFLAGS) -I$(GBENCH_DIR)/include -o $@ $< -L$(GBENCH_DIR)/lib -lbenchmark -lbenchmark_main

test: $(TEST_BIN)
	./$(TEST_BIN)

bench: $(BENCH_BIN)
	./$(BENCH_BIN)

clean:
	rm -f $(TEST_BIN) $(BENCH_BIN)
