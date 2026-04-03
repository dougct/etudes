bench: SuffixArrayBench.cpp
	c++ -O2 -std=c++20 -Wall -lpthread -lbenchmark -o suffix_array_bench SuffixArrayBench.cpp

test: SuffixArrayTests.cpp
	c++ -O2 -std=c++20 -Wall -lpthread -lgtest -lgtest_main -o suffix_array_tests SuffixArrayTests.cpp

clean:
	rm -f suffix_array_bench suffix_array_tests
