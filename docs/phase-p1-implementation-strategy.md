# Phase P1: Core Logic - Implementation Strategy

**Duration**: 2-3 weeks  
**Approach**: 4 Sequential Milestones with incremental testing  
**Target**: SRD v3.2 compliance with ≥70% test coverage

---

## 🎯 **Milestone Overview**

| Milestone | Focus | Duration | Dependencies |
|-----------|-------|----------|--------------|
| **P1.1** | Core Data Models | 3-4 days | None |
| **P1.2** | Map Interface Refactoring | 2-3 days | P1.1 complete |
| **P1.3** | Game Logic Foundation | 4-5 days | P1.1, P1.2 complete |
| **P1.4** | Testing & Integration | 2-3 days | All previous complete |

---

## 🔧 **Milestone P1.1: Core Data Models (Days 1-4)**

**Goal**: Create fundamental enums and refactor GameCell to use proper state management.

### **Step 1.1: Create Core Enums (Day 1)**

Create the foundational enum types that everything else depends on:

```cpp
// include/octa-core/core/Enums.h
enum class CellState : uint8_t {
    NEUTRAL, PLAYER_1, PLAYER_2, BLOCKED
};

enum class Player : uint8_t {
    PLAYER_1, PLAYER_2
};
```

**Deliverables:**
- [x] `include/octa-core/core/Enums.h`
- [x] Basic enum tests in `tests/test_main.cpp`

### **Step 1.2: Create Direction System (Day 1-2)**

Build the octagonal direction system with rotation helpers:

```cpp
// include/octa-core/core/Direction.h
enum class Direction : uint8_t {
    N, NE, E, SE, S, SW, W, NW
};

inline Direction rotateClockwise(Direction d);
inline Direction rotateCounterClockwise(Direction d);
```

**Deliverables:**
- [x] `include/octa-core/core/Direction.h`
- [x] Comprehensive rotation tests (all 8 directions)

### **Step 1.3: Refactor GameCell (Day 2-3)**

Major refactoring to align with SRD v3.2:

**Key Changes:**
- `Player occupant_` → `CellState state_`
- Add `Direction direction_` member
- `std::vector<shared_ptr>` → `std::array<weak_ptr, 8>` for neighbors
- Update all method signatures

**Deliverables:**
- [x] Updated `include/octa-core/core/GameCell.h`
- [x] Updated `src/core/GameCell.cpp`
- [x] GameCell tests (state, direction, weak_ptr neighbors)

### **Step 1.4: Test & Validate (Day 4)**

Ensure all components build and pass tests:

```bash
# Build with new components
cmake --preset conan-release
cmake --build --preset conan-release

# Run tests  
ctest --preset conan-release --verbose
```

**Success Criteria:**
- ✅ All new enums compile and test
- ✅ Direction rotation functions work correctly
- ✅ GameCell refactoring complete and tested
- ✅ No breaking changes to existing functionality

---

## 🗺️ **Milestone P1.2: Map Interface Refactoring (Days 5-7)**

**Goal**: Update map interfaces to SRD v3.2 specification and ensure all existing code works.

### **Step 2.1: Refactor IGameMap Interface (Day 5)**

Update to SRD v3.2 specification:

```cpp
// include/octa-core/map/IGameMap.h
class IGameMap {
public:
    virtual std::shared_ptr<GameCell> at(const Coordinate& coord) const = 0;
    virtual size_t size() const = 0;
    // Remove: getCell(), getAllCells(), getSize(), getTotalCellCount()
};
```

**Deliverables:**
- [x] Updated `include/octa-core/map/IGameMap.h`
- [x] Updated interface tests

### **Step 2.2: Update GraphGameMap Implementation (Day 5-6)**

Implement new interface and handle GameCell changes:

**Key Updates:**
- Implement `at()` and `size()` methods
- Update neighbor linking for `weak_ptr` system
- Handle `CellState`/`Direction` initialization
- Remove deprecated methods

