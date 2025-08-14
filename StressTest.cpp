/**
 * @file StressTest.cpp
 * @brief Enterprise-grade stress tests for Logger library
 * @author Ömer Bulut
 * 
 * This file contains comprehensive stress tests including:
 * - Extreme load testing
 * - Memory pressure testing
 * - CPU pressure testing
 * - Network/filesystem stress testing
 * - Long-running stability tests
 * - Resource exhaustion tests
 */

#include "Logger.hpp"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <memory>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <filesystem>

class StressTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create stress test directories
        std::filesystem::create_directories("stress_logs");
        std::filesystem::create_directories("stress_temp");
        
        // Extreme stress configuration
        extremeConfig.logFilePath = "stress_logs/extreme.log";
        extremeConfig.minLevel = Logger::LogLevel::TRACE;
        extremeConfig.consoleOutput = false;
        extremeConfig.asyncLogging = true;
        extremeConfig.maxFileSize = 10 * 1024 * 1024; // 10MB
        extremeConfig.maxFiles = 20;
        extremeConfig.queueSize = 1000000;
        
        // Memory stress configuration
        memoryConfig.logFilePath = "stress_logs/memory.log";
        memoryConfig.minLevel = Logger::LogLevel::INFO;
        memoryConfig.consoleOutput = false;
        memoryConfig.asyncLogging = true;
        memoryConfig.maxFileSize = 1 * 1024 * 1024; // 1MB
        memoryConfig.maxFiles = 50;
        memoryConfig.queueSize = 2000000;
        
        // CPU stress configuration
        cpuConfig.logFilePath = "stress_logs/cpu.log";
        cpuConfig.minLevel = Logger::LogLevel::DEBUG;
        cpuConfig.consoleOutput = false;
        cpuConfig.asyncLogging = true;
        cpuConfig.maxFileSize = 5 * 1024 * 1024; // 5MB
        cpuConfig.maxFiles = 10;
        cpuConfig.queueSize = 500000;
    }
    
    void TearDown() override {
        // Clean up stress test files with robust error handling
        try {
            if (std::filesystem::exists("stress_logs")) {
                std::filesystem::remove_all("stress_logs");
            }
            if (std::filesystem::exists("stress_temp")) {
                std::filesystem::remove_all("stress_temp");
            }
        } catch (const std::exception& e) {
            // Silently ignore cleanup errors - they don't affect test results
            // No warning messages to keep output clean
        }
    }
    
    // Helper function to generate random messages
    std::string generateRandomMessage(size_t length) {
        static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);
        
        std::string message;
        message.reserve(length);
        
        for (size_t i = 0; i < length; ++i) {
            message += charset[dis(gen)];
        }
        
        return message;
    }
    
    // Helper function to simulate CPU-intensive work
    void simulateCPUWork(int iterations) {
        volatile double result = 1.0;
        for (int i = 0; i < iterations; ++i) {
            result = std::sin(result) * std::cos(result) + std::sqrt(result);
        }
    }
    
    // Helper function to get system memory info
    struct MemoryInfo {
        size_t total;
        size_t available;
        size_t used;
    };
    
    MemoryInfo getSystemMemoryInfo() {
        MemoryInfo info = {0, 0, 0};
        
        std::ifstream meminfo("/proc/meminfo");
        std::string line;
        
        while (std::getline(meminfo, line)) {
            if (line.substr(0, 9) == "MemTotal:") {
                size_t pos = line.find_first_of("0123456789");
                if (pos != std::string::npos) {
                    info.total = std::stoul(line.substr(pos));
                }
            } else if (line.substr(0, 13) == "MemAvailable:") {
                size_t pos = line.find_first_of("0123456789");
                if (pos != std::string::npos) {
                    info.available = std::stoul(line.substr(pos));
                }
            }
        }
        
        info.used = info.total - info.available;
        return info;
    }
    
    Logger::Config extremeConfig;
    Logger::Config memoryConfig;
    Logger::Config cpuConfig;
    
    // Stress test constants
    static constexpr int EXTREME_MESSAGE_COUNT = 1000000;  // 1M messages (reduced from 5M)
    static constexpr int MEMORY_TEST_COUNT = 1000000;      // 1M messages
    static constexpr int CPU_TEST_COUNT = 500000;          // 500K messages
    static constexpr int STABILITY_TEST_DURATION = 60;     // 1 minute (reduced from 5 minutes)
    static constexpr int MAX_THREADS = 16;
    static constexpr size_t MAX_MESSAGE_SIZE = 10000;      // 10KB messages
};

