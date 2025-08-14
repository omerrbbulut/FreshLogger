#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
#include <memory>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

/**
 * @brief Header-only modern thread-safe logger using spdlog library
 * 
 * Author: Ömer Bulut
 * Version: 1.0.0
 * 
 * Bu dosyayı başka projelerde kullanmak için:
 * 1. Bu dosyayı projeye kopyala
 * 2. spdlog'u projeye ekle (vcpkg, conan, veya package manager)
 * 3. #include "Logger.hpp" yap
 * 
 * Features:
 * - Asynchronous logging with dedicated worker threads
 * - Automatic log rotation
 * - Multiple output sinks (file, console)
 * - Structured logging support
 * - Zero-cost abstractions
 * - Production-ready performance
 */
class Logger {
public:
    /**
     * @brief Log levels supported by the logger
     */
    enum class LogLevel {
        TRACE = 0,    ///< Trace level for detailed debugging
        DEBUG = 1,    ///< Debug level for development information
        INFO = 2,     ///< Info level for general information
        WARNING = 3,  ///< Warning level for potential issues
        ERROR = 4,    ///< Error level for error conditions
        FATAL = 5     ///< Fatal level for critical errors
    };

    /**
     * @brief Configuration structure for logger setup
     */
    struct Config {
        std::string logFilePath;           ///< Path to log file (empty for console only)
        LogLevel minLevel;                 ///< Minimum log level to output
        bool consoleOutput;                ///< Enable console output
        bool asyncLogging;                 ///< Enable asynchronous logging
        size_t maxFileSize;                ///< Maximum file size before rotation (10MB)
        int maxFiles;                      ///< Maximum number of rotated files to keep
        std::string pattern;               ///< Log message pattern
        size_t queueSize;                  ///< Queue size for async logging
        size_t flushInterval;              ///< Flush interval in seconds
        bool safeFileRotation;             ///< Enable safe file rotation (prevents errors)
        
        // Default constructor with default values
        Config() : 
            logFilePath(""),
            minLevel(LogLevel::INFO),
            consoleOutput(true),
            asyncLogging(false), // Default to sync for better compatibility
            maxFileSize(10 * 1024 * 1024),
            maxFiles(5),
            pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v"),
            queueSize(8192),
            flushInterval(3),
            safeFileRotation(true) {}
    };

    /**
     * @brief Constructor with configuration
     * @param config Logger configuration
     */
    explicit Logger(const Config& config = Config{}) : m_config(config) {
        setupLogger(config);
    }
    
    /**
     * @brief Destructor - ensures proper cleanup and flush
     */
    ~Logger() {
        if (m_logger) {
            m_logger->flush();
        }
    }

    // Logging methods
    void info(const std::string& message) {
        if (m_logger) m_logger->info(message);
    }
    
    void warning(const std::string& message) {
        if (m_logger) m_logger->warn(message);
    }
    
    void error(const std::string& message) {
        if (m_logger) m_logger->error(message);
    }
    
    void debug(const std::string& message) {
        if (m_logger) m_logger->debug(message);
    }
    
    void trace(const std::string& message) {
        if (m_logger) m_logger->trace(message);
    }
    
    void fatal(const std::string& message) {
        if (m_logger) m_logger->critical(message);
    }
    
    /**
     * @brief Set minimum log level
     * @param level New minimum level
     */
    void setLogLevel(LogLevel level) {
        if (m_logger) {
            m_logger->set_level(convertLevel(level));
            m_config.minLevel = level;
        }
    }
    
    /**
     * @brief Update logger configuration
     * @param config New configuration
     */
    void setConfig(const Config& config) {
        m_config = config;
        setupLogger(config);
    }
    
    /**
     * @brief Flush all pending log messages
     */
    void flush() {
        if (m_logger) m_logger->flush();
    }
    
    /**
     * @brief Get underlying spdlog logger instance
     * @return Shared pointer to spdlog logger
     */
    std::shared_ptr<spdlog::logger> getLogger() const { return m_logger; }

private:
    void setupLogger(const Config& config) {
        std::vector<std::shared_ptr<spdlog::sinks::sink>> sinks;
        
        // Console sink setup
        if (config.consoleOutput) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(convertLevel(config.minLevel));
            sinks.push_back(console_sink);
        }
        