**Deliverables:**
- [x] Updated `include/octa-core/map/GraphGameMap.h`
- [x] Updated `src/map/GraphGameMap.cpp`
- [x] GraphGameMap tests (initialization, neighbor linking)

### **Step 2.3: Update ArrayGameMap Placeholder (Day 6)**

Simple interface compliance:

```cpp
// src/map/ArrayGameMap.cpp
std::shared_ptr<GameCell> ArrayGameMap::at(const Coordinate& coord) const {
    throw std::runtime_error("ArrayGameMap not implemented yet");
}

size_t ArrayGameMap::size() const {
    throw std::runtime_error("ArrayGameMap not implemented yet");  
}
```

**Deliverables:**
- [x] Updated `include/octa-core/map/ArrayGameMap.h`
- [x] Updated `src/map/ArrayGameMap.cpp`

### **Step 2.4: Fix GameEngine Dependencies (Day 7)**

Update GameEngine to use new map interface:

**Key Changes:**
- `gameMap_->getCell()` → `gameMap_->at()`
- `gameMap_->getTotalCellCount()` → `gameMap_->size()`
- Remove `getSize()` calls

**Deliverables:**
- [x] Updated `include/octa-core/engine/GameEngine.h`
- [x] Updated `src/engine/GameEngine.cpp`  
- [x] Updated `src/Main.cpp`

---

## 🎮 **Milestone P1.3: Game Logic Foundation (Days 8-12)**

**Goal**: Implement the core game logic system with VALIDATE_ONLY safety tier.

### **Step 3.1: Create Game Configuration (Day 8)**

Build the configuration system:

```cpp
// include/octa-core/model/GameConfig.h
enum class WinCondition { ELIMINATION, TURN_LIMIT_MAJORITY };
enum class SafetyLevel { VALIDATE_ONLY, LIGHT_UNDO, FULL_ROLLBACK };

struct GameConfig {
    WinCondition winCondition = WinCondition::ELIMINATION;
    int turnLimit = 60;
    bool stopOnEnemy = false;
    SafetyLevel safetyLevel = SafetyLevel::VALIDATE_ONLY;
};

struct GameResult {
    std::optional<Player> winner;
    std::string reason;
};
```

**Deliverables:**
- [x] `include/octa-core/model/GameConfig.h`
- [x] GameConfig tests

### **Step 3.2: Create Game Logic Interface (Day 8-9)**

Define the game logic contract:

```cpp
// include/octa-core/engine/IGameLogic.h
class IGameLogic {
public:
    virtual bool isValidMove(std::shared_ptr<const GameCell> cell, Player player) const = 0;
    virtual GameResult makeMove(std::shared_ptr<GameCell> cell, Player player) = 0;
    virtual bool isGameOver() const = 0;
    virtual Player getCurrentPlayer() const = 0;
    virtual void switchPlayer() = 0;
    virtual int getTurnCount() const = 0;
    virtual const GameConfig& getConfig() const = 0;
};
```

**Deliverables:**
- [x] `include/octa-core/engine/IGameLogic.h`
- [x] Interface documentation

### **Step 3.3: Implement OctaGameLogic (Day 9-11)**

Core game logic implementation:

**Key Components:**
- `isValidMove()` - Check if cell belongs to player
- `makeMove()` - Implement chain reaction algorithm
- `propagateChain()` - Handle direction-based propagation
- Basic game state management

**Chain Reaction Algorithm:**
1. Rotate initial cell clockwise
2. Follow direction to next cell
3. Apply stopping conditions:
   - Off-grid boundary
   - Owned by current player  
   - Already captured this turn
   - Enemy cell (if stopOnEnemy enabled)
4. Capture cell, rotate direction, continue

**Deliverables:**
- [x] `include/octa-core/engine/OctaGameLogic.h`
- [x] `src/engine/OctaGameLogic.cpp`
- [x] Helper functions for Player ↔ CellState conversion

### **Step 3.4: Update GameEngine Integration (Day 11-12)**

Integrate game logic with engine:

**Changes:**
- Add `std::shared_ptr<IGameLogic> gameLogic_` member
- Update constructor to accept logic
- Modify `run()` for basic game loop

