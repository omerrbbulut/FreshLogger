# 🚀 FreshLogger - Enterprise-Grade C++ Logging Library

A high-performance, feature-rich C++ logging library with comprehensive CI/CD pipeline and enterprise-grade testing.

## ✨ Features

### 🔧 Core Logging Features
- **Multi-level logging**: TRACE, DEBUG, INFO, WARNING, ERROR, FATAL
- **File rotation**: Automatic log file rotation with size limits
- **Async logging**: High-performance asynchronous logging
- **Thread-safe**: Full thread safety for concurrent applications
- **Custom patterns**: Configurable log message patterns
- **Console & file output**: Flexible output configuration

### 🏢 Enterprise Features
- **Comprehensive test suite**: Unit, performance, stress, edge case tests
- **Code quality tools**: clang-tidy, cppcheck integration
- **Security scanning**: OWASP Dependency-Check integration
- **Cross-compiler support**: GCC and Clang compatibility
- **Performance monitoring**: Regression detection and benchmarking

## 🚀 CI/CD Pipeline

### Phase 1: Code Quality & Security
- ✅ **Code Quality Analysis**: clang-tidy, cppcheck
- ✅ **Security Scanning**: OWASP Dependency-Check
- ✅ **Cross-Compiler Testing**: GCC-11, Clang-14

### Phase 2: Advanced Testing & Performance
- ✅ **Parallel Test Execution**: Multi-threaded test running
- ✅ **Build Caching System**: Intelligent build artifact caching
- ✅ **Performance Regression Detection**: Historical performance tracking

### Phase 3: Enterprise Features (Planned)
- 🔄 **Cross-Platform Testing**: Windows, macOS support
- 🔄 **Integration Tests**: Real-world scenario testing
- 🔄 **Load Testing**: High concurrency testing

## 📦 Installation

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install -y clang++ libspdlog-dev libfmt-dev libgtest-dev

# Arch Linux
sudo pacman -S clang spdlog fmt gtest

# macOS
brew install clang spdlog fmt googletest
```

### Build & Test
```bash
# Clone the repository
git clone https://github.com/omerrbbulut/FreshLogger.git
cd FreshLogger

# Build all components
make all

# Run enterprise test suite
make enterprise-test

# Run Phase 2 features
make phase2-pipeline
```

## 🔧 Usage

### Basic Usage
```cpp
#include "Logger.hpp"

int main() {
    // Default logger (console output)
    Logger logger;
    logger.info("Hello, World!");
    
    // Custom configuration
    Logger::Config config;
    config.logFilePath = "app.log";
    config.minLevel = Logger::LogLevel::DEBUG;
    config.asyncLogging = true;
    
    Logger customLogger(config);
    customLogger.debug("Debug message");
    customLogger.info("Info message");
    customLogger.warning("Warning message");
    customLogger.error("Error message");
    
    return 0;
}
```

### Convenience Macros
```cpp
#include "Logger.hpp"

// Global logger instance
Logger g_logger;

// Convenience macros
LOG_TRACE("Trace message");
LOG_DEBUG("Debug message");
LOG_INFO("Info message");
LOG_WARNING("Warning message");
LOG_ERROR("Error message");
LOG_FATAL("Fatal message");
```

## 🧪 Testing

### Test Suites
```bash
# Individual test suites
make unit_tests && ./unit_tests
make performance_tests && ./performance_tests
make stress_tests && ./stress_tests
make edge_case_tests && ./edge_case_tests
make macro_tests && ./macro_tests

# Complete enterprise test suite
make enterprise-test

# Parallel test execution
make parallel-test

# Performance regression detection
make perf-regression
```

### Test Coverage
- **Unit Tests**: Core functionality testing
- **Performance Tests**: Throughput and latency benchmarks
- **Stress Tests**: High-load and stability testing
- **Edge Case Tests**: Boundary condition testing
- **Macro Tests**: Convenience macro functionality

## 🔧 Build System

### Available Targets
```bash
# Build targets
make all                    # Build everything
make clean                  # Clean build artifacts
make clean-all             # Clean everything including Phase 2

# Phase 1 - Enterprise Testing
make enterprise-test       # Run complete enterprise test suite

