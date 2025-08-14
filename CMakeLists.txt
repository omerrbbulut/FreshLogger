# FreshLogger CMake Configuration
# Author: Ömer Bulut

cmake_minimum_required(VERSION 3.10)
project(FreshLogger VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(GTest REQUIRED)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Create example executable
add_executable(example example.cpp)
target_link_libraries(example spdlog::spdlog fmt::fmt pthread)

# Create unit tests executable
add_executable(unit_tests LoggerTest.cpp)
target_link_libraries(unit_tests spdlog::spdlog fmt::fmt pthread GTest::gtest GTest::gtest_main)

# Create simple tests executable
add_executable(simple_tests SimpleLoggerTest.cpp)
target_link_libraries(simple_tests spdlog::spdlog fmt::fmt pthread GTest::gtest GTest::gtest_main)

# Create performance tests executable
add_executable(performance_tests PerformanceTest.cpp)
target_link_libraries(performance_tests spdlog::spdlog fmt::fmt pthread GTest::gtest GTest::gtest_main)

# Create stress tests executable
add_executable(stress_tests StressTest.cpp)
target_link_libraries(stress_tests spdlog::spdlog fmt::fmt pthread GTest::gtest GTest::gtest_main)

# Enable testing
enable_testing()

# Add tests
add_test(NAME UnitTests COMMAND unit_tests)
add_test(NAME SimpleTests COMMAND simple_tests)
add_test(NAME PerformanceTests COMMAND performance_tests)
add_test(NAME StressTests COMMAND stress_tests)

# Custom targets for convenience
add_custom_target(basic-tests
    COMMAND ${CMAKE_COMMAND} -E echo "Running basic tests..."
    COMMAND simple_tests
    COMMAND unit_tests
    DEPENDS simple_tests unit_tests
    COMMENT "Running basic test suite"
)

add_custom_target(performance-tests
    COMMAND ${CMAKE_COMMAND} -E echo "Running performance tests..."
    COMMAND performance_tests
    DEPENDS performance_tests
    COMMENT "Running performance test suite"
)

add_custom_target(stress-tests
    COMMAND ${CMAKE_COMMAND} -E echo "Running stress tests..."
    COMMAND stress_tests
    DEPENDS stress_tests
    COMMENT "Running stress test suite"
)

add_custom_target(enterprise-tests
    COMMAND ${CMAKE_COMMAND} -E echo "Running enterprise-grade test suite..."
    COMMAND simple_tests
    COMMAND unit_tests
    COMMAND performance_tests
    COMMAND stress_tests
    DEPENDS simple_tests unit_tests performance_tests stress_tests
    COMMENT "Running complete enterprise test suite"
)

# Status message
message(STATUS "FreshLogger CMake configuration complete!")
message(STATUS "Available targets: example, unit_tests, simple_tests, performance_tests, stress_tests")
message(STATUS "Custom targets: basic-tests, performance-tests, stress-tests, enterprise-tests") 