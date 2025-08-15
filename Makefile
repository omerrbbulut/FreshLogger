# FreshLogger Makefile
# Author: Ömer Bulut

# Compiler and flags (can be overridden by environment)
CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -g
INCLUDES = -I.
LIBS = -lspdlog -lfmt -lpthread

# Source files
EXAMPLE_SOURCE = example.cpp
TEST_SOURCE = LoggerTest.cpp
SIMPLE_TEST_SOURCE = SimpleLoggerTest.cpp
PERFORMANCE_SOURCE = PerformanceTest.cpp
STRESS_SOURCE = StressTest.cpp
EDGE_CASE_SOURCE = EdgeCaseTests.cpp
MACRO_TEST_SOURCE = MacroTest.cpp

# Executables
EXAMPLE = example
TEST_EXECUTABLE = unit_tests
SIMPLE_TEST_EXECUTABLE = simple_tests
PERFORMANCE_EXECUTABLE = performance_tests
STRESS_EXECUTABLE = stress_tests
EDGE_CASE_EXECUTABLE = edge_case_tests
MACRO_TEST_EXECUTABLE = macro_tests

# Default target
all: $(EXAMPLE) $(TEST_EXECUTABLE) $(SIMPLE_TEST_EXECUTABLE) $(PERFORMANCE_EXECUTABLE) $(STRESS_EXECUTABLE) $(EDGE_CASE_EXECUTABLE) $(MACRO_TEST_EXECUTABLE)

# Build main example
$(EXAMPLE): $(EXAMPLE_SOURCE)
	@echo "🌱 Building FreshLogger example..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LIBS)
	@echo "✅ Example built successfully!"

# Build unit tests
$(TEST_EXECUTABLE): $(TEST_SOURCE)
	@echo "🧪 Building unit tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LIBS) -lgtest -lgtest_main
	@echo "✅ Unit tests built successfully!"

# Build simple tests
$(SIMPLE_TEST_EXECUTABLE): $(SIMPLE_TEST_SOURCE)
	@echo "🔬 Building simple tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LIBS) -lgtest -lgtest_main
	@echo "✅ Simple tests built successfully!"

# Build performance tests
$(PERFORMANCE_EXECUTABLE): $(PERFORMANCE_SOURCE)
	@echo "⚡ Building performance tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LIBS) -lgtest -lgtest_main
	@echo "✅ Performance tests built successfully!"

# Build stress tests
$(STRESS_EXECUTABLE): $(STRESS_SOURCE)
	@echo "🔥 Building stress tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LIBS) -lgtest -lgtest_main
	@echo "✅ Stress tests built successfully!"

# Build edge case tests
$(EDGE_CASE_EXECUTABLE): $(EDGE_CASE_SOURCE)
	@echo "🔍 Building edge case tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LIBS) -lgtest -lgtest_main
	@echo "✅ Edge case tests built successfully!"

# Build macro tests
$(MACRO_TEST_EXECUTABLE): $(MACRO_TEST_SOURCE)
	@echo "🔧 Building macro tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LIBS) -lgtest -lgtest_main
	@echo "✅ Macro tests built successfully!"

# Test targets
test: $(TEST_EXECUTABLE)
	@echo "🧪 Running unit tests..."
	./$(TEST_EXECUTABLE)

simple-test: $(SIMPLE_TEST_EXECUTABLE)
	@echo "🔬 Running simple tests..."
	./$(SIMPLE_TEST_EXECUTABLE)

performance-tests: $(PERFORMANCE_EXECUTABLE)
	@echo "⚡ Running performance tests..."
	./$(PERFORMANCE_EXECUTABLE)

stress-tests: $(STRESS_EXECUTABLE)
	@echo "🔥 Running stress tests..."
	./$(STRESS_EXECUTABLE)

# Run edge case tests
edge-case-tests: $(EDGE_CASE_EXECUTABLE)
	@echo "🔍 Running edge case tests..."
	./$(EDGE_CASE_EXECUTABLE)

