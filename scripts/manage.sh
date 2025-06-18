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

# Available presets from CMakePresets.json
readonly AVAILABLE_PRESETS=(
    "conan-debug"
    "conan-release"
    "dev-debug"
    "dev-release" 
    "package-unity"
    "package-godot"
    "conan-default"
)

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
    
    printf "%sAvailable Presets:%s\n" "${CYAN}" "${NC}"
    printf "  %sConan-generated presets (preferred when available):%s\n" "${GREEN}" "${NC}"
    printf "    conan-debug, conan-release\n"
    printf "  %sManual presets:%s\n" "${GREEN}" "${NC}"
    printf "    dev-debug, dev-release, package-unity, package-godot, conan-default\n"
    printf "\n"
    printf "%sNote:%s The script auto-detects the best preset based on your Conan setup.\n" "${YELLOW}" "${NC}"
    printf "Conan-generated presets are preferred as they use the correct compiler toolchain.\n\n"
    
    printf "%sExamples:%s\n" "${CYAN}" "${NC}"
    printf "  %s build                    # Build with default preset\n" "$SCRIPT_NAME"
    printf "  %s -p dev-release build     # Build release version\n" "$SCRIPT_NAME"
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

# Function to detect the best preset for the current system
detect_best_preset() {
    local preset_type="$1"  # "debug" or "release"
    
    # Check if Conan has generated presets (these are preferred)
    case "$preset_type" in
        "debug")
            if [[ -f "build/build/Debug/generators/CMakePresets.json" ]] && 
               grep -q "conan-debug" build/build/Debug/generators/CMakePresets.json 2>/dev/null; then
                echo "conan-debug"
                return
            fi
            ;;
        "release")
            if [[ -f "build/build/Release/generators/CMakePresets.json" ]] && 
               grep -q "conan-release" build/build/Release/generators/CMakePresets.json 2>/dev/null; then
                echo "conan-release"
                return
            fi
            ;;
    esac
    
    # Fall back to manual presets
    case "$preset_type" in
        "debug")
            echo "dev-debug"
            ;;
        "release")
            echo "dev-release"
            ;;
    esac
}

# Function to check if a preset exists
preset_exists() {
    local preset="$1"
    
    # Check in Conan-generated presets first
    if [[ -f "build/build/Debug/generators/CMakePresets.json" ]]; then
        if grep -q "\"name\": \"$preset\"" build/build/Debug/generators/CMakePresets.json 2>/dev/null; then
            return 0
        fi
    fi
    
    if [[ -f "build/build/Release/generators/CMakePresets.json" ]]; then
        if grep -q "\"name\": \"$preset\"" build/build/Release/generators/CMakePresets.json 2>/dev/null; then
            return 0
        fi
    fi
    
    # Check in our manual presets
    for available in "${AVAILABLE_PRESETS[@]}"; do
        if [[ "$preset" == "$available" ]]; then
            return 0
        fi
    done
    
    return 1
}

# Validate preset
validate_preset() {
    local preset="$1"
    if ! preset_exists "$preset"; then
        log_error "Invalid preset '$preset'"
        log_info "Available presets: ${AVAILABLE_PRESETS[*]}"
        if [[ -f "build/build/Debug/generators/CMakePresets.json" ]]; then
            log_info "Note: Conan-generated presets are available and preferred"
        fi
        exit 1
    fi
}

# Get build directory for preset
get_build_dir() {
    local preset="$1"
    
    # For Conan-generated presets, check the actual preset definition
    for preset_file in "build/build/Debug/generators/CMakePresets.json" "build/build/Release/generators/CMakePresets.json"; do
        if [[ -f "$preset_file" ]] && 
           grep -q "\"name\": \"$preset\"" "$preset_file" 2>/dev/null; then
            # Extract binaryDir from the Conan preset
            local binary_dir=$(grep -A10 "\"name\": \"$preset\"" "$preset_file" | \
                              grep "binaryDir" | head -1 | sed 's/.*": *"//' | sed 's/".*//')
            if [[ -n "$binary_dir" ]]; then
                echo "$binary_dir"
                return
            fi
        fi
    done
    
    # Fall back to manual preset mapping
    case "$preset" in
        "conan-debug") echo "build/build/Debug" ;;
        "conan-release") echo "build/build/Release" ;;
        "dev-debug") echo "build/Debug" ;;
        "dev-release") echo "build/Release" ;;
        "package-unity") echo "build/Unity" ;;
        "package-godot") echo "build/Godot" ;;
        "conan-default") echo "build" ;;
        *) echo "build/$preset" ;;
    esac
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
MAKE_VERBOSE=""
if $VERBOSE; then
    CMAKE_VERBOSE="--verbose"
    MAKE_VERBOSE="VERBOSE=1"
