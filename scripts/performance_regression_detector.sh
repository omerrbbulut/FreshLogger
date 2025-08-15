#!/bin/bash

# Performance Regression Detector for FreshLogger
# Author: Ömer Bulut
# Phase 2: Performance Regression Detection

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BASELINE_DIR="performance_baselines"
CURRENT_RESULTS_DIR="current_performance"
REGRESSION_THRESHOLD=${REGRESSION_THRESHOLD:-10}  # 10% degradation threshold
VERBOSE=${VERBOSE:-false}
HISTORY_SIZE=${HISTORY_SIZE:-10}  # Keep last 10 baseline results

# Performance metrics to track
METRICS=(
    "throughput_msg_per_sec"
    "latency_microseconds"
    "memory_usage_kb"
    "file_rotation_time_ms"
    "concurrent_threads"
)

# Function to log messages
log() {
    local level="$1"
    shift
    local message="$*"
    
    case "$level" in
        "INFO") echo -e "${BLUE}ℹ️  $message${NC}" ;;
        "SUCCESS") echo -e "${GREEN}✅ $message${NC}" ;;
        "WARNING") echo -e "${YELLOW}⚠️  $message${NC}" ;;
        "ERROR") echo -e "${RED}❌ $message${NC}" ;;
    esac
}

# Function to run performance tests
run_performance_tests() {
    log "INFO" "Running performance tests..."
    
    mkdir -p "$CURRENT_RESULTS_DIR"
    
    # Run performance test suite
    if [[ -f "performance_tests" ]]; then
        ./performance_tests --gtest_output=xml:"$CURRENT_RESULTS_DIR/performance_results.xml" > "$CURRENT_RESULTS_DIR/performance_output.log" 2>&1
        
        if [[ $? -eq 0 ]]; then
            log "SUCCESS" "Performance tests completed successfully"
        else
            log "ERROR" "Performance tests failed"
            return 1
        fi
    else
        log "ERROR" "Performance test executable not found"
        return 1
    fi
    
    # Run stress tests for additional metrics
    if [[ -f "stress_tests" ]]; then
        ./stress_tests --gtest_output=xml:"$CURRENT_RESULTS_DIR/stress_results.xml" > "$CURRENT_RESULTS_DIR/stress_output.log" 2>&1
        log "INFO" "Stress tests completed"
    fi
}

# Function to extract performance metrics from test output
extract_metrics() {
    local results_file="$1"
    local metrics_file="$2"
    
    if [[ ! -f "$results_file" ]]; then
        log "WARNING" "Results file not found: $results_file"
        return 1
    fi
    
    # Initialize metrics JSON
    cat > "$metrics_file" << 'EOF'
{
    "timestamp": "",
    "git_commit": "",
    "metrics": {}
}
EOF
    
    # Extract timestamp
    local current_time=$(date -Iseconds)
    sed -i "s/\"timestamp\": \"\"/\"timestamp\": \"$current_time\"/" "$metrics_file"
    
    # Extract git commit hash
    local git_commit=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
    sed -i "s/\"git_commit\": \"\"/\"git_commit\": \"$git_commit\"/" "$metrics_file"
    
    # Parse test output for metrics
    local output_log="$CURRENT_RESULTS_DIR/performance_output.log"
    if [[ -f "$output_log" ]]; then
        # Extract throughput (messages per second)
        local throughput=$(grep -o "Throughput: [0-9.]* msg/sec" "$output_log" | tail -1 | grep -o "[0-9.]*" || echo "0")
        if [[ "$throughput" != "0" ]]; then
            jq ".metrics.throughput_msg_per_sec = $throughput" "$metrics_file" > "${metrics_file}.tmp" && mv "${metrics_file}.tmp" "$metrics_file"
        fi
        
        # Extract latency (microseconds)
        local latency=$(grep -o "Average Latency: [0-9.]* μs" "$output_log" | tail -1 | grep -o "[0-9.]*" || echo "0")
        if [[ "$latency" != "0" ]]; then
            jq ".metrics.latency_microseconds = $latency" "$metrics_file" > "${metrics_file}.tmp" && mv "${metrics_file}.tmp" "$metrics_file"
        fi
        
        # Extract memory usage (KB)
        local memory=$(grep -o "Memory Usage: [0-9.]* KB" "$output_log" | tail -1 | grep -o "[0-9.]*" || echo "0")
        if [[ "$memory" != "0" ]]; then
            jq ".metrics.memory_usage_kb = $memory" "$metrics_file" > "${metrics_file}.tmp" && mv "${metrics_file}.tmp" "$metrics_file"
        fi
        
        # Extract file rotation time (ms)
        local rotation_time=$(grep -o "File Rotation Time: [0-9.]* ms" "$output_log" | tail -1 | grep -o "[0-9.]*" || echo "0")
        if [[ "$rotation_time" != "0" ]]; then
            jq ".metrics.file_rotation_time_ms = $rotation_time" "$metrics_file" > "${metrics_file}.tmp" && mv "${metrics_file}.tmp" "$metrics_file"
        fi
        
        # Extract concurrent threads
        local threads=$(grep -o "Concurrent Threads: [0-9]*" "$output_log" | tail -1 | grep -o "[0-9]*" || echo "0")
        if [[ "$threads" != "0" ]]; then
            jq ".metrics.concurrent_threads = $threads" "$metrics_file" > "${metrics_file}.tmp" && mv "${metrics_file}.tmp" "$metrics_file"
        fi
    fi
    
    log "SUCCESS" "Metrics extracted to $metrics_file"
}

