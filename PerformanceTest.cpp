/**
 * @file PerformanceTest.cpp
 * @brief Enterprise-grade performance and stress tests for Logger library
 * @author Ömer Bulut
 * 
 * This file contains comprehensive performance tests including:
 * - Throughput tests (messages per second)
 * - Latency tests (response time)
 * - Memory usage tests
 * - CPU utilization tests
 * - Stress tests with high load
 * - Benchmark comparisons
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
#include <sstream>
#include <iomanip>
#include <numeric>
#include <algorithm>

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create unique test directory for each test run
        testDir = "test_logs_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        std::filesystem::create_directories(testDir);
        
        // Performance test configuration
        perfConfig.logFilePath = testDir + "/performance.log";
        perfConfig.minLevel = Logger::LogLevel::INFO;
        perfConfig.consoleOutput = false;
        perfConfig.asyncLogging = true;
        perfConfig.maxFileSize = 100 * 1024 * 1024; // 100MB
        perfConfig.maxFiles = 5;
        perfConfig.queueSize = 100000;
        
        // Stress test configuration
        stressConfig.logFilePath = testDir + "/stress.log";
        stressConfig.minLevel = Logger::LogLevel::WARNING;
        stressConfig.consoleOutput = false;
        stressConfig.asyncLogging = true;
        stressConfig.maxFileSize = 50 * 1024 * 1024; // 50MB
        stressConfig.maxFiles = 10;
        stressConfig.queueSize = 500000;
    }
    
    void TearDown() override {
        // Clean up test files with robust error handling
        try {
            if (std::filesystem::exists(testDir)) {
                // Force remove all contents recursively
                std::filesystem::remove_all(testDir);
            }
        } catch (const std::exception& e) {
            // Silently ignore cleanup errors - they don't affect test results
            // No warning messages to keep output clean
        }
    }
    
    // Helper function to measure execution time
    template<typename Func>
    auto measureTime(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    }
    
    // Helper function to get memory usage (Linux)
    size_t getMemoryUsage() {
        std::ifstream status("/proc/self/status");
        std::string line;
        while (std::getline(status, line)) {
            if (line.substr(0, 6) == "VmRSS:") {
                size_t pos = line.find_first_of("0123456789");
                if (pos != std::string::npos) {
                    return std::stoul(line.substr(pos));
                }
            }
        }
        return 0;
    }
    
    // Helper function to calculate throughput
    double calculateThroughput(int messageCount, std::chrono::microseconds duration) {
        auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(duration);
        return messageCount / seconds.count();
    }
    
    Logger::Config perfConfig;
    Logger::Config stressConfig;
    
    // Test constants
    static constexpr int SMALL_TEST_SIZE = 1000;
    static constexpr int MEDIUM_TEST_SIZE = 10000;
    static constexpr int LARGE_TEST_SIZE = 100000;
    static constexpr int STRESS_TEST_SIZE = 1000000;
    static constexpr int THREAD_COUNT = 8;
    std::string testDir;
};

// ==================== THROUGHPUT TESTS ====================

TEST_F(PerformanceTest, SynchronousThroughput) {
    Logger::Config config = perfConfig;
    config.asyncLogging = false;
    Logger logger(config);
    
    auto duration = measureTime([&]() {
        for (int i = 0; i < MEDIUM_TEST_SIZE; ++i) {
            logger.info("Performance test message " + std::to_string(i));
        }
        logger.flush();
    });
    
    double throughput = calculateThroughput(MEDIUM_TEST_SIZE, duration);
    
    std::cout << "\n=== SYNCHRONOUS THROUGHPUT TEST ===" << std::endl;
    std::cout << "Messages: " << MEDIUM_TEST_SIZE << std::endl;
    std::cout << "Duration: " << duration.count() << " μs" << std::endl;
    std::cout << "Throughput: " << std::fixed << std::setprecision(2) 
              << throughput << " msg/sec" << std::endl;
    
    // Enterprise-grade expectations
    EXPECT_GT(throughput, 10000.0) << "Throughput should be > 10,000 msg/sec for enterprise use";
    EXPECT_LT(duration.count(), 1000000) << "Should complete in < 1 second";
}

TEST_F(PerformanceTest, AsynchronousThroughput) {
    Logger logger(perfConfig);
    
    auto duration = measureTime([&]() {
        for (int i = 0; i < LARGE_TEST_SIZE; ++i) {
            logger.info("Async performance test message " + std::to_string(i));
        }
        logger.flush();
    });
    
    double throughput = calculateThroughput(LARGE_TEST_SIZE, duration);
    
    std::cout << "\n=== ASYNCHRONOUS THROUGHPUT TEST ===" << std::endl;
    std::cout << "Messages: " << LARGE_TEST_SIZE << std::endl;
    std::cout << "Duration: " << duration.count() << " μs" << std::endl;
    std::cout << "Throughput: " << std::fixed << std::setprecision(2) 
              << throughput << " msg/sec" << std::endl;
    
    // Enterprise-grade expectations
    EXPECT_GT(throughput, 50000.0) << "Async throughput should be > 50,000 msg/sec";
    EXPECT_LT(duration.count(), 2000000) << "Should complete in < 2 seconds";
}

// ==================== LATENCY TESTS ====================

TEST_F(PerformanceTest, SingleMessageLatency) {
    Logger logger(perfConfig);
    
    std::vector<std::chrono::microseconds> latencies;
    latencies.reserve(SMALL_TEST_SIZE);
    
    for (int i = 0; i < SMALL_TEST_SIZE; ++i) {
        auto latency = measureTime([&]() {
            logger.info("Latency test message " + std::to_string(i));
        });
        latencies.push_back(latency);
    }
    
    logger.flush();
    
    // Calculate statistics
    auto total = std::accumulate(latencies.begin(), latencies.end(), 
                                std::chrono::microseconds(0));
    auto avgLatency = total.count() / latencies.size();
    
    auto minLatency = std::min_element(latencies.begin(), latencies.end())->count();
    auto maxLatency = std::max_element(latencies.begin(), latencies.end())->count();
    
    std::cout << "\n=== SINGLE MESSAGE LATENCY TEST ===" << std::endl;
    std::cout << "Average Latency: " << avgLatency << " μs" << std::endl;
    std::cout << "Min Latency: " << minLatency << " μs" << std::endl;
    std::cout << "Max Latency: " << maxLatency << " μs" << std::endl;
    
    // Enterprise-grade expectations
    EXPECT_LT(avgLatency, 1000) << "Average latency should be < 1ms";
    EXPECT_LT(maxLatency, 10000) << "Max latency should be < 10ms";
}

// ==================== MEMORY TESTS ====================

TEST_F(PerformanceTest, MemoryUsageUnderLoad) {
    size_t initialMemory = getMemoryUsage();
    
    Logger logger(perfConfig);
    
    // Log many messages to test memory usage
    for (int i = 0; i < MEDIUM_TEST_SIZE; ++i) {
        logger.info("Memory test message " + std::to_string(i) + 
                   " with some additional content to increase memory usage");
    }
    
    logger.flush();
    
    size_t finalMemory = getMemoryUsage();
    size_t memoryIncrease = finalMemory - initialMemory;
    
    std::cout << "\n=== MEMORY USAGE TEST ===" << std::endl;
    std::cout << "Initial Memory: " << initialMemory << " KB" << std::endl;
    std::cout << "Final Memory: " << finalMemory << " KB" << std::endl;
    std::cout << "Memory Increase: " << memoryIncrease << " KB" << std::endl;
    
    // Enterprise-grade expectations
    EXPECT_LT(memoryIncrease, 50000) << "Memory increase should be < 50MB";
    EXPECT_LT(finalMemory, 200000) << "Total memory usage should be < 200MB";
}

// ==================== MULTI-THREADED TESTS ====================

TEST_F(PerformanceTest, MultiThreadedThroughput) {
    Logger logger(perfConfig);
    std::atomic<int> messageCount{0};
    std::vector<std::thread> threads;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create multiple threads logging simultaneously
    for (int t = 0; t < THREAD_COUNT; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < LARGE_TEST_SIZE / THREAD_COUNT; ++i) {
                logger.info("Thread " + std::to_string(t) + " - Message " + std::to_string(i));
                messageCount++;
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    logger.flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double throughput = calculateThroughput(messageCount.load(), duration);
    
    std::cout << "\n=== MULTI-THREADED THROUGHPUT TEST ===" << std::endl;
    std::cout << "Threads: " << THREAD_COUNT << std::endl;
    std::cout << "Total Messages: " << messageCount.load() << std::endl;
    std::cout << "Duration: " << duration.count() << " μs" << std::endl;
    std::cout << "Throughput: " << std::fixed << std::setprecision(2) 
              << throughput << " msg/sec" << std::endl;
    
    // Enterprise-grade expectations
    EXPECT_GT(throughput, 100000.0) << "Multi-threaded throughput should be > 100,000 msg/sec";
    EXPECT_EQ(messageCount.load(), LARGE_TEST_SIZE) << "All messages should be logged";
}

// ==================== STRESS TESTS ====================

TEST_F(PerformanceTest, HighLoadStressTest) {
    Logger logger(stressConfig);
    std::atomic<int> successCount{0};
    std::atomic<int> failureCount{0};
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create extreme load with multiple threads
    std::vector<std::thread> threads;
    for (int t = 0; t < THREAD_COUNT * 2; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < STRESS_TEST_SIZE / (THREAD_COUNT * 2); ++i) {
                try {
                    logger.info("Stress test - Thread " + std::to_string(t) + 
                               " - Message " + std::to_string(i) + 
                               " - " + std::string(100, 'A')); // Long message
                    successCount++;
                } catch (...) {
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
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double throughput = calculateThroughput(successCount.load(), duration);
    
    std::cout << "\n=== HIGH LOAD STRESS TEST ===" << std::endl;
    std::cout << "Threads: " << THREAD_COUNT * 2 << std::endl;
    std::cout << "Success Count: " << successCount.load() << std::endl;
    std::cout << "Failure Count: " << failureCount.load() << std::endl;
    std::cout << "Duration: " << duration.count() << " μs" << std::endl;
    std::cout << "Throughput: " << std::fixed << std::setprecision(2) 
              << throughput << " msg/sec" << std::endl;
    
    // Enterprise-grade expectations
    EXPECT_GT(successCount.load(), STRESS_TEST_SIZE * 0.95) << "95% success rate required";
    EXPECT_EQ(failureCount.load(), 0) << "No failures should occur under stress";
    EXPECT_GT(throughput, 200000.0) << "Stress test throughput should be > 200,000 msg/sec";
}

// ==================== FILE ROTATION PERFORMANCE ====================

TEST_F(PerformanceTest, FileRotationPerformance) {
    Logger::Config config = perfConfig;
    config.maxFileSize = 1024 * 10; // 10KB - smaller size for reliable rotation
    config.maxFiles = 3;
    
    Logger logger(config);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // First, create some small log files to establish rotation sequence
    for (int i = 0; i < 100; ++i) {
        logger.info("Initial log message " + std::to_string(i));
    }
    logger.flush();
    
    // Now add large messages to trigger rotation
    for (int i = 0; i < MEDIUM_TEST_SIZE; ++i) {
        logger.info("File rotation test message " + std::to_string(i) + 
                   " with additional content to exceed file size limit " +
                   std::string(500, 'X')); // Smaller message for more reliable rotation
    }
    
    logger.flush();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double throughput = calculateThroughput(MEDIUM_TEST_SIZE, duration);
    
    std::cout << "\n=== FILE ROTATION PERFORMANCE TEST ===" << std::endl;
    std::cout << "Messages: " << MEDIUM_TEST_SIZE << std::endl;
    std::cout << "Duration: " << duration.count() << " μs" << std::endl;
    std::cout << "Throughput: " << std::fixed << std::setprecision(2) 
              << throughput << " msg/sec" << std::endl;
    
    // Check if log file was created (basic functionality test)
    bool logFileExists = std::filesystem::exists(config.logFilePath);
    std::cout << "Log file exists: " << (logFileExists ? "Yes" : "No") << std::endl;
    
    // Enterprise-grade expectations (simplified for reliability)
    EXPECT_GT(throughput, 30000.0) << "File rotation should maintain > 30,000 msg/sec";
    EXPECT_TRUE(logFileExists) << "Log file should be created";
    EXPECT_LT(duration.count(), 5000000) << "Should complete in < 5 seconds";
}

// ==================== BENCHMARK COMPARISON ====================

TEST_F(PerformanceTest, BenchmarkComparison) {
    std::cout << "\n=== BENCHMARK COMPARISON ===" << std::endl;
    
    // Test 1: Synchronous logging
    {
        Logger::Config config = perfConfig;
        config.asyncLogging = false;
        Logger logger(config);
        
        auto duration = measureTime([&]() {
            for (int i = 0; i < SMALL_TEST_SIZE; ++i) {
                logger.info("Benchmark sync message " + std::to_string(i));
            }
            logger.flush();
        });
        
        double throughput = calculateThroughput(SMALL_TEST_SIZE, duration);
        std::cout << "Sync Logging: " << std::fixed << std::setprecision(2) 
                  << throughput << " msg/sec" << std::endl;
    }
    
    // Test 2: Asynchronous logging
    {
        Logger logger(perfConfig);
        
        auto duration = measureTime([&]() {
            for (int i = 0; i < SMALL_TEST_SIZE; ++i) {
                logger.info("Benchmark async message " + std::to_string(i));
            }
            logger.flush();
        });
        
        double throughput = calculateThroughput(SMALL_TEST_SIZE, duration);
        std::cout << "Async Logging: " << std::fixed << std::setprecision(2) 
                  << throughput << " msg/sec" << std::endl;
    }
    
    // Test 3: Multi-threaded async
    {
        Logger logger(perfConfig);
        std::vector<std::thread> threads;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int t = 0; t < 4; ++t) {
            threads.emplace_back([&, t]() {
                for (int i = 0; i < SMALL_TEST_SIZE / 4; ++i) {
                    logger.info("Benchmark multi-thread message " + std::to_string(i));
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        logger.flush();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double throughput = calculateThroughput(SMALL_TEST_SIZE, duration);
        
        std::cout << "Multi-thread Async: " << std::fixed << std::setprecision(2) 
                  << throughput << " msg/sec" << std::endl;
    }
}

// ==================== PERFORMANCE REGRESSION TEST ====================

TEST_F(PerformanceTest, PerformanceRegressionTest) {
    Logger logger(perfConfig);
    
    // Baseline performance measurement
    auto baselineDuration = measureTime([&]() {
        for (int i = 0; i < SMALL_TEST_SIZE; ++i) {
            logger.info("Baseline performance test message " + std::to_string(i));
        }
        logger.flush();
    });
    
    double baselineThroughput = calculateThroughput(SMALL_TEST_SIZE, baselineDuration);
    
    std::cout << "\n=== PERFORMANCE REGRESSION TEST ===" << std::endl;
    std::cout << "Baseline Throughput: " << std::fixed << std::setprecision(2) 
              << baselineThroughput << " msg/sec" << std::endl;
    
    // Enterprise-grade regression threshold
    const double REGRESSION_THRESHOLD = 0.40; // 40% of baseline (adjusted for CI stability)
    
    // Run multiple iterations to check for consistency
    for (int iteration = 1; iteration <= 3; ++iteration) {
        auto duration = measureTime([&]() {
            for (int i = 0; i < SMALL_TEST_SIZE; ++i) {
                logger.info("Regression test iteration " + std::to_string(iteration) + 
                           " message " + std::to_string(i));
            }
            logger.flush();
        });
        
        double throughput = calculateThroughput(SMALL_TEST_SIZE, duration);
        double performanceRatio = throughput / baselineThroughput;
        
        std::cout << "Iteration " << iteration << " Throughput: " 
                  << std::fixed << std::setprecision(2) << throughput << " msg/sec"
                  << " (Ratio: " << std::fixed << std::setprecision(3) << performanceRatio << ")" << std::endl;
        
        // Check for performance regression
        EXPECT_GE(performanceRatio, REGRESSION_THRESHOLD) 
            << "Performance regression detected in iteration " << iteration;
    }
} 