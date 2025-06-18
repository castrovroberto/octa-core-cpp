#!/usr/bin/env bash
set -euo pipefail

# Colors for output (check if terminal supports colors)
if [[ -t 1 ]] && command -v tput >/dev/null 2>&1 && tput colors >/dev/null 2>&1; then
    readonly RED=$(tput setaf 1)
    readonly GREEN=$(tput setaf 2)
    readonly YELLOW=$(tput setaf 3)
    readonly BLUE=$(tput setaf 4)
    readonly PURPLE=$(tput setaf 5)
    readonly CYAN=$(tput setaf 6)
    readonly WHITE=$(tput setaf 7; tput bold)
    readonly NC=$(tput sgr0) # No Color
else
    readonly RED=''
    readonly GREEN=''
    readonly YELLOW=''
    readonly BLUE=''
    readonly PURPLE=''
    readonly CYAN=''
    readonly WHITE=''
    readonly NC=''
fi

# Script metadata
readonly SCRIPT_NAME=$(basename "$0")
readonly SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
readonly ROOT_DIR=$(cd "$SCRIPT_DIR/.." && pwd)
cd "$ROOT_DIR"

# Logging functions
log_info() {
    printf "%sℹ%s %s\n" "${BLUE}" "${NC}" "$*"
}

log_success() {
    printf "%s✓%s %s\n" "${GREEN}" "${NC}" "$*"
}

log_warning() {
    printf "%s⚠%s %s\n" "${YELLOW}" "${NC}" "$*"
}

log_error() {
    printf "%s✗%s %s\n" "${RED}" "${NC}" "$*" >&2
}

log_step() {
    printf "%s▶%s %s\n" "${CYAN}" "${NC}" "$*"
}

# Dynamic preset discovery using CMake
get_available_configure_presets() {
    cmake --list-presets=configure 2>/dev/null | grep -E "^  ['\"]?[a-zA-Z0-9_-]+['\"]?" | sed 's/^[[:space:]]*['\''"]*//' | sed 's/['\''"].*$//' || true
}

get_available_build_presets() {
    cmake --list-presets=build 2>/dev/null | grep -E "^  ['\"]?[a-zA-Z0-9_-]+['\"]?" | sed 's/^[[:space:]]*['\''"]*//' | sed 's/['\''"].*$//' || true
}

get_available_test_presets() {
    cmake --list-presets=test 2>/dev/null | grep -E "^  ['\"]?[a-zA-Z0-9_-]+['\"]?" | sed 's/^[[:space:]]*['\''"]*//' | sed 's/['\''"].*$//' || true
}

# Check if a preset exists using CMake
preset_exists() {
    local preset="$1"
    local preset_type="$2"  # configure, build, or test
    
    case "$preset_type" in
        "configure")
            get_available_configure_presets | grep -Fxq "$preset"
            ;;
        "build")
            get_available_build_presets | grep -Fxq "$preset"
            ;;
        "test")
            get_available_test_presets | grep -Fxq "$preset"
            ;;
        *)
            return 1
            ;;
    esac
}

# Intelligent preset detection that prioritizes Conan-generated presets
detect_best_preset() {
    local build_type="$1"  # "debug" or "release"
    local available_presets
    
    available_presets=$(get_available_configure_presets)
    
    # Priority order: conan-generated > manual presets
    case "$build_type" in
        "debug")
            for preset in "conan-debug" "dev-debug" "conan-default"; do
                if echo "$available_presets" | grep -Fxq "$preset"; then
                    echo "$preset"
                    return
                fi
            done
            ;;
        "release")
            for preset in "conan-release" "dev-release"; do
                if echo "$available_presets" | grep -Fxq "$preset"; then
                    echo "$preset"
                    return
                fi
            done
            ;;
    esac
    
    # If no preferred preset found, return the first available one
    echo "$available_presets" | head -1
}

# Get corresponding build/test preset for a configure preset
get_corresponding_preset() {
    local configure_preset="$1"
    local preset_type="$2"  # "build" or "test"
    
    # First, try the exact same name (works for Conan-generated presets)
    if preset_exists "$configure_preset" "$preset_type"; then
        echo "$configure_preset"
        return
    fi
    
    # Then try with suffix (works for manual presets)
    local suffixed_preset="${configure_preset}-${preset_type}"
    if preset_exists "$suffixed_preset" "$preset_type"; then
        echo "$suffixed_preset"
        return
    fi
    
    # Fallback: return any available preset of the requested type
    case "$preset_type" in
        "build")
            get_available_build_presets | head -1
            ;;
        "test")
            get_available_test_presets | head -1
            ;;
    esac
}