**Deliverables:**
- [x] Updated `include/octa-core/engine/GameEngine.h`
- [x] Updated `src/engine/GameEngine.cpp`
- [x] Updated `src/Main.cpp` with logic creation

---

## 🧪 **Milestone P1.4: Testing & Integration (Days 13-15)**

**Goal**: Achieve ≥70% test coverage and ensure all components work together.

### **Step 4.1: Comprehensive Unit Tests (Day 13-14)**

**Test Categories:**

1. **Direction Tests**:
   ```cpp
   TEST(DirectionTests, RotateClockwise) {
       EXPECT_EQ(rotateClockwise(Direction::N), Direction::NE);
       // All 8 directions
   }
   ```

2. **GameCell Tests**:
   ```cpp
   TEST(GameCellTests, StateAndDirection) {
       GameCell cell(Coordinate(0,0));
       EXPECT_EQ(cell.getState(), CellState::NEUTRAL);
       cell.setState(CellState::PLAYER_1);
       EXPECT_EQ(cell.getState(), CellState::PLAYER_1);
   }
   ```

3. **GraphGameMap Tests**:
   ```cpp
   TEST(GraphGameMapTests, Initialization) {
       GraphGameMap map(2);
       EXPECT_EQ(map.size(), 25);
       EXPECT_NE(map.at(Coordinate(0,0)), nullptr);
   }
   ```

4. **Game Logic Tests**:
   ```cpp
   TEST(OctaGameLogicTests, IsValidMove) {
       // Test ownership validation
   }
   
   TEST(OctaGameLogicTests, MakeMove_BasicChain) {
       // Test chain reaction
   }
   
   TEST(OctaGameLogicTests, MakeMove_StoppingConditions) {
       // Test all stopping scenarios
   }
   ```

**Deliverables:**
- [x] Updated `tests/test_main.cpp`
- [x] New `tests/GameLogicTests.cpp`
- [x] Coverage report ≥70%

### **Step 4.2: Integration Testing (Day 14-15)**

**End-to-End Scenarios:**
- Create map → Initialize logic → Execute moves → Verify results
- Test error handling and edge cases
- Performance verification with benchmarks

**Deliverables:**
- [x] Integration test suite
- [x] Performance baseline measurements
- [x] Memory leak verification (Valgrind)

### **Step 4.3: Documentation & Cleanup (Day 15)**

**Final Steps:**
- Update all Doxygen comments
- Code formatting with clang-format
- Final build verification on all platforms

**Success Criteria:**
- ✅ All tests passing (≥70% coverage)
- ✅ Clean builds on all platforms
- ✅ No memory leaks detected
- ✅ Code style compliance
- ✅ Ready for Phase P2

---

## 📊 **Implementation Checklist**

### **Dependencies & Build System**
- [ ] Update CMakeLists.txt with new source files
- [ ] Ensure Conan dependencies still work
- [ ] Test builds on all platforms (CI)

### **Code Quality**
- [ ] All code follows Google C++ Style Guide
- [ ] Comprehensive Doxygen documentation
- [ ] No compiler warnings
- [ ] clang-format compliance

### **Testing Requirements**
- [ ] Unit tests for all public APIs
- [ ] Integration tests for game scenarios
- [ ] Error handling tests
- [ ] Performance regression tests
- [ ] Memory leak detection (Valgrind/Dr. Memory)

### **Deliverable Verification**
- [ ] SRD v3.2 compliance verified
- [ ] No breaking changes to Phase P0 infrastructure
- [ ] Ready for Phase P2 (Light Undo & Benchmarking)

---

## 🚀 **Ready to Implement?**

This strategy provides:
- **Clear milestones** with testable deliverables
- **Dependency management** to avoid integration issues  
- **Incremental validation** at each step
- **Comprehensive testing** strategy
- **Quality assurance** throughout development

**Next Step**: Begin with Milestone P1.1 - Core Data Models

Would you like me to start implementing the first milestone? 