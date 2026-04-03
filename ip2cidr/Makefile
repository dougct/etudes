test: Ip2CidrTests.cpp
	clang++ -O2 -std=c++20 -Wall -Wextra `pkg-config --cflags --libs gtest` -L/opt/homebrew/Cellar/llvm/21.1.8/lib/c++ -Wl,-rpath,/opt/homebrew/Cellar/llvm/21.1.8/lib/c++ -o ip2cidr_tests Ip2CidrTests.cpp
	./ip2cidr_tests

clean:
	rm -f ip2cidr_tests
