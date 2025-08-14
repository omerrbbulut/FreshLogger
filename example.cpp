/**
 * @file example.cpp
 * @brief Example usage of the Logger library
 * @author Omer Bulut
 */

#include "Logger.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

int main() {
    std::cout << "🚀 Logger Library Example" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // Basic logger usage
    Logger logger;
    logger.info("Logger initialized successfully!");
    
    // Different log levels
    logger.info("This is an info message");
    logger.warning("This is a warning message");
    logger.error("This is an error message");
    logger.fatal("This is a fatal message");
    
    // Advanced logger with file output
    Logger::Config config;
    config.logFilePath = "logs/example.log";
    config.minLevel = Logger::LogLevel::DEBUG;
    config.consoleOutput = false;
    config.asyncLogging = true;
    
    Logger advancedLogger(config);
    advancedLogger.info("Advanced logger with file output created");
    
    // Multi-threaded logging example
    std::vector<std::thread> threads;
    const int numThreads = 4;
    const int messagesPerThread = 10;
    
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&advancedLogger, t, messagesPerThread]() {
            for (int i = 0; i < messagesPerThread; ++i) {
                advancedLogger.info("Thread " + std::to_string(t) + " - Message " + std::to_string(i));
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Flush to ensure all messages are written
    advancedLogger.flush();
    
    std::cout << "\n✅ Example completed successfully!" << std::endl;
    std::cout << "Check the logs/example.log file for output." << std::endl;
    
    return 0;
} 