# Function to get baseline metrics
get_baseline_metrics() {
    local baseline_file="$1"
    
    if [[ -f "$baseline_file" ]]; then
        jq -r '.metrics | to_entries[] | "\(.key)=\(.value)"' "$baseline_file" 2>/dev/null
    fi
}

# Function to compare current vs baseline metrics
compare_metrics() {
    local current_metrics="$1"
    local baseline_metrics="$2"
    local comparison_file="$3"
    
    log "INFO" "Comparing current metrics with baseline..."
    
    # Initialize comparison JSON
    cat > "$comparison_file" << 'EOF'
{
    "timestamp": "",
    "baseline_file": "",
    "regressions": [],
    "improvements": [],
    "stable": []
}
EOF
    
    local current_time=$(date -Iseconds)
    sed -i "s/\"timestamp\": \"\"/\"timestamp\": \"$current_time\"/" "$comparison_file"
    sed -i "s/\"baseline_file\": \"\"/\"baseline_file\": \"$(basename "$baseline_metrics")\"/" "$comparison_file"
    
    # Compare each metric
    for metric in "${METRICS[@]}"; do
        local current_value=$(jq -r ".metrics.$metric // empty" "$current_metrics" 2>/dev/null)
        local baseline_value=$(jq -r ".metrics.$metric // empty" "$baseline_metrics" 2>/dev/null)
        
        if [[ -n "$current_value" && -n "$baseline_value" ]]; then
            # Use bc for floating point arithmetic
            local change_percent=$(echo "scale=2; ($current_value - $baseline_value) * 100 / $baseline_value" | bc)
            
            if [[ $(echo "$change_percent > $REGRESSION_THRESHOLD" | bc -l) -eq 1 ]]; then
                # Performance regression detected
                local regression_data="{\"metric\": \"$metric\", \"current\": $current_value, \"baseline\": $baseline_value, \"change_percent\": $change_percent}"
                jq ".regressions += [$regression_data]" "$comparison_file" > "${comparison_file}.tmp" && mv "${comparison_file}.tmp" "$comparison_file"
                
                log "WARNING" "Performance regression detected in $metric: $baseline_value → $current_value ($change_percent% change)"
                
            elif [[ $(echo "$change_percent < -$REGRESSION_THRESHOLD" | bc -l) -eq 1 ]]; then
                # Performance improvement detected
                local improvement_data="{\"metric\": \"$metric\", \"current\": $current_value, \"baseline\": $baseline_value, \"change_percent\": $change_percent}"
                jq ".improvements += [$improvement_data]" "$comparison_file" > "${comparison_file}.tmp" && mv "${comparison_file}.tmp" "$comparison_file"
                
                log "SUCCESS" "Performance improvement detected in $metric: $baseline_value → $current_value ($change_percent% change)"
                
            else
                # Performance is stable
                local stable_data="{\"metric\": \"$metric\", \"current\": $current_value, \"baseline\": $baseline_value, \"change_percent\": $change_percent}"
                jq ".stable += [$stable_data]" "$comparison_file" > "${comparison_file}.tmp" && mv "${comparison_file}.tmp" "$comparison_file"
                
                log "INFO" "Performance stable in $metric: $baseline_value → $current_value ($change_percent% change)"
            fi
        fi
    done
    
    log "SUCCESS" "Performance comparison completed"
}

# Function to save current results as baseline
save_as_baseline() {
    local current_metrics="$1"
    local baseline_name="$2"
    
    mkdir -p "$BASELINE_DIR"
    
    local baseline_file="$BASELINE_DIR/${baseline_name}_$(date +%Y%m%d_%H%M%S).json"
    cp "$current_metrics" "$baseline_file"
    
    log "SUCCESS" "Current results saved as baseline: $baseline_file"
    
    # Clean up old baselines (keep only last HISTORY_SIZE)
    local baseline_count=$(find "$BASELINE_DIR" -name "*.json" | wc -l)
    if [[ $baseline_count -gt $HISTORY_SIZE ]]; then
        log "INFO" "Cleaning up old baselines (keeping last $HISTORY_SIZE)..."
        find "$BASELINE_DIR" -name "*.json" -printf '%T@ %p\n' | sort -n | head -n $((baseline_count - HISTORY_SIZE)) | cut -d' ' -f2- | xargs rm -f
    fi
}

