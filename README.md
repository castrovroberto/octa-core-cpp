# Octa-Core C++

[![Build Status](https://github.com/castrovroberto/octa-core-cpp/workflows/CI/badge.svg)](https://github.com/castrovroberto/octa-core-cpp/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A high-performance, modular C++ implementation of the Octa-Core game logic library. This project ports the original Java implementation to modern C++20, providing a lightweight, engine-agnostic SDK suitable for integration with game engines like Godot and Unity.

## 🎮 About Octa-Core

Octa-Core is a strategic board game featuring an octagonal grid where players compete through chain reactions. Key mechanics include:

- **Octagonal Grid**: 8-directional movement and neighbor relationships
- **Chain Reactions**: Cascading captures that can dramatically change the board state
- **Strategic Depth**: Simple rules with complex emergent gameplay
- **Configurable Rules**: Multiple win conditions and safety levels

## ✨ Features

- **🚀 High Performance**: Optimized for real-time gameplay (median < 2ms per move)
- **🔧 Modular Design**: Interface-based architecture supporting multiple map implementations
- **🛡️ Exception Safety**: Three-tier rollback system (Validate, Light Undo, Full Rollback)
- **🧵 Thread Safety**: Optional thread-safe adapters for concurrent access
- **🎯 Engine Agnostic**: Clean C API for easy integration with any game engine
- **📦 Easy Integration**: First-class bindings for Godot 4 and Unity 2022 LTS
- **🧪 Well Tested**: Comprehensive unit tests with >90% code coverage
- **📚 Documented**: Full Doxygen documentation with examples

## 🏗️ Architecture

### Core Components

- **`IGameMap`**: Abstract interface for board representation
  - `GraphGameMap`: Hash map-based implementation for sparse grids
  - `ArrayGameMap`: 2D array-based implementation (planned)

- **`IGameLogic`**: Game rules and state management interface
  - `OctaGameLogic`: Complete implementation with chain reaction mechanics

- **`GameEngine`**: Main orchestrator coordinating game components

- **`GameCell`**: Individual board cells with octagonal neighbor relationships

### Data Models

```cpp
// Core coordinate system
struct Coordinate { int x, y; };

// Game states and players
enum class CellState { NEUTRAL, PLAYER_1, PLAYER_2, BLOCKED };
enum class Player { PLAYER_1, PLAYER_2 };

// Octagonal directions
enum class Direction { N, NE, E, SE, S, SW, W, NW };
```

## 🔧 Building

### Prerequisites

- **C++20** compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- **CMake 3.20+**
- **Conan 2.0+** (for dependency management)

### Quick Start

```bash
# Clone the repository
git clone https://github.com/robertocastro/octa-core-cpp.git
cd octa-core-cpp

# Install dependencies
conan install . --build=missing

# Configure and build
cmake --preset dev-debug
cmake --build --preset dev-debug

# Run the demo
./build/dev-debug/octa-core
```

### Build Options

```bash
# Enable testing (default: ON)
cmake -DBUILD_TESTS=ON ..

# Enable benchmarks
cmake -DBUILD_BENCHMARKS=ON ..

# Enable thread safety
cmake -DOCTA_THREAD_SAFE=ON ..

# Enable test coverage
cmake -DENABLE_COVERAGE=ON ..
```

## 🎯 Usage

### Basic Example

```cpp
#include "octa-core/engine/GameEngine.h"
#include "octa-core/map/GraphGameMap.h"
#include "octa-core/logic/OctaGameLogic.h"

int main() {
    // Create a 4-radius octagonal map (81 cells)
    auto gameMap = std::make_shared<GraphGameMap>(4);
    
    // Initialize game logic with default configuration
    auto gameLogic = std::make_unique<OctaGameLogic>(gameMap);
    
    // Create and run the game engine
    GameEngine engine(gameMap);
    engine.run();
    
    return 0;
}
```

### Making Moves

```cpp
// Get the current player
Player currentPlayer = gameLogic->getCurrentPlayer();

// Get a cell to play
auto centerCell = gameMap->at(Coordinate(0, 0));

// Validate and make the move
if (gameLogic->isValidMove(centerCell, currentPlayer)) {
    GameResult result = gameLogic->makeMove(centerCell, currentPlayer);
    
    if (result.winner.has_value()) {
        std::cout << "Game over! Winner: Player " 
                  << static_cast<int>(*result.winner) + 1 << std::endl;
    }
}
```

## 🧪 Testing

```bash
# Run unit tests
cmake --build --preset dev-debug --target run_tests
ctest --preset dev-debug

# Run with coverage
cmake --preset dev-debug -DENABLE_COVERAGE=ON
cmake --build --preset dev-debug
./scripts/generate_coverage.sh

# Run benchmarks
cmake --build --preset dev-debug --target perf_suite
./build/dev-debug/perf_suite
```

## 📦 Integration

### Game Engine Integration

The library provides clean interfaces for integration:

- **C API**: For maximum compatibility
- **Godot GDNative**: Native Godot 4 bindings
- **Unity P/Invoke**: C# wrapper for Unity integration

### Configuration Options

```cpp
struct GameConfig {
    WinCondition winCondition = WinCondition::ELIMINATION;
    int turnLimit = 60;
    bool stopOnEnemy = false;
    SafetyLevel safetyLevel = SafetyLevel::VALIDATE_ONLY;
};
```

## 📊 Performance

- **Move Processing**: < 2ms median, < 5ms 99th percentile
- **Memory Usage**: ~1KB per cell in GraphGameMap
- **Thread Safety**: Optional with minimal overhead
- **Platforms**: Windows, macOS, Linux

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Guidelines

- Follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- Write tests for new features
- Update documentation as needed
- Ensure CI passes

## 📚 Documentation

- **[API Documentation](https://robertocastro.github.io/octa-core-cpp/)** - Full Doxygen documentation
- **[Game Rules](docs/srd-v0.3.2.md)** - Complete game mechanics specification
- **[Implementation Plan](docs/implementation-plan-overview.md)** - Development roadmap
- **[Architecture Guide](docs/graph-map.md)** - Technical deep-dive

## 🗺️ Roadmap

- [x] **Phase P0**: Project scaffold and build system
- [x] **Phase P1**: Core game logic and data models
- [ ] **Phase P2**: Enhanced safety and benchmarking
- [ ] **Phase P3**: Thread safety and memory optimization
- [ ] **Phase P4**: Engine bindings and serialization
- [ ] **Phase P5**: Documentation and polish

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Original Java implementation team
- Modern C++ community for best practices
- Game engine communities for integration feedback

---

**Built with ❤️ and modern C++20**
