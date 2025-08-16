# 🔧 FreshLogger Troubleshooting Guide

**Author:** Ömer Bulut  
**Version:** 1.3.0  
**Last Updated:** August 16, 2025

---

## 📋 Table of Contents

1. [Quick Diagnosis](#quick-diagnosis)
2. [Common Issues](#common-issues)
3. [Error Messages](#error-messages)
4. [Performance Problems](#performance-problems)
5. [Build Issues](#build-issues)
6. [Runtime Issues](#runtime-issues)
7. [File System Issues](#file-system-issues)
8. [Debugging Techniques](#debugging-techniques)
9. [Getting Help](#getting-help)

---

## 🚨 Quick Diagnosis

### **Symptom Checker**

| Symptom | Likely Cause | Quick Fix |
|---------|--------------|-----------|
| **Build fails** | Missing dependencies | `sudo apt-get install libspdlog-dev libfmt-dev libgtest-dev` |
| **Tests fail** | File permissions | `chmod +x scripts/*.sh` |
| **Low performance** | Wrong configuration | Enable `asyncLogging: true` |
| **Memory issues** | Large queue size | Reduce `queueSize` |
| **File rotation errors** | Small file size | Increase `maxFileSize` |

### **Emergency Reset**
```bash
# Clean everything and rebuild
make clean-all
make all
make enterprise-test
```

---

## ❌ Common Issues

### **1. Build Failures**

#### **Missing Dependencies**
```bash
# Error: fatal error: 'spdlog/spdlog.h' file not found
sudo apt-get install -y libspdlog-dev libfmt-dev libgtest-dev
```

#### **Compiler Version Issues**
```bash
# Error: 'std::filesystem' is not a member of 'std'
# Solution: Use C++17 or later
export CXXFLAGS="-std=c++17"
make clean && make all
```

#### **Permission Issues**
```bash
# Error: Permission denied
sudo chown -R $USER:$USER .
chmod +x scripts/*.sh
```

### **2. Test Failures**

#### **File System Errors**
```bash
# Error: cannot create directories: Permission denied
mkdir -p test_logs
chmod 755 test_logs
```

#### **Memory Issues**
```bash
# Error: Memory increase should be < 500MB
# Solution: Check system memory, close other applications
free -h
```

#### **Timeout Issues**
```bash
# Error: Test timeout
# Solution: Increase timeout or reduce test load
export TIMEOUT_SECONDS=300
```

### **3. Runtime Issues**

#### **Logger Not Working**
```cpp
// Problem: No log output
Logger logger;
logger.info("Test message"); // Nothing appears

// Solution: Check configuration
Logger::Config config;
config.consoleOutput = true;  // Enable console output
config.minLevel = Logger::LogLevel::INFO;
Logger logger(config);
```

#### **File Output Issues**
```cpp
// Problem: Logs not written to file
Logger::Config config;
config.logFilePath = "app.log";
config.consoleOutput = false;

// Solution: Ensure directory exists
std::filesystem::create_directories("logs");
config.logFilePath = "logs/app.log";
```

---

## 🚨 Error Messages

### **Build Errors**

#### **1. Compiler Errors**
```bash
# Error: 'Logger' does not name a type
#include "Logger.hpp"  // Check include path

# Error: undefined reference to 'spdlog'
# Missing library linkage
g++ -o program main.cpp -lspdlog -lfmt -lpthread
```

#### **2. Linker Errors**
```bash
# Error: undefined reference to 'vtable for Logger'
# Missing virtual function implementation
# Check Logger.cpp is included in build
```

#### **3. Template Errors**
```bash
# Error: template instantiation failed
# Check C++17 support
g++ --version
# Should show GCC 7+ or Clang 5+
```

### **Runtime Errors**

#### **1. File System Errors**
```
[*** LOG ERROR #0001 ***] rotating_file_sink: failed renaming 
stress_logs/memory.40.log to stress_logs/memory.41.log: 
No such file or directory
```

**Cause:** File rotation race condition
**Solution:** Increase `maxFileSize`, reduce `maxFiles`

```cpp
Logger::Config config;
config.maxFileSize = 10 * 1024 * 1024; // 10MB instead of 1MB
config.maxFiles = 10;                   // 10 instead of 50
```

#### **2. Memory Errors**
```
Memory increase should be < 500MB (realistic for 400K large messages)
```

**Cause:** High memory usage during stress tests
**Solution:** Optimize message size, reduce test load

```cpp
// Reduce message size in tests
size_t messageSize = 1000 + (i % 500); // Smaller messages
```

#### **3. Performance Errors**
```
Performance regression detected (ratio: 0.792)
```

**Cause:** Performance degradation
**Solution:** Check recent changes, optimize configuration

---

## 📉 Performance Problems

### **1. Low Throughput**

#### **Symptoms**
- Throughput <100K messages/second
- Tests take longer than expected
- High CPU usage

#### **Diagnosis**
```bash
# Check system resources
htop
iostat 1
df -h

# Run performance test
./performance_tests --gtest_filter="PerformanceTest.SynchronousThroughput"
```

#### **Solutions**
```cpp
// Enable async logging
config.asyncLogging = true;
config.queueSize = 100000;

// Optimize file rotation
config.maxFileSize = 100 * 1024 * 1024; // 100MB
config.maxFiles = 3;

// Reduce flush frequency
config.flushInterval = 5; // 5 seconds
```

### **2. High Latency**

#### **Symptoms**
- Average latency >10 μs
- Inconsistent timing
- Spikes in response time

#### **Diagnosis**
```bash
# Profile with perf
perf record ./performance_tests
perf report

# Check file system performance
dd if=/dev/zero of=test_file bs=1M count=100
```

#### **Solutions**
```cpp
// Use synchronous logging for low latency
config.asyncLogging = false;

// Disable console output
config.consoleOutput = false;

// Optimize file operations
config.maxFileSize = 10 * 1024 * 1024; // 10MB
config.maxFiles = 2;
```

### **3. Memory Issues**

#### **Symptoms**
- Memory usage >100MB
- Memory growth over time
- Out of memory errors

#### **Diagnosis**
```bash
# Monitor memory usage
free -h
ps aux | grep performance_tests

# Use valgrind for memory analysis
valgrind --tool=massif ./performance_tests
```

#### **Solutions**
```cpp
// Reduce queue size
config.queueSize = 10000;

// Frequent flushing
config.flushInterval = 1;

// Monitor memory usage
// Check for memory leaks
```

---

## 🔨 Build Issues

### **1. Dependency Problems**

#### **Missing Libraries**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y build-essential libspdlog-dev libfmt-dev libgtest-dev

# Arch Linux
sudo pacman -S base-devel spdlog fmt googletest

# macOS
brew install spdlog fmt googletest
```

#### **Version Conflicts**
```bash
# Check installed versions
pkg-config --modversion spdlog
pkg-config --modversion fmt

# Install specific versions if needed
sudo apt-get install libspdlog-dev=1.8.0-1
```

### **2. Compiler Issues**

#### **C++ Standard Support**
```bash
# Check C++17 support
g++ -std=c++17 -dM -E - < /dev/null | grep -i "cplusplus"

# Use specific compiler version
export CXX=g++-11
export CXXFLAGS="-std=c++17 -Wall -Wextra -O2"
```

#### **Compiler Flags**
```bash
# Remove problematic flags
export CXXFLAGS="-std=c++17 -Wall -Wextra -O2"

# Build without optimization for debugging
export CXXFLAGS="-std=c++17 -Wall -Wextra -g -O0"
```

### **3. Makefile Issues**

#### **Target Not Found**
```bash
# Check available targets
make help

# Clean and rebuild
make clean
make all
```

#### **Permission Denied**
```bash
# Fix script permissions
chmod +x scripts/*.sh

# Fix directory permissions
sudo chown -R $USER:$USER .
chmod 755 .
```

---

## ⚡ Runtime Issues

### **1. Logger Initialization**

#### **Configuration Errors**
```cpp
// Problem: Invalid configuration
Logger::Config config;
config.maxFileSize = 0;  // Invalid value

// Solution: Use defaults
Logger::Config config;  // Uses default values
```

#### **File Path Issues**
```cpp
// Problem: Invalid file path
config.logFilePath = "/invalid/path/app.log";

// Solution: Use relative paths or ensure directory exists
config.logFilePath = "logs/app.log";
std::filesystem::create_directories("logs");
```

### **2. Logging Failures**

#### **No Output**
```cpp
// Problem: No log output
Logger logger;
logger.info("Test");

// Solution: Check configuration
Logger::Config config;
config.consoleOutput = true;
config.minLevel = Logger::LogLevel::INFO;
Logger logger(config);
```

#### **Partial Output**
```cpp
// Problem: Only some messages appear
logger.debug("Debug message");  // Not shown
logger.info("Info message");    // Shown

// Solution: Check log level
logger.setLogLevel(Logger::LogLevel::DEBUG);
```

### **3. Thread Safety Issues**

#### **Race Conditions**
```cpp
// Problem: Multiple threads using same logger
static Logger logger;  // Shared instance

// Solution: Thread-local or synchronized access
thread_local Logger logger;  // Per-thread instance
```

#### **Deadlocks**
```cpp
// Problem: Logger used in signal handlers
void signalHandler(int sig) {
    logger.info("Signal received");  // May cause deadlock
}

// Solution: Use async-safe logging
// Or set up logger before signal handlers
```

---

## 📁 File System Issues

### **1. Permission Problems**

#### **Directory Access**
```bash
# Check permissions
ls -la logs/
ls -la test_logs/

# Fix permissions
chmod 755 logs/
chmod 755 test_logs/
chown $USER:$USER logs/ test_logs/
```

#### **File Creation**
```bash
# Test file creation
touch logs/test.log
rm logs/test.log

# If failed, check directory permissions
sudo chown -R $USER:$USER .
```

### **2. Disk Space Issues**

#### **Insufficient Space**
```bash
# Check disk space
df -h
du -sh logs/

# Clean up old logs
find logs/ -name "*.log.*" -mtime +7 -delete
```

#### **File System Full**
```bash
# Emergency cleanup
sudo find /tmp -name "*.log" -delete
sudo find /var/log -name "*.log.*" -delete

# Check for large files
sudo find / -type f -size +100M 2>/dev/null
```

### **3. File Rotation Issues**

#### **Rotation Failures**
```cpp
// Problem: File rotation errors
config.maxFileSize = 1024;  // 1KB - too small
config.maxFiles = 100;      // Too many files

// Solution: Increase file size, reduce file count
config.maxFileSize = 10 * 1024 * 1024; // 10MB
config.maxFiles = 5;
```

#### **File Naming Conflicts**
```cpp
// Problem: File name conflicts in tests
config.logFilePath = "test.log";  // Multiple tests use same file

// Solution: Use unique file names
config.logFilePath = "test_" + std::to_string(getpid()) + ".log";
```

---

## 🔍 Debugging Techniques

### **1. Enable Debug Output**

#### **Verbose Logging**
```cpp
// Enable debug level
Logger::Config config;
config.minLevel = Logger::LogLevel::DEBUG;
config.consoleOutput = true;

// Use debug statements
logger.debug("Variable value: " + std::to_string(value));
logger.debug("Function entered: " + __FUNCTION__);
```

#### **Test Verbosity**
```bash
# Enable verbose test output
./performance_tests --gtest_verbose

# Specific test with details
./stress_tests --gtest_filter="StressTest.MemoryPressureTest" --gtest_verbose
```

### **2. Performance Profiling**

#### **CPU Profiling**
```bash
# Profile with perf
perf record -g ./performance_tests
perf report

# Profile specific function
perf record -g -p $(pgrep performance_tests)
```

#### **Memory Profiling**
```bash
# Memory analysis with valgrind
valgrind --tool=massif ./performance_tests
ms_print massif.out.*

# Memory leak detection
valgrind --tool=memcheck --leak-check=full ./performance_tests
```

### **3. System Monitoring**

#### **Resource Monitoring**
```bash
# Monitor CPU and memory
htop
iotop
nethogs

# Monitor file system
watch -n 1 'df -h; ls -la logs/'
```

#### **Network Monitoring**
```bash
# Monitor network if using remote logging
iftop
netstat -tulpn | grep logger
```

---

## 🆘 Getting Help

### **1. Self-Diagnosis**

#### **Check Documentation**
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API documentation
- [PERFORMANCE_BENCHMARK_GUIDE.md](PERFORMANCE_BENCHMARK_GUIDE.md) - Performance guide
- [TESTING.md](TESTING.md) - Testing procedures

#### **Run Diagnostics**
```bash
# System check
./scripts/system_check.sh

# Performance baseline
make perf-baseline

# Complete test suite
make enterprise-test
```

### **2. Community Support**

#### **GitHub Issues**
- **Bug Reports**: [Create Issue](https://github.com/omerrbbulut/FreshLogger/issues)
- **Feature Requests**: [Feature Request](https://github.com/omerrbbulut/FreshLogger/issues)
- **Questions**: [GitHub Discussions](https://github.com/omerrbbulut/FreshLogger/discussions)

#### **Issue Template**
```markdown
## Problem Description
Brief description of the issue

## Environment
- OS: Ubuntu 20.04
- Compiler: GCC 9.4.0
- FreshLogger Version: 1.2.0

## Steps to Reproduce
1. Step 1
2. Step 2
3. Step 3

## Expected Behavior
What should happen

## Actual Behavior
What actually happens

## Error Messages
Copy error messages here

## Additional Information
Any other relevant information
```

### **3. Professional Support**

#### **Contact Information**
- **Author**: Ömer Bulut
- **Repository**: [FreshLogger](https://github.com/omerrbbulut/FreshLogger)
- **Issues**: [GitHub Issues](https://github.com/omerrbbulut/FreshLogger/issues)

#### **Support Levels**
| Level | Response Time | Contact Method |
|-------|---------------|----------------|
| **Community** | 1-3 days | GitHub Issues |
| **Priority** | 24 hours | GitHub Issues + @mention |
| **Enterprise** | 4 hours | Direct contact |

---

## 📋 Troubleshooting Checklist

### **Before Asking for Help**
- [ ] Checked this troubleshooting guide
- [ ] Verified system requirements
- [ ] Cleaned and rebuilt project
- [ ] Ran basic tests
- [ ] Checked error logs
- [ ] Searched GitHub issues

### **When Reporting Issues**
- [ ] Clear problem description
- [ ] Environment details
- [ ] Steps to reproduce
- [ ] Error messages
- [ ] Expected vs actual behavior
- [ ] Relevant code snippets

### **After Getting Help**
- [ ] Documented solution
- [ ] Updated documentation if needed
- [ ] Thanked helpful contributors
- [ ] Shared knowledge with community

---

## 🔗 Related Documentation

- [API_REFERENCE.md](API_REFERENCE.md) - Complete API documentation
- [PERFORMANCE_BENCHMARK_GUIDE.md](PERFORMANCE_BENCHMARK_GUIDE.md) - Performance optimization
- [TESTING.md](TESTING.md) - Testing procedures and debugging
- [README.md](README.md) - Project overview and setup

---

**For additional troubleshooting support, visit the [GitHub repository](https://github.com/omerrbbulut/FreshLogger) or create an issue.**

---

*This troubleshooting guide helps resolve common issues and provides debugging techniques for FreshLogger.* 🔧✨ 