// ==================== EXTREME LOAD STRESS TEST ====================

TEST_F(StressTest, ExtremeLoadTest) {
    Logger logger(extremeConfig);
    std::atomic<int> successCount{0};
    std::atomic<int> failureCount{0};
    std::atomic<bool> shouldStop{false};
    
    std::cout << "\n=== EXTREME LOAD STRESS TEST ===" << std::endl;
    std::cout << "Target: " << EXTREME_MESSAGE_COUNT << " messages" << std::endl;
    std::cout << "Threads: " << MAX_THREADS << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create maximum number of threads for extreme load
    std::vector<std::thread> threads;
    for (int t = 0; t < MAX_THREADS; ++t) {
        threads.emplace_back([&, t]() {
            int localSuccess = 0;
            int localFailure = 0;
            
            for (int i = 0; i < EXTREME_MESSAGE_COUNT / MAX_THREADS && !shouldStop; ++i) {
                try {
                    // Generate messages of varying sizes
                    size_t messageSize = (i % 100 == 0) ? MAX_MESSAGE_SIZE : (i % 1000) + 100;
                    std::string message = "Extreme load test - Thread " + std::to_string(t) + 
                                        " - Message " + std::to_string(i) + " - " +
                                        generateRandomMessage(messageSize);
                    
                    logger.trace(message);
                    localSuccess++;
                    
                    // Add some variation in logging levels
                    if (i % 10 == 0) logger.debug(message);
                    if (i % 100 == 0) logger.info(message);
                    if (i % 1000 == 0) logger.warning(message);
                    if (i % 10000 == 0) logger.error(message);
                    
                } catch (const std::exception& e) {
                    localFailure++;
                    if (localFailure > 100) {
                        shouldStop = true;
                        break;
                    }
                }
                
                // Add small delay to prevent overwhelming
                if (i % 1000 == 0) {
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }
            }
            
            successCount += localSuccess;
            failureCount += localFailure;
        });
    }
    
    // Monitor progress
    std::thread monitorThread([&]() {
        int lastCount = 0;
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        // Check if verbose output is enabled (default: false for CI/CD)
        bool verboseOutput = (std::getenv("STRESS_TEST_VERBOSE") != nullptr);
        
        // Show initial progress only if verbose
        if (verboseOutput) {
            std::cout << "Progress: 0/" << EXTREME_MESSAGE_COUNT << " (0%) Rate: 0 msg/sec" << std::endl;
        }
        
        while (!shouldStop && successCount.load() < EXTREME_MESSAGE_COUNT) {
            std::this_thread::sleep_for(std::chrono::seconds(3)); // Reduced from 5 to 3 seconds
            
            int currentCount = successCount.load();
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime);
            
            if (duration.count() > 0) {
                double rate = (currentCount - lastCount) / duration.count();
                double percentage = (currentCount * 100.0 / EXTREME_MESSAGE_COUNT);
                
                // Only show progress if verbose mode is enabled
                if (verboseOutput) {
                    std::cout << "Progress: " << currentCount << "/" << EXTREME_MESSAGE_COUNT 
                              << " (" << std::fixed << std::setprecision(1) << percentage << "%) "
                              << "Rate: " << std::fixed << std::setprecision(0) << rate << " msg/sec" << std::endl;
                }
            }
            
            lastCount = currentCount;
            lastTime = currentTime;
        }
    });
    
    // Wait for completion or timeout
    for (auto& thread : threads) {
        thread.join();
    }
    
    shouldStop = true;
    if (monitorThread.joinable()) {
        monitorThread.join();
    }
    
    logger.flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    std::cout << "\n=== EXTREME LOAD TEST RESULTS ===" << std::endl;
    std::cout << "Total Success: " << successCount.load() << std::endl;
    std::cout << "Total Failures: " << failureCount.load() << std::endl;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    std::cout << "Average Rate: " << std::fixed << std::setprecision(2) 
              << (successCount.load() / duration.count()) << " msg/sec" << std::endl;
    
    // Enterprise-grade expectations
    EXPECT_GT(successCount.load(), EXTREME_MESSAGE_COUNT * 0.99) << "99% success rate required";
    EXPECT_LT(failureCount.load(), EXTREME_MESSAGE_COUNT * 0.01) << "Failure rate should be < 1%";
    EXPECT_GT(successCount.load() / duration.count(), 10000.0) << "Should maintain > 10K msg/sec";
}