# Get build directory using CMake introspection
get_build_dir() {
    local preset="$1"
    
    # For well-known presets, use direct mapping (fastest and most reliable)
    case "$preset" in
        "conan-debug") echo "build/build/Debug" ;;
        "conan-release") echo "build/build/Release" ;;
        "dev-debug") echo "build/Debug" ;;
        "dev-release") echo "build/Release" ;;
        "package-unity") echo "build/Unity" ;;
        "package-godot") echo "build/Godot" ;;
        "conan-default") echo "build" ;;
        *)
            # For unknown presets, fall back to pattern-based detection
            case "$preset" in
                *debug*) echo "build/Debug" ;;
                *release*) echo "build/Release" ;;
                *) echo "build" ;;
            esac
            ;;
    esac
}

# Public API for other scripts to use
# Usage: manage.sh detect-preset [debug|release]
if [[ "${1:-}" == "detect-preset" ]]; then
    detect_best_preset "${2:-debug}"
    exit 0
fi

# Usage: manage.sh list-presets [configure|build|test]
if [[ "${1:-}" == "list-presets" ]]; then
    case "${2:-configure}" in
        "configure") get_available_configure_presets ;;
        "build") get_available_build_presets ;;
        "test") get_available_test_presets ;;
        *) 
            echo "configure presets:"
            get_available_configure_presets | sed 's/^/  /'
            echo "build presets:"
            get_available_build_presets | sed 's/^/  /'
            echo "test presets:"
            get_available_test_presets | sed 's/^/  /'
            ;;
    esac
    exit 0
fi

usage() {
    printf "%sOcta-Core C++ Project Management Script%s\n\n" "${WHITE}" "${NC}"
    printf "%sUsage:%s %s [options] <command>\n\n" "${CYAN}" "${NC}" "$SCRIPT_NAME"
    
    printf "%sCommands:%s\n" "${CYAN}" "${NC}"
    printf "  %sinstall%s      Install dependencies with Conan\n" "${WHITE}" "${NC}"
    printf "  %sbuild%s        Configure and build the project\n" "${WHITE}" "${NC}"
    printf "  %stest%s         Run unit tests\n" "${WHITE}" "${NC}"
    printf "  %sbenchmark%s    Run performance benchmarks\n" "${WHITE}" "${NC}"
    printf "  %scoverage%s     Generate test coverage report\n" "${WHITE}" "${NC}"
    printf "  %sformat%s       Format code using clang-format\n" "${WHITE}" "${NC}"
    printf "  %sdocs%s         Generate documentation with Doxygen\n" "${WHITE}" "${NC}"
    printf "  %spackage%s      Create distribution packages\n" "${WHITE}" "${NC}"
    printf "  %sclean%s        Remove build directories\n" "${WHITE}" "${NC}"
    printf "  %spresets%s      List available CMake presets\n" "${WHITE}" "${NC}"
    printf "  %sinfo%s         Show project and environment information\n" "${WHITE}" "${NC}"
    printf "  %sall%s          Install deps, configure, build, and test\n\n" "${WHITE}" "${NC}"
    
    printf "%sOptions:%s\n" "${CYAN}" "${NC}"
    printf "  %s-p PRESET%s    Specify CMake configure preset (default: auto-detect)\n" "${WHITE}" "${NC}"
    printf "  %s-j JOBS%s      Number of parallel build jobs (default: auto)\n" "${WHITE}" "${NC}"
    printf "  %s-v%s           Verbose output\n" "${WHITE}" "${NC}"
    printf "  %s-q%s           Quiet mode (minimal output)\n" "${WHITE}" "${NC}"
    printf "  %s--shared%s     Build shared libraries\n" "${WHITE}" "${NC}"
    printf "  %s--thread-safe%s Enable thread safety\n" "${WHITE}" "${NC}"
    printf "  %s--benchmarks%s  Force enable benchmarks\n" "${WHITE}" "${NC}"
    printf "  %s-h%s           Show this help\n\n" "${WHITE}" "${NC}"
    
    printf "%sNote:%s The script auto-detects the best preset based on your CMake configuration.\n" "${YELLOW}" "${NC}"
    printf "Conan-generated presets are preferred when available.\n\n"
    
    printf "%sExamples:%s\n" "${CYAN}" "${NC}"
    printf "  %s build                    # Build with auto-detected preset\n" "$SCRIPT_NAME"
    printf "  %s -p dev-release build     # Build with specific preset\n" "$SCRIPT_NAME"
    printf "  %s -p dev-release benchmark # Run benchmarks on release build\n" "$SCRIPT_NAME"
    printf "  %s --shared package         # Build shared libs for packaging\n" "$SCRIPT_NAME"
    printf "  %s all                      # Full development cycle\n\n" "$SCRIPT_NAME"
    
    exit 1
}

