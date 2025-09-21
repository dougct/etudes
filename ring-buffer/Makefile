
test-singlethread:
	clang++ -std=c++20 -Wall -Wextra `pkg-config --cflags --libs gtest` SingleThreadRingBufferTest.cpp -o single_thread_ring_buffer_tests
	./single_thread_ring_buffer_tests

test-lockfree:
	clang++ -std=c++20 -Wall -Wextra `pkg-config --cflags --libs gtest` LockFreeRingBufferTest.cpp -o lock_free_ring_buffer_tests
	./lock_free_ring_buffer_tests

test-all: test-lockfree test-singlethread

test:
ifeq ($(version),singlethread)
	$(MAKE) test-singlethread
else ifeq ($(version),lockfree)
	$(MAKE) test-lockfree
else ifeq ($(version),all)
	$(MAKE) test-all
else
	@echo "Usage: make test version=[singlethread|lockfree|all]"
	@echo "  make test version=singlethread - run single-threaded ring buffer tests"
	@echo "  make test version=lockfree     - run lock-free ring buffer tests"
	@echo "  make test version=all          - run both test suites"
	@echo ""
	@echo "Or use the direct targets:"
	@echo "  make test-singlethread - run single-threaded ring buffer tests"
	@echo "  make test-lockfree     - run lock-free ring buffer tests"
	@echo "  make test-all          - run both test suites"
endif

bench:
		clang++ -std=c++20 -Wall -Wextra -O3 `pkg-config --cflags --libs benchmark gtest` RingBufferBench.cpp -o ring_buffer_bench
		./ring_buffer_bench --benchmark_report_aggregates_only=true

clean:
		rm -rf lock_free_ring_buffer_tests single_thread_ring_buffer_tests ring_buffer_bench
