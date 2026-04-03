CXX = /usr/bin/clang++
CXXFLAGS = -O2 -std=c++20 -Wall -Wextra
GTEST_DIR = $(HOME)/local
GTEST_CFLAGS = -I$(GTEST_DIR)/include
GTEST_LIBS = -L$(GTEST_DIR)/lib -lgtest

test: HashMapTests.cpp
	$(CXX) $(CXXFLAGS) $(GTEST_CFLAGS) -o hashmap_tests HashMapTests.cpp $(GTEST_LIBS)
	./hashmap_tests

clean:
	rm -f hashmap_tests
