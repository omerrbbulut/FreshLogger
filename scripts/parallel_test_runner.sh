#!/bin/bash

# Parallel Test Runner for FreshLogger
# Author: Ömer Bulut
# Phase 2: Parallel Execution Implementation

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
MAX_PARALLEL_JOBS=${MAX_PARALLEL_JOBS:-4}  # Default to 4 parallel jobs
TIMEOUT_SECONDS=${TIMEOUT_SECONDS:-300}     # 5 minutes timeout per test
VERBOSE=${VERBOSE:-false}
REPORT_DIR="parallel_test_reports"
LOG_DIR="parallel_test_logs"

# Test suites to run
TEST_SUITES=(
    "unit_tests"
    "simple_tests" 
    "performance_tests"
    "stress_tests"
    "edge_case_tests"
    "macro_tests"
)

# Create directories
mkdir -p "$REPORT_DIR" "$LOG_DIR" "test_logs"

echo -e "${BLUE}🚀 FreshLogger Parallel Test Runner${NC}"
echo -e "${BLUE}================================${NC}"
echo -e "Max Parallel Jobs: ${YELLOW}$MAX_PARALLEL_JOBS${NC}"
echo -e "Timeout per Test: ${YELLOW}${TIMEOUT_SECONDS}s${NC}"
echo -e "Report Directory: ${YELLOW}$REPORT_DIR${NC}"
echo -e "Log Directory: ${YELLOW}$LOG_DIR${NC}"
echo ""

# Function to run a single test suite
run_test_suite() {
    local test_name="$1"
    local log_file="$LOG_DIR/${test_name}.log"
    local report_file="$REPORT_DIR/${test_name}_report.xml"
    local start_time=$(date +%s)
    
    echo -e "${BLUE}▶️  Starting $test_name...${NC}"
    
    # Run test with timeout and capture output
    if timeout "$TIMEOUT_SECONDS" ./"$test_name" --gtest_output=xml:"$report_file" > "$log_file" 2>&1; then
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        echo -e "${GREEN}✅ $test_name completed in ${duration}s${NC}"
        return 0
    else
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        echo -e "${RED}❌ $test_name failed after ${duration}s${NC}"
        
        # Check if it's a timeout or actual test failure
        if grep -q "Killed" "$log_file"; then
            echo -e "${YELLOW}⚠️  $test_name timed out${NC}"
        else
            echo -e "${YELLOW}⚠️  $test_name had test failures (check logs)${NC}"
        fi
        
        return 1
    fi
}