# Run all tests
enterprise-test: $(SIMPLE_TEST_EXECUTABLE) $(TEST_EXECUTABLE) $(PERFORMANCE_EXECUTABLE) $(STRESS_EXECUTABLE) $(EDGE_CASE_EXECUTABLE) $(MACRO_TEST_EXECUTABLE)
	@echo "🏢 Running enterprise-grade test suite..."
	@echo ""
	@echo "=== BASIC FUNCTIONALITY ==="
	./$(SIMPLE_TEST_EXECUTABLE)
	@echo ""
	@echo "=== UNIT TESTS ==="
	./$(TEST_EXECUTABLE)
	@echo ""
	@echo "=== PERFORMANCE BENCHMARKS ==="
	./$(PERFORMANCE_EXECUTABLE)
	@echo ""
	@echo "=== STRESS AND STABILITY ==="
	./$(STRESS_EXECUTABLE)
	@echo ""
	@echo "=== EDGE CASES AND BOUNDARY CONDITIONS ==="
	./$(EDGE_CASE_EXECUTABLE)
	@echo ""
	@echo "=== MACRO TESTS ==="
	./$(MACRO_TEST_EXECUTABLE)
	@echo ""
	@echo "🏆 Enterprise-grade test suite completed!"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -f $(EXAMPLE) $(TEST_EXECUTABLE) $(SIMPLE_TEST_EXECUTABLE) $(PERFORMANCE_EXECUTABLE) $(STRESS_EXECUTABLE) $(EDGE_CASE_EXECUTABLE) $(MACRO_TEST_EXECUTABLE)
	rm -rf bin/ logs/ test_logs/ stress_logs/ stress_temp/ edge_test_logs/
	@echo "✅ Cleaned build artifacts"

# Install dependencies (Arch Linux)
install-deps:
	@echo "📦 Installing dependencies for Arch Linux..."
	sudo pacman -S --noconfirm gcc gtest spdlog fmt
	@echo "✅ Dependencies installed successfully!"

# Check if dependencies are available
check-deps:
	@echo "🔍 Checking dependencies..."
	@echo "C++ Compiler: $(CXX)"
	@echo "C++ Flags: $(CXXFLAGS)"
	@echo "spdlog: $(shell pkg-config --exists spdlog && echo "✅ Found" || echo "❌ Not found")"
	@echo "fmt: $(shell pkg-config --exists fmt && echo "✅ Found" || echo "❌ Not found")"
	@echo "gtest: $(shell pkg-config --exists gtest && echo "✅ Found" || echo "❌ Not found")"

# Help target
help:
	@echo "🌱 FreshLogger Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all              - Build everything"
	@echo "  example          - Build main example"
	@echo "  unit-tests       - Build unit tests"
	@echo "  simple-tests     - Build simple tests"
	@echo "  performance-tests - Build performance tests"
	@echo "  stress-tests     - Build stress tests"
	@echo "  edge-case-tests  - Build edge case tests"
	@echo "  test             - Build and run unit tests"
	@echo "  simple-test      - Build and run simple tests"
	@echo "  performance-tests - Build and run performance tests"
	@echo "  stress-tests     - Build and run stress tests"
	@echo "  edge-case-tests  - Build and run edge case tests"
	@echo "  enterprise-test  - Run complete test suite"
	@echo "  clean            - Clean build artifacts"
	@echo "  install-deps     - Install dependencies (Arch Linux)"
	@echo "  check-deps       - Check dependency availability"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Environment variables:"
	@echo "  CXX              - C++ compiler (default: g++)"
	@echo "  CXXFLAGS         - C++ compiler flags (default: -std=c++17 -Wall -Wextra -O2 -g)"

.PHONY: all test simple-test performance-tests stress-tests edge-case-tests enterprise-test clean install-deps check-deps help 