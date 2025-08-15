/**
 * @file MacroTest.cpp
 * @brief Tests for convenience macros in Logger.hpp
 * @author Ömer Bulut
 */

#include "Logger.hpp"
#include <gtest/gtest.h>
#include <filesystem>

class MacroTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        std::filesystem::create_directories("macro_test_logs");
    }
    
    void TearDown() override {
        // Clean up test files
        try {
            std::filesystem::remove_all("macro_test_logs");
        } catch (const std::exception& e) {
            // Silently ignore cleanup errors
        }
    }
};

// Test convenience macros
TEST_F(MacroTest, ConvenienceMacros) {
    Logger::Config config;
    config.logFilePath = "macro_test_logs/macro_test.log";
    config.asyncLogging = false;
    config.consoleOutput = false;
    
    Logger logger(config);
    
    // Test all convenience macros
    EXPECT_NO_THROW(LOG_TRACE("Trace message via macro"));
    EXPECT_NO_THROW(LOG_DEBUG("Debug message via macro"));
    EXPECT_NO_THROW(LOG_INFO("Info message via macro"));
    EXPECT_NO_THROW(LOG_WARNING("Warning message via macro"));
    EXPECT_NO_THROW(LOG_ERROR("Error message via macro"));
    EXPECT_NO_THROW(LOG_FATAL("Fatal message via macro"));
    
    logger.flush();
    
    // Verify file was created
    EXPECT_TRUE(std::filesystem::exists("macro_test_logs/macro_test.log"));
    EXPECT_GT(std::filesystem::file_size("macro_test_logs/macro_test.log"), 0);
}

// Test macro with different message types
TEST_F(MacroTest, MacroWithSpecialMessages) {
    Logger::Config config;
    config.logFilePath = "macro_test_logs/macro_special.log";
    config.asyncLogging = false;
    config.consoleOutput = false;
    
    Logger logger(config);
    
    // Test macros with empty messages
    EXPECT_NO_THROW(LOG_INFO(""));
    EXPECT_NO_THROW(LOG_WARNING("   "));
    
    // Test macros with special characters
    EXPECT_NO_THROW(LOG_ERROR("Special chars: !@#$%^&*()"));
    EXPECT_NO_THROW(LOG_FATAL("Unicode: 🚀🔥💻"));
    
    logger.flush();
    
    EXPECT_TRUE(std::filesystem::exists("macro_test_logs/macro_special.log"));
}

// Test macro performance
TEST_F(MacroTest, MacroPerformance) {
    Logger::Config config;
    config.logFilePath = "macro_test_logs/macro_perf.log";
    config.asyncLogging = true;
    config.consoleOutput = false;
    
    Logger logger(config);
    
    // Test macro performance with many messages
    for (int i = 0; i < 1000; ++i) {
        LOG_INFO("Performance test message " + std::to_string(i));
    }
    
    logger.flush();
    
    EXPECT_TRUE(std::filesystem::exists("macro_test_logs/macro_perf.log"));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 