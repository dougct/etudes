CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Generic rule: make <name> compiles and runs <name>.cpp
%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@.bin $<
	./$@.bin

clean:
	rm -f *.bin

.PHONY: clean
