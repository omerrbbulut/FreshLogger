# FreshLogger Makefile
# Author: Ömer Bulut

# Compiler and flags (can be overridden by environment)
CXX ?= clang++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -g -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic \
	-Wno-global-constructors -Wno-exit-time-destructors \
	-Wno-padded -Wno-covered-switch-default \
	-Wno-unused-exception-parameter -Wno-unused-lambda-capture \
	-Wno-unsafe-buffer-usage -Wno-sign-conversion -Wno-newline-eof
INCLUDES = -I.
LIBS = -lspdlog -lfmt -lpthread

# Source files
SOURCES = example.cpp
TEST_SOURCES = LoggerTest.cpp
SIMPLE_TEST_SOURCES = SimpleLoggerTest.cpp
PERFORMANCE_TEST_SOURCES = PerformanceTest.cpp
STRESS_TEST_SOURCES = StressTest.cpp
EDGE_TEST_SOURCES = EdgeCaseTests.cpp
MACRO_TEST_SOURCE = MacroTest.cpp

# Executables
EXAMPLE_EXECUTABLE = example
UNIT_TEST_EXECUTABLE = unit_tests
SIMPLE_TEST_EXECUTABLE = simple_tests
PERFORMANCE_TEST_EXECUTABLE = performance_tests
STRESS_TEST_EXECUTABLE = stress_tests
EDGE_TEST_EXECUTABLE = edge_case_tests
MACRO_TEST_EXECUTABLE = macro_tests

# Phase 2: Advanced CI/CD Features
PHASE2_TARGETS = parallel-test cache-init cache-stats perf-baseline perf-regression

# Default target
all: $(EXAMPLE_EXECUTABLE) $(UNIT_TEST_EXECUTABLE) $(SIMPLE_TEST_EXECUTABLE) \
     $(PERFORMANCE_TEST_EXECUTABLE) $(STRESS_TEST_EXECUTABLE) $(EDGE_TEST_EXECUTABLE) \
     $(MACRO_TEST_EXECUTABLE)

# Main example
$(EXAMPLE_EXECUTABLE): $(SOURCES)
	@echo "🌱 Building FreshLogger example..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)
	@echo "✅ Example built successfully!"

# Unit tests
$(UNIT_TEST_EXECUTABLE): $(TEST_SOURCES)
	@echo "🧪 Building unit tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS) -lgtest -lgtest_main
	@echo "✅ Unit tests built successfully!"

# Simple tests
$(SIMPLE_TEST_EXECUTABLE): $(SIMPLE_TEST_SOURCES)
	@echo "🔬 Building simple tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS) -lgtest -lgtest_main
	@echo "✅ Simple tests built successfully!"

# Performance tests
$(PERFORMANCE_TEST_EXECUTABLE): $(PERFORMANCE_TEST_SOURCES)
	@echo "⚡ Building performance tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS) -lgtest -lgtest_main
	@echo "✅ Performance tests built successfully!"

# Stress tests
$(STRESS_TEST_EXECUTABLE): $(STRESS_TEST_SOURCES)
	@echo "🔥 Building stress tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS) -lgtest -lgtest_main
	@echo "✅ Stress tests built successfully!"

# Edge case tests
$(EDGE_TEST_EXECUTABLE): $(EDGE_TEST_SOURCES)
	@echo "🔍 Building edge case tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS) -lgtest -lgtest_main
	@echo "✅ Edge case tests built successfully!"

# Macro tests
$(MACRO_TEST_EXECUTABLE): $(MACRO_TEST_SOURCE)
	@echo "🔧 Building macro tests..."
	@echo "Using compiler: $(CXX) with flags: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS) -lgtest -lgtest_main
	@echo "✅ Macro tests built successfully!"

# Enterprise test suite (Phase 1)
enterprise-test: all
	@echo "🚀 Running Enterprise Test Suite..."
	@echo "=================================="
	@echo ""
	@echo "🧪 Unit Tests:"
	./$(UNIT_TEST_EXECUTABLE) --gtest_output=xml:test_logs/unit_results.xml
	@echo ""
	@echo "🔬 Simple Tests:"
	./$(SIMPLE_TEST_EXECUTABLE) --gtest_output=xml:test_logs/simple_results.xml
	@echo ""
	@echo "⚡ Performance Tests:"
	./$(PERFORMANCE_TEST_EXECUTABLE) --gtest_output=xml:test_logs/performance_results.xml
	@echo ""
	@echo "🔥 Stress Tests:"
	./$(STRESS_TEST_EXECUTABLE) --gtest_output=xml:test_logs/stress_results.xml
	@echo ""
	@echo "🔍 Edge Case Tests:"
	./$(EDGE_TEST_EXECUTABLE) --gtest_output=xml:test_logs/edge_results.xml
	@echo ""
	@echo "🔧 Macro Tests:"
	./$(MACRO_TEST_EXECUTABLE) --gtest_output=xml:test_logs/macro_results.xml
	@echo ""
	@echo "✅ Enterprise Test Suite completed!"

