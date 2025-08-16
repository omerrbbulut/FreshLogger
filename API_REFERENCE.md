# 📚 FreshLogger API Reference

**Author:** Ömer Bulut  
**Version:** 1.2.0  
**Last Updated:** August 16, 2025

---

## 📋 Table of Contents

1. [Core Classes](#core-classes)
2. [Configuration](#configuration)
3. [Logging Methods](#logging-methods)
4. [Utility Methods](#utility-methods)
5. [Convenience Macros](#convenience-macros)
6. [Error Handling](#error-handling)
7. [Examples](#examples)

---

## 🏗️ Core Classes

### `Logger` Class

The main logging class that provides all logging functionality.

```cpp
class Logger {
public:
    // Constructors
    Logger();
    Logger(const Config& config);
    
    // Destructor
    ~Logger();
    
    // Logging methods
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);
    
    // Utility methods
    void flush();
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    std::shared_ptr<spdlog::logger> getLogger() const;
    
private:
    std::shared_ptr<spdlog::logger> m_logger;
    Config m_config;
};
```

---

## ⚙️ Configuration

### `Config` Struct

Configuration structure for logger setup.

```cpp
struct Config {
    // File configuration
    std::string logFilePath;           // Path to log file
    int maxFiles;                      // Maximum number of log files
    size_t maxFileSize;                // Maximum size per log file (bytes)
    
    // Logging configuration
    LogLevel minLevel;                 // Minimum log level
    bool consoleOutput;                // Enable console output
    bool asyncLogging;                 // Enable asynchronous logging
    
    // Performance configuration
    size_t queueSize;                  // Async queue size
    size_t flushInterval;              // Flush interval (seconds)
    
    // Formatting configuration
    std::string pattern;               // Log message pattern
    
    // Constructor with defaults
    Config();
};
```

### `LogLevel` Enum

Available log levels.

```cpp
enum class LogLevel {
    TRACE = 0,     // Most verbose
    DEBUG = 1,     // Debug information
    INFO = 2,      // General information
    WARNING = 3,   // Warning messages
    ERROR = 4,     // Error messages
    FATAL = 5      // Fatal errors (least verbose)
};
```

---

## 📝 Logging Methods

### Basic Logging

#### `trace(const std::string& message)`
Logs a trace-level message.

**Parameters:**
- `message` - The message to log

**Example:**
```cpp
Logger logger;
logger.trace("Entering function");
```

#### `debug(const std::string& message)`
Logs a debug-level message.

**Parameters:**
- `message` - The message to log

**Example:**
```cpp
Logger logger;
logger.debug("Variable value: " + std::to_string(value));
```

#### `info(const std::string& message)`
Logs an info-level message.

**Parameters:**
- `message` - The message to log

**Example:**
```cpp
Logger logger;
logger.info("Application started successfully");
```

#### `warning(const std::string& message)`
Logs a warning-level message.

**Parameters:**
- `message` - The message to log

**Example:**
```cpp
Logger logger;
logger.warning("High memory usage detected");
```

#### `error(const std::string& message)`
Logs an error-level message.

**Parameters:**
- `message` - The message to log

**Example:**
```cpp
Logger logger;
logger.error("Failed to connect to database");
```

#### `fatal(const std::string& message)`
Logs a fatal-level message.

**Parameters:**
- `message` - The message to log

**Example:**
```cpp
Logger logger;
logger.fatal("Critical system failure - shutting down");
```

---

## 🔧 Utility Methods

### `flush()`
Flushes all pending log messages to output.

**Parameters:** None

**Return Value:** void

**Example:**
```cpp
Logger logger;
logger.info("Important message");
logger.flush(); // Ensure message is written immediately
```

### `setLogLevel(LogLevel level)`
Sets the minimum log level for the logger.

**Parameters:**
- `level` - The new minimum log level

**Return Value:** void

**Example:**
```cpp
Logger logger;
logger.setLogLevel(Logger::LogLevel::WARNING);
// Only WARNING, ERROR, and FATAL messages will be logged
```

### `getLogLevel() const`
Gets the current minimum log level.

**Parameters:** None

**Return Value:** `LogLevel` - Current minimum log level

**Example:**
```cpp
Logger logger;
LogLevel currentLevel = logger.getLogLevel();
if (currentLevel == Logger::LogLevel::DEBUG) {
    // Enable verbose logging
}
```

### `getLogger() const`
Gets the underlying spdlog logger instance.

**Parameters:** None

**Return Value:** `std::shared_ptr<spdlog::logger>` - spdlog logger instance

**Example:**
```cpp
Logger logger;
auto spdlogLogger = logger.getLogger();
// Access advanced spdlog features if needed
```

---

## 🚀 Convenience Macros

### Global Logger Macros

These macros provide convenient logging without creating logger instances.

```cpp
// Global logger instance (must be defined)
extern Logger g_logger;

// Convenience macros
LOG_TRACE(message)      // Logs trace message
LOG_DEBUG(message)      // Logs debug message
LOG_INFO(message)       // Logs info message
LOG_WARNING(message)    // Logs warning message
LOG_ERROR(message)      // Logs error message
LOG_FATAL(message)      // Logs fatal message
```

**Example:**
```cpp
#include "Logger.hpp"

// Define global logger
Logger g_logger;

void someFunction() {
    LOG_INFO("Function called");
    LOG_DEBUG("Processing data...");
    
    if (error_condition) {
        LOG_ERROR("Error occurred");
    }
}
```

---

## ⚠️ Error Handling

### Exception Safety

All logging methods provide strong exception safety guarantees:

- **No-throw guarantee**: Logging methods never throw exceptions
- **Data integrity**: Failed logging doesn't corrupt internal state
- **Graceful degradation**: Logger continues working even if individual messages fail

### Error Scenarios

#### File System Errors
```cpp
Logger logger(Config{"app.log"});

// If log directory doesn't exist, logger will:
// 1. Attempt to create directory
// 2. Fall back to console output if file creation fails
// 3. Continue logging without interruption
```

#### Memory Pressure
```cpp
// Under memory pressure:
// 1. Logger reduces internal buffer sizes
// 2. Continues logging with reduced performance
// 3. Never crashes or throws exceptions
```

#### Invalid Configuration
```cpp
Config config;
config.maxFileSize = 0; // Invalid value

// Logger will:
// 1. Use default values for invalid parameters
// 2. Log warning about invalid configuration
// 3. Continue with safe defaults
```

---

## 💡 Examples

### Basic Usage

```cpp
#include "Logger.hpp"

int main() {
    // Default logger (console output only)
    Logger logger;
    logger.info("Application started");
    
    // Custom configuration
    Logger::Config config;
    config.logFilePath = "app.log";
    config.minLevel = Logger::LogLevel::DEBUG;
    config.asyncLogging = true;
    config.maxFileSize = 10 * 1024 * 1024; // 10MB
    config.maxFiles = 5;
    
    Logger customLogger(config);
    customLogger.debug("Debug message");
    customLogger.info("Info message");
    customLogger.warning("Warning message");
    
    // Ensure all messages are written
    customLogger.flush();
    
    return 0;
}
```

### Multi-threaded Usage

```cpp
#include "Logger.hpp"
#include <thread>
#include <vector>

void workerThread(Logger& logger, int threadId) {
    for (int i = 0; i < 1000; ++i) {
        logger.info("Thread " + std::to_string(threadId) + 
                   " - Message " + std::to_string(i));
    }
}

int main() {
    Logger::Config config;
    config.logFilePath = "multithread.log";
    config.asyncLogging = true;
    config.queueSize = 10000;
    
    Logger logger(config);
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(workerThread, std::ref(logger), i);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    logger.flush();
    return 0;
}
```

### Performance Monitoring

```cpp
#include "Logger.hpp"
#include <chrono>

class PerformanceMonitor {
private:
    Logger logger;
    std::chrono::high_resolution_clock::time_point startTime;
    
public:
    PerformanceMonitor() : logger(Logger::Config{"performance.log"}) {}
    
    void startOperation(const std::string& operation) {
        startTime = std::chrono::high_resolution_clock::now();
        logger.debug("Started: " + operation);
    }
    
    void endOperation(const std::string& operation) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>
                       (endTime - startTime);
        
        logger.info("Completed: " + operation + 
                   " in " + std::to_string(duration.count()) + " μs");
    }
};
```

---

## 🔍 Advanced Usage

### Custom Pattern Formatting

```cpp
Logger::Config config;
config.pattern = "[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] [%s:%#] %v";

// Pattern elements:
// %Y-%m-%d     - Date (YYYY-MM-DD)
// %H:%M:%S.%e  - Time with milliseconds
// %l           - Log level
// %t           - Thread ID
// %s:%#        - Source file and line number
// %v           - Message content
```

### Dynamic Configuration Changes

```cpp
Logger logger;

// Change log level at runtime
logger.setLogLevel(Logger::LogLevel::DEBUG);

// Log some debug messages
logger.debug("Debug message 1");
logger.debug("Debug message 2");

// Switch back to production level
logger.setLogLevel(Logger::LogLevel::WARNING);

// These won't be logged
logger.debug("Debug message 3"); // Ignored
logger.info("Info message");     // Ignored

// These will be logged
logger.warning("Warning message"); // Logged
logger.error("Error message");     // Logged
```

---

## 📊 Performance Characteristics

### Throughput
- **Synchronous**: 2.4M+ messages/second
- **Asynchronous**: 4.1M+ messages/second
- **Multi-threaded**: 1.2M+ messages/second (8 threads)

### Latency
- **Average**: <1 μs per message
- **95th percentile**: <5 μs
- **Maximum**: <10 μs

### Memory Usage
- **Base overhead**: <1MB
- **Per message**: <100 bytes
- **Queue overhead**: <1KB per 1000 messages

---

## 🔗 Related Documentation

- [README.md](README.md) - Project overview and installation
- [TESTING.md](TESTING.md) - Testing guide and benchmarks
- [TECHNICAL_ANALYSIS.md](TECHNICAL_ANALYSIS.md) - Technical details
- [CONTRIBUTING.md](CONTRIBUTING.md) - Development guidelines

---

**For additional support and examples, visit the [GitHub repository](https://github.com/omerrbbulut/FreshLogger).**

---

*This API reference covers all public interfaces of FreshLogger. For internal implementation details, see the source code.* 📚✨ 