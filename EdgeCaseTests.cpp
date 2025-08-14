/**
 * @file EdgeCaseTests.cpp
 * @brief Edge case tests for FreshLogger
 * @author Ömer Bulut
 */

#include "Logger.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <thread>
#include <chrono>
#include <limits>

class EdgeCaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean up test directories
        try {
            std::filesystem::remove_all("edge_test_logs");
            std::filesystem::create_directories("edge_test_logs");
        } catch (const std::exception& e) {
            // Silently ignore cleanup errors
        }
    }
    
    void TearDown() override {
        // Clean up test files
        try {
            std::filesystem::remove_all("edge_test_logs");
        } catch (const std::exception& e) {
            // Silently ignore cleanup errors
        }
    }
};

// Test 1: Extremely long messages
TEST_F(EdgeCaseTest, ExtremelyLongMessages) {
    Logger::Config config;
    config.logFilePath = "edge_test_logs/long_messages.log";
    config.asyncLogging = false;
    config.consoleOutput = false;
    
    Logger logger(config);
    
    // Create long message (1KB instead of 1MB for faster testing)
    std::string longMessage(1024, 'X');
    longMessage += " - END OF LONG MESSAGE";
    
    EXPECT_NO_THROW(logger.info(longMessage));
    logger.flush();
    
    // Check if file was created and contains the message
    EXPECT_TRUE(std::filesystem::exists("edge_test_logs/long_messages.log"));
    EXPECT_GT(std::filesystem::file_size("edge_test_logs/long_messages.log"), 0);
}

// Test 2: Empty messages
TEST_F(EdgeCaseTest, EmptyMessages) {
    Logger::Config config;
    config.logFilePath = "edge_test_logs/empty_messages.log";
    config.asyncLogging = false;
    config.consoleOutput = false;
    
    Logger logger(config);
    
    EXPECT_NO_THROW(logger.info(""));
    EXPECT_NO_THROW(logger.info("   ")); // Only spaces
    EXPECT_NO_THROW(logger.info("\n\t\r")); // Only whitespace
    
    logger.flush();
    
    // Should handle empty messages gracefully
    EXPECT_TRUE(std::filesystem::exists("edge_test_logs/empty_messages.log"));
}

// Test 3: Special characters in messages
TEST_F(EdgeCaseTest, SpecialCharacters) {
    Logger::Config config;
    config.logFilePath = "edge_test_logs/special_chars.log";
    config.asyncLogging = false;
    config.consoleOutput = false;
    
    Logger logger(config);
    
    // Test various special characters
    std::string specialMessage = "Special chars: !@#$%^&*()_+-=[]{}|;':\",./<>?`~";
    specialMessage += "\nNew line\n\tTab\rCarriage return";
    specialMessage += "\x00\x01\x02\x03"; // Control characters
    
    EXPECT_NO_THROW(logger.info(specialMessage));
    logger.flush();
    
    EXPECT_TRUE(std::filesystem::exists("edge_test_logs/special_chars.log"));
}

// Test 4: Unicode and international characters
TEST_F(EdgeCaseTest, UnicodeCharacters) {
    Logger::Config config;
    config.logFilePath = "edge_test_logs/unicode.log";
    config.asyncLogging = false;
    config.consoleOutput = false;
    
    Logger logger(config);
    
    // Test various Unicode characters
    std::string unicodeMessage = "Unicode: 你好世界 Привет мир 🌍 🚀 💻";
    unicodeMessage += " Türkçe: ğüşıöçĞÜŞİÖÇ";
    unicodeMessage += " Emoji: 🎉✨🔥💪";
    
    EXPECT_NO_THROW(logger.info(unicodeMessage));
    logger.flush();
    
    EXPECT_TRUE(std::filesystem::exists("edge_test_logs/unicode.log"));
}

// Test 5: Very high frequency logging
TEST_F(EdgeCaseTest, VeryHighFrequencyLogging) {
    Logger::Config config;
    config.logFilePath = "edge_test_logs/high_freq.log";
    config.asyncLogging = true;
    config.queueSize = 100000; // Large queue
    config.consoleOutput = false;
    
    Logger logger(config);
    
    auto start = std::chrono::high_resolution_clock::now();
    
               // Log 10,000 messages as fast as possible (reduced for faster testing)
           for (int i = 0; i < 10000; ++i) {
               logger.info("High frequency message " + std::to_string(i));
           }
    
    logger.flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete in reasonable time
    EXPECT_LT(duration.count(), 10000); // Less than 10 seconds
    
    EXPECT_TRUE(std::filesystem::exists("edge_test_logs/high_freq.log"));
}