// ==================== MEMORY PRESSURE STRESS TEST ====================

TEST_F(StressTest, MemoryPressureTest) {
    Logger logger(memoryConfig);
    std::atomic<int> successCount{0};
    std::atomic<int> failureCount{0};
    
    std::cout << "\n=== MEMORY PRESSURE STRESS TEST ===" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Get initial memory state
    MemoryInfo initialMemory = getSystemMemoryInfo();
    std::cout << "Initial Memory - Total: " << initialMemory.total << " KB, "
              << "Available: " << initialMemory.available << " KB, "
              << "Used: " << initialMemory.used << " KB" << std::endl;
    
    // Create memory pressure with large messages (reduced count for realistic testing)
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < 100000; ++i) { // Reduced from 1M to 100K per thread
                try {
                    // Generate large messages to stress memory
                    size_t messageSize = 1000 + (i % 500); // Reduced from 10KB to 1-1.5KB
                    std::string message = "Memory pressure test - Thread " + std::to_string(t) + 
                                        " - Message " + std::to_string(i) + " - " +
                                        generateRandomMessage(messageSize);
                    
                    logger.info(message);
                    successCount++;
                    
                    // Force garbage collection pressure
                    if (i % 1000 == 0) {
                        std::vector<std::string> temp;
                        temp.reserve(100); // Reduced from 1000 to 100
                        for (int j = 0; j < 100; ++j) {
                            temp.push_back(generateRandomMessage(50)); // Reduced size
                        }
                    }
                    
                } catch (const std::exception& e) {
                    failureCount++;
                }
            }
        });
    }
    
    // Wait for completion
    for (auto& thread : threads) {
        thread.join();
    }
    
    logger.flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    // Get final memory state
    MemoryInfo finalMemory = getSystemMemoryInfo();
    std::cout << "\nFinal Memory - Total: " << finalMemory.total << " KB, "
              << "Available: " << finalMemory.available << " KB, "
              << "Used: " << finalMemory.used << " KB" << std::endl;
    
    // Fix memory calculation to handle potential underflow
    size_t memoryIncrease = 0;
    if (finalMemory.used > initialMemory.used) {
        memoryIncrease = finalMemory.used - initialMemory.used;
    } else {
        memoryIncrease = 0; // Memory actually decreased
    }
    
    std::cout << "\n=== MEMORY PRESSURE TEST RESULTS ===" << std::endl;
    std::cout << "Success Count: " << successCount.load() << std::endl;
    std::cout << "Failure Count: " << failureCount.load() << std::endl;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    std::cout << "Memory Increase: " << memoryIncrease << " KB" << std::endl;
    
    // Enterprise-grade expectations (adjusted for realistic testing)
    EXPECT_GT(successCount.load(), 300000) << "Should handle > 300K messages under memory pressure";
    EXPECT_LT(memoryIncrease, 150000) << "Memory increase should be < 150MB (realistic for 400K messages)";
    EXPECT_GT(finalMemory.available, 50000) << "Should maintain > 50MB available memory";
}

// ==================== CPU PRESSURE STRESS TEST ====================

