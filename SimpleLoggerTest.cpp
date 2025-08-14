/**
 * @file SimpleLoggerTest.cpp
 * @brief Simple unit tests for the Logger library
 * @author Omer Bulut
 */

#include "Logger.hpp"
#include <gtest/gtest.h>
#include <iostream>

class SimpleLoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }
    
    void TearDown() override {
        // Test cleanup
    }
};

// Test 1: Basic logger creation
TEST_F(SimpleLoggerTest, BasicCreation) {
    Logger logger;
    EXPECT_TRUE(logger.getLogger() != nullptr);
}

// Test 2: Basic logging methods exist
TEST_F(SimpleLoggerTest, LoggingMethodsExist) {
    Logger logger;
    
    // These should not crash
    EXPECT_NO_THROW(logger.info("Test info"));
    EXPECT_NO_THROW(logger.warning("Test warning"));
    EXPECT_NO_THROW(logger.error("Test error"));
    EXPECT_NO_THROW(logger.debug("Test debug"));
    EXPECT_NO_THROW(logger.trace("Test trace"));
    EXPECT_NO_THROW(logger.fatal("Test fatal"));
}

// Test 3: Configuration structure
TEST_F(SimpleLoggerTest, ConfigStructure) {
    Logger::Config config;
    
    // Test default values
    EXPECT_EQ(config.minLevel, Logger::LogLevel::INFO);
    EXPECT_EQ(config.consoleOutput, true);
    EXPECT_EQ(config.asyncLogging, false); // Changed default to false for better compatibility
    EXPECT_EQ(config.maxFileSize, 10 * 1024 * 1024); // 10MB
    EXPECT_EQ(config.maxFiles, 5);
    EXPECT_EQ(config.queueSize, 8192);
    EXPECT_EQ(config.flushInterval, 3);
}

// Test 4: Log level enum
TEST_F(SimpleLoggerTest, LogLevelEnum) {
    // Test that all log levels are defined
    EXPECT_EQ(static_cast<int>(Logger::LogLevel::TRACE), 0);
    EXPECT_EQ(static_cast<int>(Logger::LogLevel::DEBUG), 1);
    EXPECT_EQ(static_cast<int>(Logger::LogLevel::INFO), 2);
    EXPECT_EQ(static_cast<int>(Logger::LogLevel::WARNING), 3);
    EXPECT_EQ(static_cast<int>(Logger::LogLevel::ERROR), 4);
    EXPECT_EQ(static_cast<int>(Logger::LogLevel::FATAL), 5);
}

// Test 5: Logger with custom config
TEST_F(SimpleLoggerTest, CustomConfig) {
    Logger::Config config;
    config.consoleOutput = true;
    config.asyncLogging = false;
    
    Logger logger(config);
    EXPECT_TRUE(logger.getLogger() != nullptr);
    
    // Should not crash
    EXPECT_NO_THROW(logger.info("Test with custom config"));
}

// Test 6: Set log level
TEST_F(SimpleLoggerTest, SetLogLevel) {
    Logger logger;
    
    // Should not crash
    EXPECT_NO_THROW(logger.setLogLevel(Logger::LogLevel::DEBUG));
    EXPECT_NO_THROW(logger.setLogLevel(Logger::LogLevel::WARNING));
    EXPECT_NO_THROW(logger.setLogLevel(Logger::LogLevel::ERROR));
}

// Test 7: Flush method
TEST_F(SimpleLoggerTest, FlushMethod) {
    Logger logger;
    
    // Should not crash
    EXPECT_NO_THROW(logger.flush());
}

// Test 8: Get logger instance
TEST_F(SimpleLoggerTest, GetLoggerInstance) {
    Logger logger;
    
    auto spdlog_logger = logger.getLogger();
    EXPECT_TRUE(spdlog_logger != nullptr);
}

// Test 9: Multiple logger instances
TEST_F(SimpleLoggerTest, MultipleInstances) {
    Logger logger1;
    Logger logger2;
    
    EXPECT_TRUE(logger1.getLogger() != nullptr);
    EXPECT_TRUE(logger2.getLogger() != nullptr);
    
    // Should not crash
    EXPECT_NO_THROW(logger1.info("From logger 1"));
    EXPECT_NO_THROW(logger2.info("From logger 2"));
}

// Test 10: Logger destruction
TEST_F(SimpleLoggerTest, LoggerDestruction) {
    {
        Logger logger;
        logger.info("Test message");
        // Logger should be destroyed here without issues
    }
    
    // Should not crash
    EXPECT_TRUE(true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 