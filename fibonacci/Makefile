test: FibonacciTests.cpp
	clang++ -O2 -std=c++20 -Wall -Wextra `pkg-config --cflags --libs gtest` -L/opt/homebrew/Cellar/llvm/21.1.8/lib/c++ -Wl,-rpath,/opt/homebrew/Cellar/llvm/21.1.8/lib/c++ -o fibonacci_tests FibonacciTests.cpp
	./fibonacci_tests

clean:
	rm -f fibonacci_tests
