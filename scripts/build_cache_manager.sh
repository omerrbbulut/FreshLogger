#!/bin/bash

# Build Cache Manager for FreshLogger
# Author: Ömer Bulut
# Phase 2: Caching Implementation

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
CACHE_DIR=".build_cache"
CACHE_MAX_SIZE=${CACHE_MAX_SIZE:-500}  # MB
CACHE_CLEANUP_THRESHOLD=${CACHE_CLEANUP_THRESHOLD:-80}  # Percentage
VERBOSE=${VERBOSE:-false}

# Cache structure
OBJECT_CACHE="$CACHE_DIR/objects"
HEADER_CACHE="$CACHE_DIR/headers"
DEPENDENCY_CACHE="$CACHE_DIR/dependencies"
BUILD_INFO="$CACHE_DIR/build_info.json"

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

# Function to calculate file hash
calculate_hash() {
    local file="$1"
    if [[ -f "$file" ]]; then
        sha256sum "$file" | cut -d' ' -f1
    else
        echo "file_not_found"
    fi
}

# Function to calculate directory hash
calculate_dir_hash() {
    local dir="$1"
    if [[ -d "$dir" ]]; then
        find "$dir" -type f -name "*.h" -o -name "*.hpp" | sort | xargs sha256sum | sha256sum | cut -d' ' -f1
    else
        echo "dir_not_found"
    fi
}

# Function to initialize cache
init_cache() {
    log "INFO" "Initializing build cache..."
    
    mkdir -p "$OBJECT_CACHE" "$HEADER_CACHE" "$DEPENDENCY_CACHE"
    
    # Create build info file if it doesn't exist
    if [[ ! -f "$BUILD_INFO" ]]; then
        cat > "$BUILD_INFO" << 'EOF'
{
    "cache_version": "1.0",
    "created": "",
    "last_cleanup": "",
    "total_hits": 0,
    "total_misses": 0,
    "cache_size_mb": 0
}
EOF
        # Set initial values
        local current_time=$(date -Iseconds)
        sed -i "s/\"created\": \"\"/\"created\": \"$current_time\"/" "$BUILD_INFO"
        sed -i "s/\"last_cleanup\": \"\"/\"last_cleanup\": \"$current_time\"/" "$BUILD_INFO"
    fi
    
    log "SUCCESS" "Cache initialized in $CACHE_DIR"
}

# Function to get cache statistics
get_cache_stats() {
    if [[ -f "$BUILD_INFO" ]]; then
        local hits=$(jq -r '.total_hits' "$BUILD_INFO" 2>/dev/null || echo "0")
        local misses=$(jq -r '.total_misses' "$BUILD_INFO" 2>/dev/null || echo "0")
        local size=$(jq -r '.cache_size_mb' "$BUILD_INFO" 2>/dev/null || echo "0")
        
        echo -e "${BLUE}📊 Cache Statistics${NC}"
        echo -e "${BLUE}==================${NC}"
        echo -e "Cache Hits: ${GREEN}$hits${NC}"
        echo -e "Cache Misses: ${YELLOW}$misses${NC}"
        echo -e "Cache Size: ${YELLOW}${size}MB${NC}"
        
        if [[ $hits -gt 0 ]] || [[ $misses -gt 0 ]]; then
            local total=$((hits + misses))
            local hit_rate=$((hits * 100 / total))
            echo -e "Hit Rate: ${GREEN}${hit_rate}%${NC}"
        fi
    fi
}

# Function to check if object is cached
is_object_cached() {
    local source_file="$1"
    local object_file="$2"
    
    local source_hash=$(calculate_hash "$source_file")
    local cache_key="${source_hash}_$(basename "$object_file")"
    local cache_file="$OBJECT_CACHE/$cache_key"
    
    if [[ -f "$cache_file" ]]; then
        # Check if cached object is newer than source
        if [[ "$cache_file" -nt "$source_file" ]]; then
            # Copy cached object to target location
            cp "$cache_file" "$object_file"
            log "SUCCESS" "Cache hit for $(basename "$source_file")"
            
            # Update cache statistics
            local hits=$(($(jq -r '.total_hits' "$BUILD_INFO" 2>/dev/null || echo "0") + 1))
            jq ".total_hits = $hits" "$BUILD_INFO" > "${BUILD_INFO}.tmp" && mv "${BUILD_INFO}.tmp" "$BUILD_INFO"
            
            return 0
        fi
    fi
    
    return 1
}

# Function to cache object
cache_object() {
    local source_file="$1"
    local object_file="$2"
    
    if [[ -f "$object_file" ]]; then
        local source_hash=$(calculate_hash "$source_file")
        local cache_key="${source_hash}_$(basename "$object_file")"
        local cache_file="$OBJECT_CACHE/$cache_key"
        
        # Cache the object
        cp "$object_file" "$cache_file"
        
        # Update cache statistics
        local misses=$(($(jq -r '.total_misses' "$BUILD_INFO" 2>/dev/null || echo "0") + 1))
        jq ".total_misses = $misses" "$BUILD_INFO" > "${BUILD_INFO}.tmp" && mv "${BUILD_INFO}.tmp" "$BUILD_INFO"
        
        log "INFO" "Cached object for $(basename "$source_file")"
    fi
}