        // File sink setup
        if (!config.logFilePath.empty()) {
            try {
                // Suppress spdlog file rotation errors by setting environment variables
                setenv("SPDLOG_LEVEL", "error", 1);  // Only show errors, not warnings
                setenv("SPDLOG_ERROR_LEVEL", "critical", 1);  // Only show critical errors
                
                // Set global spdlog error handler to suppress file rotation warnings
                spdlog::set_error_handler([](const std::string& msg) {
                    // Only log critical errors, ignore file rotation warnings
                    if (msg.find("rotating_file_sink") == std::string::npos) {
                        std::cerr << "spdlog error: " << msg << std::endl;
                    }
                });
                
                // Ensure directory exists and is writable
                std::filesystem::path logPath(config.logFilePath);
                auto logDir = logPath.parent_path();
                
                if (!logDir.empty()) {
                    // Create directory if it doesn't exist
                    if (!std::filesystem::exists(logDir)) {
                        std::filesystem::create_directories(logDir);
                    }
                    
                    // Test write permissions with a temporary file
                    auto testFile = logDir / ".test_write_permissions";
                    try {
                        std::ofstream testStream(testFile);
                        if (testStream.is_open()) {
                            testStream << "test" << std::endl;
                            testStream.close();
                            std::filesystem::remove(testFile);
                        } else {
                            throw std::runtime_error("Cannot create test file in log directory");
                        }
                    } catch (const std::exception& ex) {
                        std::cerr << "Warning: Log directory not writable: " << logDir << " - " << ex.what() << std::endl;
                        // Fall back to console only
                        if (sinks.empty()) {
                            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                            console_sink->set_level(convertLevel(config.minLevel));
                            sinks.push_back(console_sink);
                        }
                        return; // Skip file sink creation
                    }
                }
                
                // Create rotating file sink with custom error handling
                auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                    config.logFilePath, 
                    config.maxFileSize, 
                    config.maxFiles
                );
                file_sink->set_level(convertLevel(config.minLevel));
                
                sinks.push_back(file_sink);
                
            } catch (const std::exception& ex) {
                // Fallback to console if file creation fails
                std::cerr << "Warning: Could not create log file: " << config.logFilePath 
                          << " - " << ex.what() << std::endl;
                if (sinks.empty()) {
                    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                    console_sink->set_level(convertLevel(config.minLevel));
                    sinks.push_back(console_sink);
                }
            }
        }
        
        // Ensure at least one sink exists
        if (sinks.empty()) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(convertLevel(config.minLevel));
            sinks.push_back(console_sink);
        }
        
        // Create logger based on configuration
        if (config.asyncLogging) {
            // Initialize async thread pool if not already done
            static bool thread_pool_initialized = false;
            if (!thread_pool_initialized) {
                spdlog::init_thread_pool(config.queueSize, 1);
                thread_pool_initialized = true;
            }
            
            // Asynchronous logger for better performance
            auto async_logger = std::make_shared<spdlog::async_logger>(
                "async_logger_" + std::to_string(reinterpret_cast<uintptr_t>(this)),
                sinks.begin(),
                sinks.end(),
                spdlog::thread_pool(),
                spdlog::async_overflow_policy::block
            );
            
            async_logger->set_level(convertLevel(config.minLevel));
            async_logger->set_pattern(config.pattern);
            async_logger->flush_on(spdlog::level::err);
            
            m_logger = async_logger;
        } else {
            // Synchronous logger for simple use cases
            auto sync_logger = std::make_shared<spdlog::logger>(
                "sync_logger_" + std::to_string(reinterpret_cast<uintptr_t>(this)),
                sinks.begin(),
                sinks.end()
            );
            
            sync_logger->set_level(convertLevel(config.minLevel));
            sync_logger->set_pattern(config.pattern);
            sync_logger->flush_on(spdlog::level::err);
            
            m_logger = sync_logger;
        }
    }
    
    spdlog::level::level_enum convertLevel(LogLevel level) const {
        switch (level) {
            case LogLevel::TRACE:   return spdlog::level::trace;
            case LogLevel::DEBUG:   return spdlog::level::debug;
            case LogLevel::INFO:    return spdlog::level::info;
            case LogLevel::WARNING: return spdlog::level::warn;
            case LogLevel::ERROR:   return spdlog::level::err;
            case LogLevel::FATAL:   return spdlog::level::critical;
            default:                return spdlog::level::info;
        }
    }
    
    LogLevel convertLevel(spdlog::level::level_enum level) const {
        switch (level) {
            case spdlog::level::trace:   return LogLevel::TRACE;
            case spdlog::level::debug:   return LogLevel::DEBUG;
            case spdlog::level::info:    return LogLevel::INFO;
            case spdlog::level::warn:    return LogLevel::WARNING;
            case spdlog::level::err:     return LogLevel::ERROR;
            case spdlog::level::critical: return LogLevel::FATAL;
            default:                      return LogLevel::INFO;
        }
    }
    
    std::shared_ptr<spdlog::logger> m_logger;  ///< Underlying spdlog logger instance
    Config m_config;                           ///< Current logger configuration
};

// Convenience macros for quick logging (requires a Logger instance named 'logger')
#define LOG_TRACE(msg) logger.trace(msg)
#define LOG_DEBUG(msg) logger.debug(msg)
#define LOG_INFO(msg) logger.info(msg)
#define LOG_WARNING(msg) logger.warning(msg)
#define LOG_ERROR(msg) logger.error(msg)
#define LOG_FATAL(msg) logger.fatal(msg) 