# Check if required tools are installed
check_dependencies() {
    local missing_tools=()
    
    command -v cmake >/dev/null || missing_tools+=("cmake")
    command -v conan >/dev/null || missing_tools+=("conan") 
    command -v ninja >/dev/null || missing_tools+=("ninja")
    
    if [[ ${#missing_tools[@]} -gt 0 ]]; then
        log_error "Missing required tools: ${missing_tools[*]}"
        log_info "Please install the missing tools and try again"
        exit 1
    fi
}

# Validate preset
validate_preset() {
    local preset="$1"
    if ! preset_exists "$preset" "configure"; then
        log_error "Invalid configure preset '$preset'"
        log_info "Available configure presets:"
        get_available_configure_presets | sed 's/^/  /'
        exit 1
    fi
}

# Initialize variables
PRESET="$(detect_best_preset debug)"
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo "4")
VERBOSE=false
QUIET=false
SHARED=false
THREAD_SAFE=false
BENCHMARKS=false

# Parse options
while [[ $# -gt 0 ]]; do
    case $1 in
        -p|--preset)
            PRESET="$2"
            shift 2
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -q|--quiet)
            QUIET=true
            shift
            ;;
        --shared)
            SHARED=true
            shift
            ;;
        --thread-safe)
            THREAD_SAFE=true
            shift
            ;;
        --benchmarks)
            BENCHMARKS=true
            shift
            ;;
        -h|--help)
            usage
            ;;
        -*)
            log_error "Unknown option: $1"
            usage
            ;;
        *)
            break
            ;;
    esac
done

COMMAND=${1:-}

if [[ -z "$COMMAND" ]]; then
    usage
fi

# Validate preset now that functions are defined
validate_preset "$PRESET"

# Set build directory
BUILD_DIR=$(get_build_dir "$PRESET")

# Set verbose flags
CMAKE_VERBOSE=""
if $VERBOSE; then
    CMAKE_VERBOSE="--verbose"
fi

