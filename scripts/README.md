# 🛠️ Development Scripts

This directory contains automation scripts for building, testing, and managing the Octa-Core C++ project.

## 📜 Scripts Overview

### [`manage.sh`](manage.sh) - Main Project Management

The primary development script with comprehensive project management capabilities.

**Smart Preset Detection**: The script automatically detects and prioritizes Conan-generated presets (like `conan-debug`) which use the correct compiler toolchain. This ensures compatibility with our Homebrew LLVM setup on macOS.

**Usage:**
```bash
./scripts/manage.sh [options] <command>
```

**Key Features:**
- **Auto-detects best preset**: Prefers `conan-debug`/`conan-release` when available
- **Compiler compatibility**: Works with Homebrew LLVM toolchain  
- **11 comprehensive commands**: install, build, test, benchmark, coverage, format, docs, package, clean, presets, info, all
- **Colored output** with terminal detection
- **Preset validation** to prevent configuration errors

**Commands:**
- `install` - Install dependencies using Conan (with smart toolchain detection)
- `build` - Configure and build using the best available preset
- `test` - Run unit tests with appropriate configuration
- `benchmark` - Run performance benchmarks (requires release build)
- `coverage` - Generate test coverage reports
- `format` - Format code using clang-format
- `docs` - Generate Doxygen documentation
- `package` - Build Unity and Godot packages
- `clean` - Remove all build artifacts
- `presets` - List available CMake presets
- `info` - Show project and environment information
- `all` - Full development cycle (install + build + test)

**Preset Priority Order:**
1. **Conan-generated presets** (preferred): `conan-debug`, `conan-release`
2. **Manual presets** (fallback): `dev-debug`, `dev-release`, `package-unity`, `package-godot`

**Options:**
- `-p PRESET` - Override preset (default: auto-detect)
- `-j JOBS` - Parallel build jobs (default: CPU count)
- `-v` - Verbose output
- `-q` - Quiet mode
- `--shared` - Build shared libraries
- `--thread-safe` - Enable thread safety

**Examples:**
```bash
# Full development cycle with auto-detected preset
./scripts/manage.sh all

# Build with specific preset
./scripts/manage.sh -p conan-debug build

# Release build with benchmarks
./scripts/manage.sh -p dev-release build
./scripts/manage.sh -p dev-release benchmark

# Show available presets and current configuration
./scripts/manage.sh presets
./scripts/manage.sh info
```

### [`quick.sh`](quick.sh) - Development Shortcuts

A convenience script providing quick shortcuts for common development tasks.

**Available Shortcuts:**
- `dev` - Full development cycle (install + build + test)
- `release` - Build and test in release mode  
- `bench` - Quick benchmark run (release build)
- `check` - Quick test run (debug build)
- `fmt` - Format code
- `clean` - Clean all build artifacts
- `pkg` - Build packages for Unity and Godot
- `cov` - Generate coverage report
- `docs` - Generate documentation

**Usage Examples:**
```bash
# Quick development validation
./scripts/quick.sh dev

# Performance testing
./scripts/quick.sh bench

# Code quality check
./scripts/quick.sh fmt
./scripts/quick.sh check
```

### [`generate_coverage.sh`](generate_coverage.sh) - Coverage Reports

Generates comprehensive test coverage reports using gcovr or lcov.

**Smart Preset Detection**: Automatically detects and uses the `conan-debug` preset when available, ensuring compatibility with the Homebrew LLVM toolchain.

**Features:**
- **Auto-detects best preset**: Prefers `conan-debug` over `conan-default`
- Supports multiple coverage tools (gcovr, lcov, gcov)
- HTML report generation
- Coverage threshold validation (≥70% for P1.4)
- Excludes test and benchmark code from coverage
- Works with both Conan-generated and manual build directories

## 🚀 Quick Start

```bash
# Make scripts executable (if needed)
chmod +x scripts/*.sh

# Development workflow
./scripts/quick.sh dev        # Install deps, build, test
./scripts/quick.sh fmt        # Format code
./scripts/quick.sh check      # Validate tests

# Performance workflow  
./scripts/quick.sh release    # Build release version
./scripts/quick.sh bench      # Run benchmarks

# Documentation workflow
./scripts/quick.sh docs       # Generate docs
./scripts/quick.sh cov        # Generate coverage

# Package workflow
./scripts/quick.sh pkg        # Build engine packages
```

## 🔧 Configuration

### CMake Presets

The scripts work with both Conan-generated and manual CMake presets:

**Conan-Generated Presets (Preferred):**
- `conan-debug` - Debug build with Homebrew LLVM toolchain
- `conan-release` - Release build with Homebrew LLVM toolchain

**Manual Presets (Fallback):**
- `dev-debug` - Development debug build
- `dev-release` - Development release build with benchmarks
- `package-unity` - Unity engine package build
- `package-godot` - Godot engine package build  
- `conan-default` - Conan-managed build (legacy)

**Smart Auto-Detection:**
The scripts automatically detect the best preset to use:
1. **First Priority**: Conan-generated presets (use correct compiler)
2. **Fallback**: Manual presets for compatibility

### Environment Variables

The scripts respect these environment variables:

- `CONAN_TOOLCHAIN` - Path to Conan toolchain file
- `CMAKE_BUILD_PARALLEL_LEVEL` - Override parallel job count
- `NINJA_STATUS` - Ninja build status format

### Dependency Requirements

Required tools (automatically checked):
- CMake 3.20+
- Conan 2.0+
- Ninja (build system)
- C++20 compiler (GCC 10+, Clang 12+, MSVC 2019+)

Optional tools:
- clang-format (for code formatting)
- Doxygen (for documentation)
- gcovr/lcov (for coverage reports)

## 📊 Output Examples

### Successful Build
```
▶ Building project (preset: dev-debug)
ℹ Configuring with preset: dev-debug
ℹ Building with preset: dev-debug-build
✓ Build completed successfully
```

### Error Handling
```
✗ Invalid preset 'invalid-preset'
ℹ Available presets: dev-debug dev-release package-unity package-godot conan-default
```

### Project Information
```
▶ Project and Environment Information

Project:
  Name: Octa-Core C++
  Version: 1.0.0
  Root: /path/to/octa-core-cpp

Current Configuration:
  Preset: dev-debug
  Build Directory: build/Debug
  Jobs: 8
  Shared Libraries: false
  Thread Safe: false

Tools:
  CMake: cmake version 3.27.4
  Conan: Conan version 2.17.0
  Ninja: 1.11.1
  GCC/Clang: Apple clang version 17.0.0
```

## 🤝 Contributing

When adding new scripts or modifying existing ones:

1. Follow the established patterns for error handling and output formatting
2. Use the logging functions (`log_info`, `log_success`, `log_error`, etc.)
3. Validate inputs and provide helpful error messages
4. Update this README with any new functionality
5. Test scripts on different platforms (macOS, Linux, Windows with WSL)

## 📝 Notes

- Scripts use `set -euo pipefail` for strict error handling
- Colors are disabled automatically in non-interactive environments
- All scripts are designed to be run from the project root directory
- Scripts gracefully handle missing optional tools
- Preset validation prevents common configuration errors

### Recent Improvements (v1.1)

- **Smart Preset Detection**: Automatically detects Conan-generated presets
- **Homebrew LLVM Support**: Prioritizes `conan-debug` preset for macOS compatibility
- **Improved Error Handling**: Better validation and fallback mechanisms
- **Coverage Script Updates**: Dynamic build directory detection
- **Documentation Updates**: Comprehensive workflow documentation 