# Function to run tests in parallel
run_parallel_tests() {
    local pids=()
    local test_names=()
    local running=0
    local completed=0
    local failed=0
    
    echo -e "${BLUE}🔄 Starting parallel test execution...${NC}"
    
    for test_suite in "${TEST_SUITES[@]}"; do
        # Check if test executable exists
        if [[ ! -f "$test_suite" ]]; then
            echo -e "${RED}❌ Test executable not found: $test_suite${NC}"
            continue
        fi
        
        # Wait if we've reached max parallel jobs
        while [[ ${#pids[@]} -ge $MAX_PARALLEL_JOBS ]]; do
            for i in "${!pids[@]}"; do
                if ! kill -0 "${pids[$i]}" 2>/dev/null; then
                    # Process finished, remove from array
                    wait "${pids[$i]}"
                    local exit_code=$?
                    if [[ $exit_code -eq 0 ]]; then
                        ((completed++))
                    else
                        ((failed++))
                    fi
                    unset "pids[$i]"
                    unset "test_names[$i]"
                fi
            done
            sleep 1
        done
        
        # Start new test
        run_test_suite "$test_suite" &
        local pid=$!
        pids+=($pid)
        test_names+=("$test_suite")
        echo -e "${YELLOW}🔄 Started $test_suite (PID: $pid)${NC}"
    done
    
    # Wait for remaining tests
    echo -e "${BLUE}⏳ Waiting for remaining tests to complete...${NC}"
    for i in "${!pids[@]}"; do
        wait "${pids[$i]}"
        local exit_code=$?
        if [[ $exit_code -eq 0 ]]; then
            ((completed++))
        else
            ((failed++))
        fi
    done
    
    echo ""
    echo -e "${BLUE}📊 Parallel Test Execution Summary${NC}"
    echo -e "${BLUE}==================================${NC}"
    echo -e "Total Tests: ${YELLOW}${#TEST_SUITES[@]}${NC}"
    echo -e "Completed: ${GREEN}$completed${NC}"
    echo -e "Failed: ${RED}$failed${NC}"
    echo -e "Success Rate: ${YELLOW}$((completed * 100 / ${#TEST_SUITES[@]}))%${NC}"
}

# Function to generate consolidated report
generate_consolidated_report() {
    echo -e "${BLUE}📋 Generating consolidated test report...${NC}"
    
    local report_file="$REPORT_DIR/consolidated_report.html"
    
    cat > "$report_file" << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <title>FreshLogger Parallel Test Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background: #f0f0f0; padding: 20px; border-radius: 5px; }
        .summary { background: #e8f5e8; padding: 15px; border-radius: 5px; margin: 20px 0; }
        .test-suite { background: #f9f9f9; padding: 15px; margin: 10px 0; border-radius: 5px; }
        .success { color: green; }
        .failure { color: red; }
        .warning { color: orange; }
        table { border-collapse: collapse; width: 100%; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #f2f2f2; }
    </style>
</head>
<body>
    <div class="header">
        <h1>🚀 FreshLogger Parallel Test Report</h1>
        <p>Generated on: $(date)</p>
    </div>
EOF
    
    # Add summary section
    echo "<div class='summary'>" >> "$report_file"
    echo "<h2>📊 Test Summary</h2>" >> "$report_file"
    echo "<p>Total Test Suites: ${#TEST_SUITES[@]}</p>" >> "$report_file"
    echo "<p>Max Parallel Jobs: $MAX_PARALLEL_JOBS</p>" >> "$report_file"
    echo "<p>Timeout per Test: ${TIMEOUT_SECONDS}s</p>" >> "$report_file"
    echo "</div>" >> "$report_file"
    
    # Add individual test suite results
    for test_suite in "${TEST_SUITES[@]}"; do
        local report_file_path="$REPORT_DIR/${test_suite}_report.xml"
        if [[ -f "$report_file_path" ]]; then
            echo "<div class='test-suite'>" >> "$report_file"
            echo "<h3>🔧 $test_suite</h3>" >> "$report_file"
            
            # Parse XML report for basic info
            if command -v xmllint >/dev/null 2>&1; then
                local tests=$(xmllint --xpath "string(//testsuites/@tests)" "$report_file_path" 2>/dev/null || echo "N/A")
                local failures=$(xmllint --xpath "string(//testsuites/@failures)" "$report_file_path" 2>/dev/null || echo "N/A")
                local time=$(xmllint --xpath "string(//testsuites/@time)" "$report_file_path" 2>/dev/null || echo "N/A")
                
                echo "<p>Tests: $tests | Failures: $failures | Time: ${time}s</p>" >> "$report_file"
            fi
            
            echo "<p><a href='${test_suite}_report.xml'>View XML Report</a></p>" >> "$report_file"
            echo "</div>" >> "$report_file"
        fi
    done
    
    echo "</body></html>" >> "$report_file"
    
    echo -e "${GREEN}✅ Consolidated report generated: $report_file${NC}"
}

# Main execution
main() {
    echo -e "${BLUE}🔍 Checking test executables...${NC}"
    
    # Verify all test executables exist
    local missing_tests=()
    for test_suite in "${TEST_SUITES[@]}"; do
        if [[ ! -f "$test_suite" ]]; then
            missing_tests+=("$test_suite")
        fi
    done
    
    if [[ ${#missing_tests[@]} -gt 0 ]]; then
        echo -e "${RED}❌ Missing test executables:${NC}"
        for test in "${missing_tests[@]}"; do
            echo -e "${RED}   - $test${NC}"
        done
        echo -e "${YELLOW}💡 Run 'make all' to build all tests${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✅ All test executables found${NC}"
    echo ""
    
    # Run parallel tests
    run_parallel_tests
    
    # Generate consolidated report
    generate_consolidated_report
    
    echo ""
    echo -e "${BLUE}🎯 Parallel test execution completed!${NC}"
    echo -e "Reports: ${YELLOW}$REPORT_DIR/${NC}"
    echo -e "Logs: ${YELLOW}$LOG_DIR/${NC}"
}

# Run main function
main "$@" 