# Function to generate performance report
generate_performance_report() {
    local comparison_file="$1"
    local report_file="$2"
    
    log "INFO" "Generating performance report..."
    
    cat > "$report_file" << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <title>FreshLogger Performance Regression Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background: #f0f0f0; padding: 20px; border-radius: 5px; }
        .regression { background: #ffe6e6; padding: 15px; margin: 10px 0; border-radius: 5px; border-left: 5px solid #ff4444; }
        .improvement { background: #e6ffe6; padding: 15px; margin: 10px 0; border-radius: 5px; border-left: 5px solid #44ff44; }
        .stable { background: #e6e6ff; padding: 15px; margin: 10px 0; border-radius: 5px; border-left: 5px solid #4444ff; }
        .summary { background: #f9f9f9; padding: 15px; margin: 20px 0; border-radius: 5px; }
        table { border-collapse: collapse; width: 100%; margin: 10px 0; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #f2f2f2; }
        .metric-name { font-weight: bold; }
        .change-positive { color: green; }
        .change-negative { color: red; }
        .change-neutral { color: blue; }
    </style>
</head>
<body>
    <div class="header">
        <h1>📊 FreshLogger Performance Regression Report</h1>
        <p>Generated on: $(date)</p>
        <p>Baseline: $(basename "$(jq -r '.baseline_file' "$comparison_file")")</p>
    </div>
EOF
    
    # Add summary section
    local regressions_count=$(jq '.regressions | length' "$comparison_file")
    local improvements_count=$(jq '.improvements | length' "$comparison_file")
    local stable_count=$(jq '.stable | length' "$comparison_file")
    
    echo "<div class='summary'>" >> "$report_file"
    echo "<h2>📈 Performance Summary</h2>" >> "$report_file"
    echo "<p>Regressions: <span style='color: red; font-weight: bold;'>$regressions_count</span></p>" >> "$report_file"
    echo "<p>Improvements: <span style='color: green; font-weight: bold;'>$improvements_count</span></p>" >> "$report_file"
    echo "<p>Stable: <span style='color: blue; font-weight: bold;'>$stable_count</span></p>" >> "$report_file"
    echo "</div>" >> "$report_file"
    
    # Add regressions section
    if [[ $regressions_count -gt 0 ]]; then
        echo "<div class='regression'>" >> "$report_file"
        echo "<h3>⚠️ Performance Regressions</h3>" >> "$report_file"
        echo "<table><tr><th>Metric</th><th>Current</th><th>Baseline</th><th>Change</th></tr>" >> "$report_file"
        
        jq -r '.regressions[] | "<tr><td class=\"metric-name\">" + .metric + "</td><td>" + (.current | tostring) + "</td><td>" + (.baseline | tostring) + "</td><td class=\"change-negative\">+" + (.change_percent | tostring) + "%</td></tr>"' "$comparison_file" >> "$report_file"
        
        echo "</table></div>" >> "$report_file"
    fi
    
    # Add improvements section
    if [[ $improvements_count -gt 0 ]]; then
        echo "<div class='improvement'>" >> "$report_file"
        echo "<h3>🚀 Performance Improvements</h3>" >> "$report_file"
        echo "<table><tr><th>Metric</th><th>Current</th><th>Baseline</th><th>Change</th></tr>" >> "$report_file"
        
        jq -r '.improvements[] | "<tr><td class=\"metric-name\">" + .metric + "</td><td>" + (.current | tostring) + "</td><td>" + (.baseline | tostring) + "</td><td class=\"change-positive\">" + (.change_percent | tostring) + "%</td></tr>"' "$comparison_file" >> "$report_file"
        
        echo "</table></div>" >> "$report_file"
    fi
    
    # Add stable section
    if [[ $stable_count -gt 0 ]]; then
        echo "<div class='stable'>" >> "$report_file"
        echo "<h3>📊 Stable Performance</h3>" >> "$report_file"
        echo "<table><tr><th>Metric</th><th>Current</th><th>Baseline</th><th>Change</th></tr>" >> "$report_file"
        
        jq -r '.stable[] | "<tr><td class=\"metric-name\">" + .metric + "</td><td>" + (.current | tostring) + "</td><td>" + (.baseline | tostring) + "</td><td class=\"change-neutral\">" + (.change_percent | tostring) + "%</td></tr>"' "$comparison_file" >> "$report_file"
        
        echo "</table></div>" >> "$report_file"
    fi
    
    echo "</body></html>" >> "$report_file"
    
    log "SUCCESS" "Performance report generated: $report_file"
}

# Function to show help
show_help() {
    echo -e "${BLUE}📊 Performance Regression Detector - Usage${NC}"
    echo -e "${BLUE}==========================================${NC}"
    echo ""
    echo -e "Commands:"
    echo -e "  ${YELLOW}run${NC}       - Run performance tests and detect regressions"
    echo -e "  ${YELLOW}baseline${NC}  - Save current results as new baseline"
    echo -e "  ${YELLOW}compare${NC}   - Compare current results with latest baseline"
    echo -e "  ${YELLOW}report${NC}    - Generate HTML performance report"
    echo -e "  ${YELLOW}help${NC}      - Show this help message"
    echo ""
    echo -e "Environment Variables:"
    echo -e "  ${YELLOW}REGRESSION_THRESHOLD${NC} - Performance degradation threshold % (default: 10)"
    echo -e "  ${YELLOW}HISTORY_SIZE${NC}        - Number of baseline results to keep (default: 10)"
    echo -e "  ${YELLOW}VERBOSE${NC}             - Enable verbose output"
    echo ""
    echo -e "Examples:"
    echo -e "  ${YELLOW}./performance_regression_detector.sh run${NC}"
    echo -e "  ${YELLOW}./performance_regression_detector.sh baseline${NC}"
    echo -e "  ${YELLOW}REGRESSION_THRESHOLD=5 ./performance_regression_detector.sh run${NC}"
}

# Main execution
main() {
    local command="${1:-help}"
    
    case "$command" in
        "run")
            run_performance_tests
            extract_metrics "$CURRENT_RESULTS_DIR/performance_results.xml" "$CURRENT_RESULTS_DIR/current_metrics.json"
            
            # Find latest baseline
            local latest_baseline=$(find "$BASELINE_DIR" -name "*.json" -printf '%T@ %p\n' 2>/dev/null | sort -n | tail -1 | cut -d' ' -f2-)
            
            if [[ -n "$latest_baseline" ]]; then
                compare_metrics "$CURRENT_RESULTS_DIR/current_metrics.json" "$latest_baseline" "$CURRENT_RESULTS_DIR/comparison.json"
                generate_performance_report "$CURRENT_RESULTS_DIR/comparison.json" "$CURRENT_RESULTS_DIR/performance_report.html"
                
                # Check for regressions
                local regressions_count=$(jq '.regressions | length' "$CURRENT_RESULTS_DIR/comparison.json")
                if [[ $regressions_count -gt 0 ]]; then
                    log "WARNING" "$regressions_count performance regression(s) detected!"
                    exit 1
                else
                    log "SUCCESS" "No performance regressions detected"
                fi
            else
                log "WARNING" "No baseline found. Run 'baseline' command first."
            fi
            ;;
        "baseline")
            if [[ ! -f "$CURRENT_RESULTS_DIR/current_metrics.json" ]]; then
                log "INFO" "No current metrics found. Running performance tests first..."
                run_performance_tests
                extract_metrics "$CURRENT_RESULTS_DIR/performance_results.xml" "$CURRENT_RESULTS_DIR/current_metrics.json"
            fi
            
            local baseline_name="${2:-$(date +%Y%m%d)}"
            save_as_baseline "$CURRENT_RESULTS_DIR/current_metrics.json" "$baseline_name"
            ;;
        "compare")
            local latest_baseline=$(find "$BASELINE_DIR" -name "*.json" -printf '%T@ %p\n' 2>/dev/null | sort -n | tail -1 | cut -d' ' -f2-)
            
            if [[ -n "$latest_baseline" && -f "$CURRENT_RESULTS_DIR/current_metrics.json" ]]; then
                compare_metrics "$CURRENT_RESULTS_DIR/current_metrics.json" "$latest_baseline" "$CURRENT_RESULTS_DIR/comparison.json"
                log "SUCCESS" "Comparison completed: $CURRENT_RESULTS_DIR/comparison.json"
            else
                log "ERROR" "Baseline or current metrics not found"
                exit 1
            fi
            ;;
        "report")
            if [[ -f "$CURRENT_RESULTS_DIR/comparison.json" ]]; then
                generate_performance_report "$CURRENT_RESULTS_DIR/comparison.json" "$CURRENT_RESULTS_DIR/performance_report.html"
            else
                log "ERROR" "Comparison file not found. Run 'compare' command first."
                exit 1
            fi
            ;;
        "help"|*)
            show_help
            ;;
    esac
}

# Run main function
main "$@" 