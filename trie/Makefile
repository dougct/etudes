test: TrieTests.cpp
	clang++ -O2 -std=c++20 -Wall -Wextra `pkg-config --cflags --libs gtest` -L/opt/homebrew/Cellar/llvm/21.1.8/lib/c++ -Wl,-rpath,/opt/homebrew/Cellar/llvm/21.1.8/lib/c++ -o trie_tests TrieTests.cpp
	./trie_tests

clean:
	rm -f trie_tests