fi

# Set up environment for manual presets that need Conan toolchain
if [[ -f "build/build/Debug/generators/conan_toolchain.cmake" ]]; then
    export CONAN_TOOLCHAIN="build/build/Debug/generators/conan_toolchain.cmake"
    if ! $QUIET; then
        log_info "Set CONAN_TOOLCHAIN=$CONAN_TOOLCHAIN (Debug)"
    fi
elif [[ -f "build/build/Release/generators/conan_toolchain.cmake" ]]; then
    export CONAN_TOOLCHAIN="build/build/Release/generators/conan_toolchain.cmake"
    if ! $QUIET; then
        log_info "Set CONAN_TOOLCHAIN=$CONAN_TOOLCHAIN (Release)"
    fi
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
        # Handle different build preset naming conventions
        preset_is_conan=false
        for preset_file in "build/build/Debug/generators/CMakePresets.json" "build/build/Release/generators/CMakePresets.json"; do
            if [[ -f "$preset_file" ]] && 
               grep -q "\"name\": \"$PRESET\"" "$preset_file" 2>/dev/null; then
                preset_is_conan=true
                break
            fi
        done
        
        if $preset_is_conan; then
            # For Conan-generated presets, the build preset has the same name
            BUILD_PRESET="$PRESET"
        else
            # For manual presets, append "-build"
            BUILD_PRESET="${PRESET}-build"
        fi
        
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
        
        # Handle different test preset naming conventions
        preset_is_conan=false
        for preset_file in "build/build/Debug/generators/CMakePresets.json" "build/build/Release/generators/CMakePresets.json"; do
            if [[ -f "$preset_file" ]] && 
               grep -q "\"name\": \"$PRESET\"" "$preset_file" 2>/dev/null; then
                preset_is_conan=true
                break
            fi
        done
        
        if $preset_is_conan; then
            # For Conan-generated presets, the test preset has the same name
            TEST_PRESET="$PRESET"
        else
            # For manual presets, append "-test"
            TEST_PRESET="${PRESET}-test"
        fi
        
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
            log_info "Consider using: $0 -p conan-release benchmark"
        fi
        
        BENCH_EXECUTABLE="$BUILD_DIR/perf_suite"
        if [[ ! -f "$BENCH_EXECUTABLE" ]]; then
            log_error "Benchmark executable not found at $BENCH_EXECUTABLE"
            log_info "Benchmarks need to be built first. Running with benchmarks enabled..."
            
            # Install dependencies with benchmarks enabled
            log_info "Installing dependencies with benchmarks enabled"
            conan_args=(
                "install" "."
                "--output-folder=build"
                "--build=missing"
                "-o" "octa-core/*:enable_benchmarks=True"
            )
            
            if [[ "$PRESET" == *"release"* ]] || [[ "$PRESET" == "package-"* ]]; then
                conan_args+=("--settings=build_type=Release")
            else
                conan_args+=("--settings=build_type=Debug")
            fi
            
            conan "${conan_args[@]}"
            
            # Reconfigure and build with benchmarks enabled
            log_info "Building with benchmarks enabled"
            cmake --preset "$PRESET" -DBUILD_BENCHMARKS=ON
            
            preset_is_conan=false
            for preset_file in "build/build/Debug/generators/CMakePresets.json" "build/build/Release/generators/CMakePresets.json"; do
                if [[ -f "$preset_file" ]] && 
                   grep -q "\"name\": \"$PRESET\"" "$preset_file" 2>/dev/null; then
                    preset_is_conan=true
                    break
                fi
            done
            
            if $preset_is_conan; then
                BUILD_PRESET="$PRESET"
            else
                BUILD_PRESET="${PRESET}-build"
            fi
            
            cmake --build --preset "$BUILD_PRESET" -j "$JOBS"
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
        
        for pkg_preset in "package-unity" "package-godot"; do
            log_info "Building $pkg_preset package"
            
            cmake --preset "$pkg_preset"
            cmake --build --preset "${pkg_preset}-build" -j "$JOBS"
            
            PKG_BUILD_DIR=$(get_build_dir "$pkg_preset")
            if ! $QUIET; then
                log_success "Package built in $PKG_BUILD_DIR"
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
        for preset in "${AVAILABLE_PRESETS[@]}"; do
            if [[ "$preset" == "$PRESET" ]]; then
                printf "  %s● %s%s (current)\n" "${GREEN}" "$preset" "${NC}"
            else
                printf "  ○ %s\n" "$preset"
            fi
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
        printf "  Preset: %s\n" "$PRESET"
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