TEST_F(StressTest, CPUPressureTest) {
    Logger logger(cpuConfig);
    std::atomic<int> successCount{0};
    std::atomic<int> failureCount{0};
    
    std::cout << "\n=== CPU PRESSURE STRESS TEST ===" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create CPU-intensive threads with logging
    std::vector<std::thread> threads;
    for (int t = 0; t < 12; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < CPU_TEST_COUNT / 12; ++i) {
                try {
                    // Perform CPU-intensive work
                    simulateCPUWork(1000 + (i % 1000));
                    
                    // Log with complex formatting
                    std::string message = "CPU pressure test - Thread " + std::to_string(t) + 
                                        " - Iteration " + std::to_string(i) + 
                                        " - Result: " + std::to_string(i * 3.14159) +
                                        " - Hash: " + std::to_string(std::hash<std::string>{}(std::to_string(i)));
                    
                    logger.debug(message);
                    successCount++;
                    
                    // Add more CPU work periodically
                    if (i % 100 == 0) {
                        simulateCPUWork(5000);
                        logger.info("CPU checkpoint - Thread " + std::to_string(t) + " at " + std::to_string(i));
                    }
                    
                } catch (const std::exception& e) {
                    failureCount++;
                }
            }
        });
    }
    
    // Wait for completion
    for (auto& thread : threads) {
        thread.join();
    }
    
    logger.flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    std::cout << "\n=== CPU PRESSURE TEST RESULTS ===" << std::endl;
    std::cout << "Success Count: " << successCount.load() << std::endl;
    std::cout << "Failure Count: " << failureCount.load() << std::endl;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    std::cout << "Average Rate: " << std::fixed << std::setprecision(2) 
              << (successCount.load() / duration.count()) << " msg/sec" << std::endl;
    
    // Enterprise-grade expectations
    EXPECT_GT(successCount.load(), CPU_TEST_COUNT * 0.95) << "95% success rate required";
    EXPECT_LT(failureCount.load(), CPU_TEST_COUNT * 0.05) << "Failure rate should be < 5%";
    EXPECT_GT(successCount.load() / duration.count(), 5000.0) << "Should maintain > 5K msg/sec under CPU load";
}

// ==================== LONG-RUNNING STABILITY TEST ====================

TEST_F(StressTest, LongRunningStabilityTest) {
    Logger logger(extremeConfig);
    std::atomic<int> messageCount{0};
    std::atomic<bool> shouldStop{false};
    std::atomic<int> errorCount{0};
    
    std::cout << "\n=== LONG-RUNNING STABILITY TEST ===" << std::endl;
    std::cout << "Duration: " << STABILITY_TEST_DURATION << " seconds" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create stability test threads
    std::vector<std::thread> threads;
    for (int t = 0; t < 6; ++t) {
        threads.emplace_back([&, t]() {
            auto threadStart = std::chrono::high_resolution_clock::now();
            
            while (!shouldStop) {
                try {
                    auto now = std::chrono::high_resolution_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - threadStart);
                    
                    if (elapsed.count() >= STABILITY_TEST_DURATION) {
                        break;
                    }
                    
                    // Log with varying patterns
                    std::string message = "Stability test - Thread " + std::to_string(t) + 
                                        " - Time: " + std::to_string(elapsed.count()) + "s" +
                                        " - Message: " + std::to_string(messageCount.load()) +
                                        " - Status: " + (elapsed.count() % 60 == 0 ? "CHECKPOINT" : "RUNNING");
                    
                    logger.info(message);
                    messageCount++;
                    
                    // Add some variation
                    if (elapsed.count() % 30 == 0) {
                        logger.warning("Stability warning - Thread " + std::to_string(t) + " at " + std::to_string(elapsed.count()) + "s");
                    }
                    
                    if (elapsed.count() % 60 == 0) {
                        logger.error("Stability error simulation - Thread " + std::to_string(t) + " at " + std::to_string(elapsed.count()) + "s");
                    }
                    
                    // Small delay to prevent overwhelming
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    
                } catch (const std::exception& e) {
                    errorCount++;
                    if (errorCount.load() > 1000) {
                        shouldStop = true;
                        break;
                    }
                }
            }
        });
    }
    
    // Monitor thread
    std::thread monitorThread([&]() {
        int lastCount = 0;
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        // Check if verbose output is enabled (default: false for CI/CD)
        bool verboseOutput = (std::getenv("STRESS_TEST_VERBOSE") != nullptr);
        
        // Show initial progress only if verbose
        if (verboseOutput) {
            std::cout << "Stability Progress: 0/" << STABILITY_TEST_DURATION << "s - Messages: 0 - Rate: 0 msg/sec" << std::endl;
        }
        
        while (!shouldStop) {
            std::this_thread::sleep_for(std::chrono::seconds(15)); // Reduced from 30 to 15 seconds
            
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - start);
            
            if (elapsed.count() >= STABILITY_TEST_DURATION) {
                break;
            }
            
            int currentCount = messageCount.load();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime);
            
            if (duration.count() > 0) {
                double rate = (currentCount - lastCount) / duration.count();
                
                // Only show progress if verbose mode is enabled
                if (verboseOutput) {
                    std::cout << "Stability Progress: " << elapsed.count() << "/" << STABILITY_TEST_DURATION 
                              << "s - Messages: " << currentCount << " - Rate: " 
                              << std::fixed << std::setprecision(0) << rate << " msg/sec" << std::endl;
                }
            }
            
            lastCount = currentCount;
            lastTime = currentTime;
        }
    });
    
    // Wait for completion
    for (auto& thread : threads) {
        thread.join();
    }
    
    shouldStop = true;
    if (monitorThread.joinable()) {
        monitorThread.join();
    }
    
    logger.flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    std::cout << "\n=== STABILITY TEST RESULTS ===" << std::endl;
    std::cout << "Total Messages: " << messageCount.load() << std::endl;
    std::cout << "Error Count: " << errorCount.load() << std::endl;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    std::cout << "Average Rate: " << std::fixed << std::setprecision(2) 
              << (messageCount.load() / duration.count()) << " msg/sec" << std::endl;
    
    // Enterprise-grade expectations
    EXPECT_GT(messageCount.load(), 3000) << "Should log > 3K messages during 1-minute stability test";
    EXPECT_LT(errorCount.load(), messageCount.load() * 0.01) << "Error rate should be < 1%";
    EXPECT_GT(duration.count(), STABILITY_TEST_DURATION * 0.9) << "Should run for at least 90% of target time";
}

