# Octa-Core Build Management Scripts

This directory contains comprehensive build management tools for the Octa-Core C++ project.

## 🛠️ Scripts Overview

### [`manage.sh`](manage.sh) - Main Management Script

A comprehensive project management script that handles all aspects of building, testing, and maintaining the Octa-Core project.

**Key Features:**
- ✅ **Colored output** with clear progress indicators
- ✅ **Dependency validation** (checks for cmake, conan, ninja)
- ✅ **Preset validation** with auto-completion suggestions  
- ✅ **Parallel builds** with automatic job detection
- ✅ **Conan integration** for dependency management
- ✅ **Multiple build configurations** (debug, release, packages)
- ✅ **Comprehensive tooling** (format, docs, benchmarks, coverage)

**Usage Examples:**
```bash
# See all available commands
./scripts/manage.sh --help

# Full development cycle
./scripts/manage.sh all

# Release build with benchmarks
./scripts/manage.sh -p dev-release build
./scripts/manage.sh -p dev-release benchmark

# Code formatting and validation
./scripts/manage.sh format
./scripts/manage.sh test

# Generate documentation
./scripts/manage.sh docs

# Build engine packages
./scripts/manage.sh --shared package
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

**Features:**
- Supports multiple coverage tools (gcovr, lcov, gcov)
- HTML report generation
- Coverage threshold validation (≥70% for P1.4)
- Excludes test and benchmark code from coverage

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

The scripts work with predefined CMake presets:

- `dev-debug` - Development debug build (default)
- `dev-release` - Development release build with benchmarks
- `package-unity` - Unity engine package build
- `package-godot` - Godot engine package build  
- `conan-default` - Conan-managed build

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