# Main command execution
case "$COMMAND" in
    install)
        log_step "Installing dependencies with Conan"
        check_dependencies
        
        conan_args=(
            "install" "."
            "--output-folder=build"
            "--build=missing"
        )
        
        if [[ "$PRESET" == *"release"* ]] || [[ "$PRESET" == "package-"* ]]; then
            conan_args+=("--settings=build_type=Release")
        else
            conan_args+=("--settings=build_type=Debug")
        fi
        
        if $SHARED; then
            conan_args+=("-o" "*:shared=True")
        fi
        
        if $THREAD_SAFE; then
            conan_args+=("-o" "octa-core/*:thread_safe=True")
        fi
        
        # Enable benchmarks for release presets or when explicitly requested
        if [[ "$PRESET" == *"release"* ]] || [[ "$PRESET" == "package-"* ]] || $BENCHMARKS; then
            conan_args+=("-o" "octa-core/*:enable_benchmarks=True")
        fi
        
        if ! $QUIET; then
            log_info "Running: conan ${conan_args[*]}"
        fi
        
        conan "${conan_args[@]}"
        log_success "Dependencies installed successfully"
        ;;
        
    build)
        log_step "Building project (preset: $PRESET)"
        check_dependencies
        
        # Configure
        if ! $QUIET; then
            log_info "Configuring with preset: $PRESET"
        fi
        
        configure_args=("--preset" "$PRESET")
        if $VERBOSE; then
            configure_args+=("$CMAKE_VERBOSE")
        fi
        
        cmake "${configure_args[@]}"
        
        # Build
        BUILD_PRESET=$(get_corresponding_preset "$PRESET" "build")
        
        if ! $QUIET; then
            log_info "Building with preset: $BUILD_PRESET"
        fi
        
        build_args=("--build" "--preset" "$BUILD_PRESET")
        build_args+=("-j" "$JOBS")
        if $VERBOSE; then
            build_args+=("$CMAKE_VERBOSE")
        fi
        
        cmake "${build_args[@]}"
        log_success "Build completed successfully"
        ;;
        
    test)
        log_step "Running tests (preset: $PRESET)"
        
        TEST_PRESET=$(get_corresponding_preset "$PRESET" "test")
        
        test_args=("--preset" "$TEST_PRESET")
        
        if $VERBOSE; then
            test_args+=("--verbose")
        fi
        
        if ! $QUIET; then
            log_info "Running tests with preset: $TEST_PRESET"
        fi
        
        ctest "${test_args[@]}"
        log_success "Tests completed successfully"
        ;;
        
    benchmark)
        log_step "Running performance benchmarks"
        check_dependencies
        
        if [[ "$PRESET" != *"release"* ]]; then
            log_warning "Benchmarks should be run with release builds for accurate results"
            log_info "Consider using: $0 -p $(detect_best_preset release) benchmark"
        fi
        
        BENCH_EXECUTABLE="$BUILD_DIR/perf_suite"
        if [[ ! -f "$BENCH_EXECUTABLE" ]]; then
            log_error "Benchmark executable not found at $BENCH_EXECUTABLE"
            log_info "Benchmarks need to be built first. Running with benchmarks enabled..."
            
            # Install dependencies with benchmarks enabled
            "$0" -p "$PRESET" --benchmarks install
            "$0" -p "$PRESET" --benchmarks build
        fi
        
        if [[ ! -f "$BENCH_EXECUTABLE" ]]; then
            log_error "Failed to build benchmark executable"
            exit 1
        fi
        
        log_info "Running benchmarks: $BENCH_EXECUTABLE"
        "$BENCH_EXECUTABLE"
        log_success "Benchmarks completed successfully"
        ;;
        
    coverage)
        log_step "Generating test coverage report"
        bash scripts/generate_coverage.sh
        log_success "Coverage report generated"
        ;;
        
    format)
        log_step "Formatting code with clang-format"
        
        if ! command -v clang-format >/dev/null; then
            log_error "clang-format not found"
            exit 1
        fi
        
        find src/ include/ tests/ bench/ -name "*.cpp" -o -name "*.h" | \
            xargs clang-format -i --style=file
        
        log_success "Code formatting completed"
        ;;
        
    docs)
        log_step "Generating documentation with Doxygen"
        
        if ! command -v doxygen >/dev/null; then
            log_error "Doxygen not found"
            exit 1
        fi
        
        if [[ -f "docs/Doxyfile" ]]; then
            cd docs
            doxygen Doxyfile
            cd ..
            log_success "Documentation generated in docs/html/"
        else
            log_error "Doxyfile not found in docs/ directory"
            exit 1
        fi
        ;;
        
    package)
        log_step "Creating distribution packages"
        
        # Dynamically find package presets
        available_presets=$(get_available_configure_presets)
        for preset in $available_presets; do
            if [[ "$preset" == package-* ]]; then
                log_info "Building $preset package"
                
                "$0" -p "$preset" build
                
                if ! $QUIET; then
                    PKG_BUILD_DIR=$(get_build_dir "$preset")
                    log_success "Package built in $PKG_BUILD_DIR"
                fi
            fi
        done
        
        log_success "All packages created successfully"
        ;;
        
    clean)
        log_step "Cleaning build directories"
        rm -rf build/
        log_success "Build directories cleaned"
        ;;
        
    presets)
        log_step "Available CMake presets"
        printf "\n"
        printf "%sConfigure Presets:%s\n" "${CYAN}" "${NC}"
        get_available_configure_presets | while read -r preset; do
            if [[ "$preset" == "$PRESET" ]]; then
                printf "  %s● %s%s (current)\n" "${GREEN}" "$preset" "${NC}"
            else
                printf "  ○ %s\n" "$preset"
            fi
        done
        
        printf "\n%sBuild Presets:%s\n" "${CYAN}" "${NC}"
        get_available_build_presets | while read -r preset; do
            printf "  ○ %s\n" "$preset"
        done
        
        printf "\n%sTest Presets:%s\n" "${CYAN}" "${NC}"
        get_available_test_presets | while read -r preset; do
            printf "  ○ %s\n" "$preset"
        done
        printf "\n"
        ;;
        
    info)
        log_step "Project and Environment Information"
        echo
        printf "${CYAN}Project:${NC}\n"
        printf "  Name: Octa-Core C++\n"
        printf "  Version: 1.0.0\n"
        printf "  Root: %s\n" "$ROOT_DIR"
        echo
        printf "${CYAN}Current Configuration:${NC}\n"
        printf "  Configure Preset: %s\n" "$PRESET"
        printf "  Build Preset: %s\n" "$(get_corresponding_preset "$PRESET" "build")"
        printf "  Test Preset: %s\n" "$(get_corresponding_preset "$PRESET" "test")"
        printf "  Build Directory: %s\n" "$BUILD_DIR"
        printf "  Jobs: %s\n" "$JOBS"
        printf "  Shared Libraries: %s\n" "$SHARED"
        printf "  Thread Safe: %s\n" "$THREAD_SAFE"
        printf "  Benchmarks: %s\n" "$BENCHMARKS"
        echo
        printf "${CYAN}Tools:${NC}\n"
        printf "  CMake: %s\n" "$(cmake --version | head -1 || echo "Not found")"
        printf "  Conan: %s\n" "$(conan --version 2>/dev/null || echo "Not found")"
        printf "  Ninja: %s\n" "$(ninja --version 2>/dev/null || echo "Not found")"
        printf "  GCC/Clang: %s\n" "$(c++ --version | head -1 || echo "Not found")"
        echo
        ;;
        
    all)
        log_step "Running full development cycle"
        "$0" -p "$PRESET" install
        "$0" -p "$PRESET" build  
        "$0" -p "$PRESET" test
        log_success "Full development cycle completed"
        ;;
        
    *)
        log_error "Unknown command: $COMMAND"
        usage
        ;;
esac