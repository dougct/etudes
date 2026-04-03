CXX = /usr/bin/clang++
CXXFLAGS = -O2 -std=c++20 -Wall -Wextra
GTEST_DIR = $(HOME)/local
GTEST_CFLAGS = -I$(GTEST_DIR)/include
GTEST_LIBS = -L$(GTEST_DIR)/lib -lgtest

test: Ip2CidrTests.cpp
	$(CXX) $(CXXFLAGS) $(GTEST_CFLAGS) -o ip2cidr_tests Ip2CidrTests.cpp $(GTEST_LIBS)
	./ip2cidr_tests

clean:
	rm -f ip2cidr_tests
