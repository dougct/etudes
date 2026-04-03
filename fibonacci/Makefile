CXX = /usr/bin/clang++
CXXFLAGS = -O2 -std=c++20 -Wall -Wextra
GTEST_DIR = $(HOME)/local
GTEST_CFLAGS = -I$(GTEST_DIR)/include
GTEST_LIBS = -L$(GTEST_DIR)/lib -lgtest

test: FibonacciTests.cpp
	$(CXX) $(CXXFLAGS) $(GTEST_CFLAGS) -o fibonacci_tests FibonacciTests.cpp $(GTEST_LIBS)
	./fibonacci_tests

clean:
	rm -f fibonacci_tests
