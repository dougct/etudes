CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra

SOURCES = $(wildcard *.cpp)
TARGETS = $(SOURCES:.cpp=)

# Run all cpp files
all:
	@for src in $(SOURCES); do \
		name=$${src%.cpp}; \
		echo "\n=== Building and running $$name ==="; \
		$(CXX) $(CXXFLAGS) -o $$name.bin $$src && ./$$name.bin || exit 1; \
	done

# Generic rule: make <name> compiles and runs <name>.cpp
%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@.bin $<
	./$@.bin

clean:
	rm -f *.bin

.PHONY: all clean