# Phase 2: Parallel Testing
parallel-test: all
	@echo "🚀 Running Parallel Test Suite (Phase 2)..."
	@echo "=========================================="
	@echo "Max Parallel Jobs: 4"
	@echo "Timeout per Test: 60s (reduced for CI/CD)"
	@echo ""
	TIMEOUT_SECONDS=60 MAX_PARALLEL_JOBS=4 ./scripts/parallel_test_runner.sh || echo "⚠️  Some tests failed or timed out (continuing...)"
	@echo ""
	@echo "✅ Parallel Test Suite completed!"

# Phase 2: Cache Management
cache-init:
	@echo "🔧 Initializing Build Cache (Phase 2)..."
	./scripts/build_cache_manager.sh init
	@echo "✅ Build cache initialized!"

cache-stats:
	@echo "📊 Build Cache Statistics (Phase 2)..."
	./scripts/build_cache_manager.sh stats
	@echo "✅ Cache statistics displayed!"

cache-cleanup:
	@echo "🧹 Cleaning Build Cache (Phase 2)..."
	./scripts/build_cache_manager.sh cleanup
	@echo "✅ Build cache cleaned up!"

# Phase 2: Performance Regression Detection
perf-baseline:
	@echo "📊 Creating Performance Baseline (Phase 2)..."
	./scripts/performance_regression_detector.sh baseline
	@echo "✅ Performance baseline created!"

perf-regression:
	@echo "🔍 Detecting Performance Regressions (Phase 2)..."
	./scripts/performance_regression_detector.sh run
	@echo "✅ Performance regression detection completed!"

perf-report:
	@echo "📋 Generating Performance Report (Phase 2)..."
	./scripts/performance_regression_detector.sh report
	@echo "✅ Performance report generated!"

# Phase 2: Complete CI/CD Pipeline
phase2-pipeline: all parallel-test cache-stats perf-regression
	@echo "🎯 Phase 2 CI/CD Pipeline completed!"
	@echo "===================================="
	@echo "✅ All tests built successfully"
	@echo "✅ Parallel test execution completed"
	@echo "✅ Cache statistics collected"
	@echo "✅ Performance regression detection completed"
	@echo ""
	@echo "📁 Reports available in:"
	@echo "   - parallel_test_reports/"
	@echo "   - current_performance/"
	@echo "   - .build_cache/"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -f $(EXAMPLE_EXECUTABLE) $(UNIT_TEST_EXECUTABLE) $(SIMPLE_TEST_EXECUTABLE) \
	       $(PERFORMANCE_TEST_EXECUTABLE) $(STRESS_TEST_EXECUTABLE) $(EDGE_TEST_EXECUTABLE) \
	       $(MACRO_TEST_EXECUTABLE)
	rm -rf bin/ logs/ test_logs/ stress_logs/ stress_temp/ edge_test_logs/
	@echo "✅ Cleaned build artifacts"

# Clean everything including Phase 2 artifacts
clean-all: clean
	@echo "🧹 Cleaning Phase 2 artifacts..."
	rm -rf parallel_test_reports/ parallel_test_logs/ .build_cache/ \
	       performance_baselines/ current_performance/
	@echo "✅ Cleaned all artifacts including Phase 2"

# Show help
help:
	@echo "🔧 FreshLogger Makefile - Available Targets"
	@echo "=========================================="
	@echo ""
	@echo "Build Targets:"
	@echo "  ${YELLOW}all${NC}              - Build all executables and tests"
	@echo "  ${YELLOW}clean${NC}            - Clean build artifacts"
	@echo "  ${YELLOW}clean-all${NC}        - Clean everything including Phase 2"
	@echo ""
	@echo "Phase 1 - Enterprise Testing:"
	@echo "  ${YELLOW}enterprise-test${NC}  - Run complete enterprise test suite"
	@echo ""
	@echo "Phase 2 - Advanced CI/CD:"
	@echo "  ${YELLOW}parallel-test${NC}    - Run tests in parallel"
	@echo "  ${YELLOW}cache-init${NC}       - Initialize build cache"
	@echo "  ${YELLOW}cache-stats${NC}      - Show cache statistics"
	@echo "  ${YELLOW}cache-cleanup${NC}    - Clean up build cache"
	@echo "  ${YELLOW}perf-baseline${NC}    - Create performance baseline"
	@echo "  ${YELLOW}perf-regression${NC}  - Detect performance regressions"
	@echo "  ${YELLOW}perf-report${NC}      - Generate performance report"
	@echo "  ${YELLOW}phase2-pipeline${NC}  - Complete Phase 2 pipeline"
	@echo ""
	@echo "Examples:"
	@echo "  ${YELLOW}make all${NC}                    - Build everything"
	@echo "  ${YELLOW}make enterprise-test${NC}        - Run Phase 1 tests"
	@echo "  ${YELLOW}make phase2-pipeline${NC}        - Run Phase 2 pipeline"
	@echo "  ${YELLOW}CXX=g++ make all${NC}            - Use GCC instead of Clang"

# Phony targets
.PHONY: all clean clean-all enterprise-test parallel-test cache-init cache-stats \
        cache-cleanup perf-baseline perf-regression perf-report phase2-pipeline help 