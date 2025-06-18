#!/usr/bin/env bash
set -euo pipefail

# Quick development shortcuts for Octa-Core C++
# This script provides convenient aliases for common development tasks

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
MANAGE_SCRIPT="$SCRIPT_DIR/manage.sh"

# Colors (check if terminal supports colors)
if [[ -t 1 ]] && command -v tput >/dev/null 2>&1 && tput colors >/dev/null 2>&1; then
    readonly GREEN=$(tput setaf 2)
    readonly BLUE=$(tput setaf 4)
    readonly YELLOW=$(tput setaf 3)
    readonly NC=$(tput sgr0)
else
    readonly GREEN=''
    readonly BLUE=''
    readonly YELLOW=''
    readonly NC=''
fi

usage() {
    printf "%sQuick Development Shortcuts for Octa-Core C++%s\n\n" "${BLUE}" "${NC}"
    printf "Usage: %s <shortcut>\n\n" "$(basename "$0")"
    
    printf "%sQuick Shortcuts:%s\n" "${YELLOW}" "${NC}"
    printf "  %sdev%s        Full development cycle (install + build + test)\n" "${GREEN}" "${NC}"
    printf "  %srelease%s    Build and test in release mode\n" "${GREEN}" "${NC}"
    printf "  %sbench%s      Quick benchmark run (release build)\n" "${GREEN}" "${NC}"
    printf "  %scheck%s      Quick test run (debug build)\n" "${GREEN}" "${NC}"
    printf "  %sfmt%s        Format code\n" "${GREEN}" "${NC}"
    printf "  %sclean%s      Clean all build artifacts\n" "${GREEN}" "${NC}"
    printf "  %spkg%s        Build packages for Unity and Godot\n" "${GREEN}" "${NC}"
    printf "  %scov%s        Generate coverage report\n" "${GREEN}" "${NC}"
    printf "  %sdocs%s       Generate documentation\n\n" "${GREEN}" "${NC}"
    
    printf "%sExamples:%s\n" "${YELLOW}" "${NC}"
    printf "  %s dev       # Install, build, and test\n" "$(basename "$0")"
    printf "  %s bench     # Run performance benchmarks\n" "$(basename "$0")"
    printf "  %s check     # Quick test validation\n" "$(basename "$0")"
    printf "  %s pkg       # Build engine packages\n\n" "$(basename "$0")"
    
    printf "For full options, use: %s --help\n\n" "$MANAGE_SCRIPT"
    exit 1
}

if [[ $# -eq 0 ]]; then
    usage
fi

case "$1" in
    dev)
        printf "%s🚀 Running full development cycle...%s\n" "${BLUE}" "${NC}"
        "$MANAGE_SCRIPT" -p dev-debug all
        ;;
    release)
        printf "%s🏁 Building and testing release version...%s\n" "${BLUE}" "${NC}"
        "$MANAGE_SCRIPT" -p dev-release build
        "$MANAGE_SCRIPT" -p dev-release test
        ;;
    bench)
        printf "%s⚡ Running performance benchmarks...%s\n" "${BLUE}" "${NC}"
        "$MANAGE_SCRIPT" -p dev-release build
        "$MANAGE_SCRIPT" -p dev-release benchmark
        ;;
    check)
        printf "%s✅ Quick test validation...%s\n" "${BLUE}" "${NC}"
        "$MANAGE_SCRIPT" -p dev-debug test
        ;;
    fmt)
        printf "%s🎨 Formatting code...%s\n" "${BLUE}" "${NC}"
        "$MANAGE_SCRIPT" format
        ;;
    clean)
        printf "%s🧹 Cleaning build artifacts...%s\n" "${BLUE}" "${NC}"
        "$MANAGE_SCRIPT" clean
        ;;
    pkg)
        printf "%s📦 Building engine packages...%s\n" "${BLUE}" "${NC}"
        "$MANAGE_SCRIPT" package
        ;;
    cov)
        printf "%s📊 Generating coverage report...%s\n" "${BLUE}" "${NC}"
        "$MANAGE_SCRIPT" coverage
        ;;
    docs)
        printf "%s📚 Generating documentation...%s\n" "${BLUE}" "${NC}"
        "$MANAGE_SCRIPT" docs
        ;;
    *)
        printf "%sUnknown shortcut: %s%s\n" "${YELLOW}" "$1" "${NC}"
        printf "\n"
        usage
        ;;
esac 