# Phase 2 - Advanced CI/CD
make parallel-test         # Run tests in parallel
make cache-init           # Initialize build cache
make cache-stats          # Show cache statistics
make cache-cleanup        # Clean up build cache
make perf-baseline        # Create performance baseline
make perf-regression      # Detect performance regressions
make perf-report          # Generate performance report
make phase2-pipeline      # Complete Phase 2 pipeline

# Help
make help                 # Show all available targets
```

### Environment Variables
```bash
# Compiler selection
export CXX=clang++        # Use Clang (default)
export CXX=g++            # Use GCC

# Cache configuration
export CACHE_MAX_SIZE=1000        # Cache size in MB
export CACHE_CLEANUP_THRESHOLD=80 # Cleanup threshold %

# Performance testing
export REGRESSION_THRESHOLD=5     # Performance threshold %
export HISTORY_SIZE=20            # Baseline history size

# Parallel testing
export MAX_PARALLEL_JOBS=4        # Max parallel jobs
export TIMEOUT_SECONDS=120        # Test timeout
```

## 📊 Performance

### Benchmarks
- **Throughput**: 8.8M+ messages/second
- **Latency**: <1μs average
- **Memory Usage**: Optimized for low overhead
- **File Rotation**: Efficient log file management

### Performance Monitoring
- **Regression Detection**: Automatic performance regression detection
- **Historical Tracking**: Baseline performance history
- **Real-time Monitoring**: Live performance metrics
- **HTML Reports**: Detailed performance analysis

## 🔒 Security

### Security Features
- **Dependency Scanning**: OWASP Dependency-Check integration
- **Code Quality**: Static analysis with clang-tidy and cppcheck
- **Vulnerability Detection**: Automatic security vulnerability scanning
- **Suppression Management**: Configurable false positive suppression

## 🏗️ CI/CD Pipeline

### GitHub Actions Workflow
The project includes a comprehensive GitHub Actions workflow with:

1. **Code Quality Job**
   - clang-tidy analysis
   - cppcheck static analysis
   - OWASP security scanning

2. **Cross-Compiler Job**
   - GCC-11 builds and tests
   - Clang-14 builds and tests
   - Matrix strategy for multiple compilers

3. **Parallel Testing Job**
   - Parallel test execution
   - Build cache management
   - Performance regression detection

4. **Cache Analysis Job**
   - Build cache effectiveness analysis
   - Cache hit/miss statistics
   - Cache optimization recommendations

5. **Auto Version & Release Job**
   - Semantic versioning based on commit messages
   - Automatic GitHub releases
   - Git tag management

### Pipeline Features
- **Parallel Execution**: Reduced CI time with parallel jobs
- **Artifact Management**: Comprehensive artifact collection
- **Status Reporting**: Detailed pipeline status reports
- **Failure Handling**: Graceful failure handling and reporting

## 📈 Monitoring & Analytics

### Build Cache Analytics
```bash
# Cache statistics
make cache-stats

# Cache usage analysis
./scripts/build_cache_manager.sh usage

# Cache cleanup
make cache-cleanup
```

### Performance Analytics
```bash
# Performance baseline creation
make perf-baseline

# Performance regression detection
make perf-regression

# Performance report generation
make perf-report
```

## 🤝 Contributing

### Development Setup
```bash
# Clone and setup
git clone https://github.com/omerrbbulut/FreshLogger.git
cd FreshLogger

# Install dependencies
make install-deps

# Build and test
make all
make enterprise-test

# Run CI/CD pipeline locally
make phase2-pipeline
```

### Code Quality
- All code must pass clang-tidy analysis
- cppcheck must not report critical issues
- Security scan must pass without critical vulnerabilities
- Performance regressions are not allowed

### Testing Requirements
- All tests must pass
- Performance benchmarks must meet minimum thresholds
- Stress tests must complete successfully
- Edge case tests must handle all scenarios

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **spdlog**: High-performance logging library
- **fmt**: Modern formatting library
- **Google Test**: Testing framework
- **OWASP**: Security scanning tools

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/omerrbbulut/FreshLogger/issues)
- **Discussions**: [GitHub Discussions](https://github.com/omerrbbulut/FreshLogger/discussions)
- **Wiki**: [Project Wiki](https://github.com/omerrbbulut/FreshLogger/wiki)

---

**Made with ❤️ by Ömer Bulut** 