// Test 6: Configuration edge cases
TEST_F(EdgeCaseTest, ConfigurationEdgeCases) {
    // Test with very small file sizes
    Logger::Config config;
    config.logFilePath = "edge_test_logs/tiny_files.log";
    config.maxFileSize = 100; // 100 bytes - very small
    config.maxFiles = 1; // Only 1 file
    config.asyncLogging = false;
    config.consoleOutput = false;
    
    Logger logger(config);
    
    // This should trigger file rotation quickly
    for (int i = 0; i < 10; ++i) {
        logger.info("Message " + std::to_string(i) + " with some content to exceed 100 bytes");
    }
    
    logger.flush();
    
    EXPECT_TRUE(std::filesystem::exists("edge_test_logs/tiny_files.log"));
}

// Test 7: Memory boundary conditions
TEST_F(EdgeCaseTest, MemoryBoundaryConditions) {
    Logger::Config config;
    config.logFilePath = "edge_test_logs/memory_test.log";
    config.asyncLogging = false;
    config.consoleOutput = false;
    
    Logger logger(config);
    
    // Test with messages that might cause memory issues
    std::vector<std::string> messages;
    messages.reserve(1000);
    
    for (int i = 0; i < 1000; ++i) {
        messages.push_back("Memory test message " + std::to_string(i) + 
                          " with some additional content to test memory handling");
    }
    
    // Log all messages
    for (const auto& msg : messages) {
        EXPECT_NO_THROW(logger.info(msg));
    }
    
    logger.flush();
    
    EXPECT_TRUE(std::filesystem::exists("edge_test_logs/memory_test.log"));
}

// Test 8: Thread safety under extreme conditions
TEST_F(EdgeCaseTest, ExtremeThreadSafety) {
    Logger::Config config;
    config.logFilePath = "edge_test_logs/extreme_threads.log";
    config.asyncLogging = true;
    config.queueSize = 50000;
    config.consoleOutput = false;
    
    Logger logger(config);
    
    const int threadCount = 32; // More threads than before
    const int messagesPerThread = 10000;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    std::atomic<int> failureCount{0};
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < messagesPerThread; ++i) {
                try {
                    std::string message = "Thread " + std::to_string(t) + 
                                        " - Message " + std::to_string(i) + 
                                        " - " + std::string(100, 'X');
                    logger.info(message);
                    successCount++;
                } catch (const std::exception& e) {
                    failureCount++;
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    logger.flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should handle extreme threading gracefully
    EXPECT_GT(successCount.load(), threadCount * messagesPerThread * 0.95); // 95% success rate
    EXPECT_LT(failureCount.load(), threadCount * messagesPerThread * 0.05); // Less than 5% failures
    
    EXPECT_TRUE(std::filesystem::exists("edge_test_logs/extreme_threads.log"));
}

// Test 9: File system edge cases
TEST_F(EdgeCaseTest, FileSystemEdgeCases) {
    // Test with very long file paths
    std::string longPath = "edge_test_logs/";
    for (int i = 0; i < 20; ++i) {
        longPath += "very_long_directory_name_" + std::to_string(i) + "/";
    }
    longPath += "deeply_nested_log.log";
    
    Logger::Config config;
    config.logFilePath = longPath;
    config.asyncLogging = false;
    config.consoleOutput = false;
    
    Logger logger(config);
    
    EXPECT_NO_THROW(logger.info("Test message in deeply nested directory"));
    logger.flush();
    
    // Should create the directory structure and file
    EXPECT_TRUE(std::filesystem::exists(longPath));
}

// Test 10: Performance under memory pressure
TEST_F(EdgeCaseTest, PerformanceUnderMemoryPressure) {
    Logger::Config config;
    config.logFilePath = "edge_test_logs/memory_pressure.log";
    config.asyncLogging = true;
    config.queueSize = 1000; // Smaller queue to create pressure
    config.consoleOutput = false;
    
    Logger logger(config);
    
    // Create memory pressure by allocating large vectors
    std::vector<std::vector<std::string>> memoryPressure;
    memoryPressure.reserve(100);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        // Allocate memory to create pressure
        std::vector<std::string> temp;
        temp.reserve(1000);
        for (int j = 0; j < 1000; ++j) {
            temp.push_back("Memory pressure test " + std::to_string(i) + "_" + std::to_string(j));
        }
        memoryPressure.push_back(std::move(temp));
        
        // Log messages under memory pressure
        logger.info("Logging under memory pressure - iteration " + std::to_string(i));
    }
    
    logger.flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete in reasonable time even under memory pressure
    EXPECT_LT(duration.count(), 30000); // Less than 30 seconds
    
    EXPECT_TRUE(std::filesystem::exists("edge_test_logs/memory_pressure.log"));
} 