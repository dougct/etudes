CXX = /usr/bin/clang++
CXXFLAGS = -O2 -std=c++20 -Wall -Wextra
GTEST_DIR = $(HOME)/local
GTEST_CFLAGS = -I$(GTEST_DIR)/include
GTEST_LIBS = -L$(GTEST_DIR)/lib -lgtest

test: TrieTests.cpp
	$(CXX) $(CXXFLAGS) $(GTEST_CFLAGS) -o trie_tests TrieTests.cpp $(GTEST_LIBS)
	./trie_tests

clean:
	rm -f trie_tests
