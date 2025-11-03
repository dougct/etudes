CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra
GTEST_FLAGS = -lgtest -lgtest_main -pthread

TARGET = test_runner
TEST_SRC = Tests.cpp
HEADERS = generate_binary_nums.h generate_permutations.h

.PHONY: all test clean python

all: $(TARGET)

$(TARGET): $(TEST_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TARGET) $(GTEST_FLAGS)

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

python:
	@echo "Running all Python files...\n"
	@for file in *.py; do \
		if [ -f "$$file" ]; then \
			echo "Running $$file"; \
			python3 "$$file"; \
			echo "";\
		fi; \
	done

install-deps:
	@echo "Please install Google Test library:"
	@echo "  Ubuntu/Debian: sudo apt-get install libgtest-dev"
	@echo "  macOS: brew install googletest"
	@echo "  Or build from source: https://github.com/google/googletest"