# Function to check header dependencies
check_header_dependencies() {
    local source_file="$1"
    local header_hash=$(calculate_dir_hash ".")
    
    local cache_key="headers_${header_hash}"
    local cache_file="$HEADER_CACHE/$cache_key"
    
    if [[ -f "$cache_file" ]]; then
        # Headers haven't changed, we can use cached objects
        return 0
    else
        # Headers changed, clear old header cache and create new
        rm -f "$HEADER_CACHE"/*
        touch "$cache_file"
        return 1
    fi
}

# Function to calculate cache size
calculate_cache_size() {
    if [[ -d "$CACHE_DIR" ]]; then
        du -sm "$CACHE_DIR" 2>/dev/null | cut -f1 || echo "0"
    else
        echo "0"
    fi
}

# Function to cleanup cache
cleanup_cache() {
    local current_size=$(calculate_cache_size)
    
    if [[ $current_size -gt $CACHE_MAX_SIZE ]]; then
        log "WARNING" "Cache size ($current_size MB) exceeds limit ($CACHE_MAX_SIZE MB)"
        log "INFO" "Cleaning up cache..."
        
        # Remove oldest cached objects (based on access time)
        find "$OBJECT_CACHE" -type f -printf '%A@ %p\n' | sort -n | head -n 100 | cut -d' ' -f2- | xargs rm -f
        
        # Update last cleanup time
        local current_time=$(date -Iseconds)
        jq ".last_cleanup = \"$current_time\"" "$BUILD_INFO" > "${BUILD_INFO}.tmp" && mv "${BUILD_INFO}.tmp" "$BUILD_INFO"
        
        local new_size=$(calculate_cache_size)
        log "SUCCESS" "Cache cleaned up: ${current_size}MB → ${new_size}MB"
    fi
}

# Function to show cache usage
show_cache_usage() {
    echo -e "${BLUE}📁 Cache Directory Structure${NC}"
    echo -e "${BLUE}============================${NC}"
    
    if [[ -d "$CACHE_DIR" ]]; then
        echo -e "Cache Root: ${YELLOW}$CACHE_DIR${NC}"
        echo -e "Objects: ${YELLOW}$(find "$OBJECT_CACHE" -type f 2>/dev/null | wc -l) files${NC}"
        echo -e "Headers: ${YELLOW}$(find "$HEADER_CACHE" -type f 2>/dev/null | wc -l) files${NC}"
        echo -e "Dependencies: ${YELLOW}$(find "$DEPENDENCY_CACHE" -type f 2>/dev/null | wc -l) files${NC}"
        echo -e "Total Size: ${YELLOW}$(calculate_cache_size)MB${NC}"
    else
        echo -e "${RED}Cache directory not found${NC}"
    fi
}

# Function to clear cache
clear_cache() {
    log "WARNING" "Clearing entire build cache..."
    rm -rf "$CACHE_DIR"
    log "SUCCESS" "Cache cleared"
}

# Function to show help
show_help() {
    echo -e "${BLUE}🔧 Build Cache Manager - Usage${NC}"
    echo -e "${BLUE}================================${NC}"
    echo ""
    echo -e "Commands:"
    echo -e "  ${YELLOW}init${NC}      - Initialize cache directory"
    echo -e "  ${YELLOW}stats${NC}     - Show cache statistics"
    echo -e "  ${YELLOW}usage${NC}     - Show cache usage information"
    echo -e "  ${YELLOW}cleanup${NC}   - Clean up cache if size exceeds limit"
    echo -e "  ${YELLOW}clear${NC}     - Clear entire cache"
    echo -e "  ${YELLOW}help${NC}      - Show this help message"
    echo ""
    echo -e "Environment Variables:"
    echo -e "  ${YELLOW}CACHE_MAX_SIZE${NC}           - Maximum cache size in MB (default: 500)"
    echo -e "  ${YELLOW}CACHE_CLEANUP_THRESHOLD${NC}  - Cleanup threshold percentage (default: 80)"
    echo -e "  ${YELLOW}VERBOSE${NC}                  - Enable verbose output"
    echo ""
    echo -e "Examples:"
    echo -e "  ${YELLOW}./build_cache_manager.sh init${NC}"
    echo -e "  ${YELLOW}./build_cache_manager.sh stats${NC}"
    echo -e "  ${YELLOW}CACHE_MAX_SIZE=1000 ./build_cache_manager.sh cleanup${NC}"
}

# Main execution
main() {
    local command="${1:-help}"
    
    case "$command" in
        "init")
            init_cache
            ;;
        "stats")
            get_cache_stats
            ;;
        "usage")
            show_cache_usage
            ;;
        "cleanup")
            cleanup_cache
            ;;
        "clear")
            clear_cache
            ;;
        "help"|*)
            show_help
            ;;
    esac
}

# Run main function
main "$@" 