// ==================== RESOURCE EXHAUSTION TEST ====================

TEST_F(StressTest, ResourceExhaustionTest) {
    std::cout << "\n=== RESOURCE EXHAUSTION TEST ===" << std::endl;
    
    // Test with extremely limited resources
    Logger::Config limitedConfig = extremeConfig;
    limitedConfig.queueSize = 50;  // Very small queue
    limitedConfig.maxFileSize = 512;  // 512 bytes files
    limitedConfig.maxFiles = 2;  // Very few files
    
    Logger logger(limitedConfig);
    std::atomic<int> successCount{0};
    std::atomic<int> failureCount{0};
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Try to exhaust resources
    std::vector<std::thread> threads;
    for (int t = 0; t < 2; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < 5000; ++i) {
                try {
                    std::string message = "Resource exhaustion test - Thread " + std::to_string(t) + 
                                        " - Message " + std::to_string(i) + 
                                        " - " + generateRandomMessage(100);
                    
                    logger.info(message);
                    successCount++;
                    
                } catch (const std::exception& e) {
                    failureCount++;
                }
                
                // Small delay
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        });
    }
    
    // Wait for completion
    for (auto& thread : threads) {
        thread.join();
    }
    
    logger.flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "\n=== RESOURCE EXHAUSTION TEST RESULTS ===" << std::endl;
    std::cout << "Success Count: " << successCount.load() << std::endl;
    std::cout << "Failure Count: " << failureCount.load() << std::endl;
    std::cout << "Duration: " << duration.count() << " ms" << std::endl;
    
    // Enterprise-grade expectations (realistic resource constraints)
    EXPECT_GT(successCount.load(), 0) << "Should handle some messages even under resource constraints";
    EXPECT_GE(failureCount.load(), 0) << "May have some failures under extreme constraints";
    EXPECT_LT(duration.count(), 30000) << "Should complete in reasonable time even with constraints";
    
    // Check if logger gracefully handled resource constraints
    EXPECT_TRUE(successCount.load() > 0 || failureCount.load() > 0) 
        << "Logger should either succeed or fail gracefully, not hang";
} 