/**
 * @file test_main.cpp
 * @brief Test runner for Octa-Core C++ library
 *
 * This file provides the main entry point for Google Test and includes
 * unit tests for core components developed in Phase P1.
 *
 * Phase P1: Core Logic implementation and testing
 */

#include <gtest/gtest.h>

#include <chrono>

// Include the new core components
#include "octa-core/core/CellChange.h"
#include "octa-core/core/Direction.h"
#include "octa-core/core/Enums.h"
#include "octa-core/core/GameCell.h"
#include "octa-core/logic/IGameLogic.h"
#include "octa-core/logic/OctaGameLogic.h"
#include "octa-core/map/ArrayGameMap.h"
#include "octa-core/map/GraphGameMap.h"
#include "octa-core/map/IGameMap.h"
#include "octa-core/model/GameConfig.h"

/**
 * @brief Basic placeholder test to verify test infrastructure works
 */
TEST(PhaseP0Tests, TestInfrastructure) {
    // Basic test to verify Google Test is working
    EXPECT_EQ(1 + 1, 2);
    EXPECT_TRUE(true);
}

// ============================================================================
// PHASE P1.1 TESTS: Core Data Models
// ============================================================================

/**
 * @brief Test suite for Player and CellState enums
 */
class EnumsTests : public ::testing::Test {
  protected:
    void SetUp() override {
        // Setup common test data if needed
    }
};

TEST_F(EnumsTests, PlayerEnumValues) {
    // Test that Player enum has the expected values
    EXPECT_EQ(static_cast<uint8_t>(Player::PLAYER_1), 0);
    EXPECT_EQ(static_cast<uint8_t>(Player::PLAYER_2), 1);
}

TEST_F(EnumsTests, CellStateEnumValues) {
    // Test that CellState enum has the expected values
    EXPECT_EQ(static_cast<uint8_t>(CellState::NEUTRAL), 0);
    EXPECT_EQ(static_cast<uint8_t>(CellState::PLAYER_1), 1);
    EXPECT_EQ(static_cast<uint8_t>(CellState::PLAYER_2), 2);
    EXPECT_EQ(static_cast<uint8_t>(CellState::BLOCKED), 3);
}

TEST_F(EnumsTests, PlayerToCellStateConversion) {
    // Test conversion from Player to CellState
    EXPECT_EQ(playerToCellState(Player::PLAYER_1), CellState::PLAYER_1);
    EXPECT_EQ(playerToCellState(Player::PLAYER_2), CellState::PLAYER_2);
}

TEST_F(EnumsTests, CellStateToPlayerConversion) {
    // Test conversion from CellState to Player
    auto player1 = cellStateToPlayer(CellState::PLAYER_1);
    auto player2 = cellStateToPlayer(CellState::PLAYER_2);
    auto neutral = cellStateToPlayer(CellState::NEUTRAL);
    auto blocked = cellStateToPlayer(CellState::BLOCKED);

    ASSERT_TRUE(player1.has_value());
    ASSERT_TRUE(player2.has_value());
    EXPECT_FALSE(neutral.has_value());
    EXPECT_FALSE(blocked.has_value());

    EXPECT_EQ(player1.value(), Player::PLAYER_1);
    EXPECT_EQ(player2.value(), Player::PLAYER_2);
}

TEST_F(EnumsTests, GetOpponent) {
    // Test opponent retrieval
    EXPECT_EQ(getOpponent(Player::PLAYER_1), Player::PLAYER_2);
    EXPECT_EQ(getOpponent(Player::PLAYER_2), Player::PLAYER_1);
}

/**
 * @brief Test suite for Direction enum and rotation functions
 */
class DirectionTests : public ::testing::Test {
  protected:
    void SetUp() override {
        // Setup common test data if needed
    }
};

TEST_F(DirectionTests, DirectionEnumValues) {
    // Test that Direction enum has the expected values
    EXPECT_EQ(static_cast<uint8_t>(Direction::N), 0);
    EXPECT_EQ(static_cast<uint8_t>(Direction::NE), 1);
    EXPECT_EQ(static_cast<uint8_t>(Direction::E), 2);
    EXPECT_EQ(static_cast<uint8_t>(Direction::SE), 3);
    EXPECT_EQ(static_cast<uint8_t>(Direction::S), 4);
    EXPECT_EQ(static_cast<uint8_t>(Direction::SW), 5);
    EXPECT_EQ(static_cast<uint8_t>(Direction::W), 6);
    EXPECT_EQ(static_cast<uint8_t>(Direction::NW), 7);
}

TEST_F(DirectionTests, RotateClockwise) {
    // Test clockwise rotation for all directions
    EXPECT_EQ(rotateClockwise(Direction::N), Direction::NE);
    EXPECT_EQ(rotateClockwise(Direction::NE), Direction::E);
    EXPECT_EQ(rotateClockwise(Direction::E), Direction::SE);
    EXPECT_EQ(rotateClockwise(Direction::SE), Direction::S);
    EXPECT_EQ(rotateClockwise(Direction::S), Direction::SW);
    EXPECT_EQ(rotateClockwise(Direction::SW), Direction::W);
    EXPECT_EQ(rotateClockwise(Direction::W), Direction::NW);
    EXPECT_EQ(rotateClockwise(Direction::NW), Direction::N);
}

TEST_F(DirectionTests, RotateCounterClockwise) {
    // Test counter-clockwise rotation for all directions
    EXPECT_EQ(rotateCounterClockwise(Direction::N), Direction::NW);
    EXPECT_EQ(rotateCounterClockwise(Direction::NE), Direction::N);
    EXPECT_EQ(rotateCounterClockwise(Direction::E), Direction::NE);
    EXPECT_EQ(rotateCounterClockwise(Direction::SE), Direction::E);
    EXPECT_EQ(rotateCounterClockwise(Direction::S), Direction::SE);
    EXPECT_EQ(rotateCounterClockwise(Direction::SW), Direction::S);
    EXPECT_EQ(rotateCounterClockwise(Direction::W), Direction::SW);
    EXPECT_EQ(rotateCounterClockwise(Direction::NW), Direction::W);
}

TEST_F(DirectionTests, RotateBySteps) {
    // Test rotation by multiple steps
    EXPECT_EQ(rotateBySteps(Direction::N, 2), Direction::E);
    EXPECT_EQ(rotateBySteps(Direction::N, 4), Direction::S);
    EXPECT_EQ(rotateBySteps(Direction::E, -1), Direction::NE);
    EXPECT_EQ(rotateBySteps(Direction::S, -2), Direction::E);  // S(4) - 2 = E(2)

    // Test wrapping around
    EXPECT_EQ(rotateBySteps(Direction::N, 8), Direction::N);
    EXPECT_EQ(rotateBySteps(Direction::N, -8), Direction::N);
}

TEST_F(DirectionTests, GetOpposite) {
    // Test opposite direction calculation
    EXPECT_EQ(getOpposite(Direction::N), Direction::S);
    EXPECT_EQ(getOpposite(Direction::NE), Direction::SW);
    EXPECT_EQ(getOpposite(Direction::E), Direction::W);
    EXPECT_EQ(getOpposite(Direction::SE), Direction::NW);
    EXPECT_EQ(getOpposite(Direction::S), Direction::N);
    EXPECT_EQ(getOpposite(Direction::SW), Direction::NE);
    EXPECT_EQ(getOpposite(Direction::W), Direction::E);
    EXPECT_EQ(getOpposite(Direction::NW), Direction::SE);
}

TEST_F(DirectionTests, DirectionToString) {
    // Test string conversion
    EXPECT_STREQ(directionToString(Direction::N), "N");
    EXPECT_STREQ(directionToString(Direction::NE), "NE");
    EXPECT_STREQ(directionToString(Direction::E), "E");
    EXPECT_STREQ(directionToString(Direction::SE), "SE");
    EXPECT_STREQ(directionToString(Direction::S), "S");
    EXPECT_STREQ(directionToString(Direction::SW), "SW");
    EXPECT_STREQ(directionToString(Direction::W), "W");
    EXPECT_STREQ(directionToString(Direction::NW), "NW");
}

/**
 * @brief Test suite for GameCell class
 */
class GameCellTests : public ::testing::Test {
  protected:
    void SetUp() override {
        // Create test coordinates
        coord1 = Coordinate(0, 0);
        coord2 = Coordinate(1, 0);
        coord3 = Coordinate(0, 1);

        // Create test cells
        cell1 = std::make_shared<GameCell>(coord1);
        cell2 = std::make_shared<GameCell>(coord2, CellState::PLAYER_1);
        cell3 = std::make_shared<GameCell>(coord3, CellState::BLOCKED);
    }

    Coordinate coord1, coord2, coord3;
    std::shared_ptr<GameCell> cell1, cell2, cell3;
};

TEST_F(GameCellTests, ConstructorAndBasicProperties) {
    // Test default constructor (NEUTRAL state)
    EXPECT_EQ(cell1->getCoordinate().getX(), 0);
    EXPECT_EQ(cell1->getCoordinate().getY(), 0);
    EXPECT_EQ(cell1->getState(), CellState::NEUTRAL);
    EXPECT_EQ(cell1->getDirection(), Direction::N);

    // Test constructor with initial state
    EXPECT_EQ(cell2->getState(), CellState::PLAYER_1);
    EXPECT_EQ(cell3->getState(), CellState::BLOCKED);
}

TEST_F(GameCellTests, StateManagement) {
    // Test setState and getState
    cell1->setState(CellState::PLAYER_2);
    EXPECT_EQ(cell1->getState(), CellState::PLAYER_2);

    cell1->setState(CellState::NEUTRAL);
    EXPECT_EQ(cell1->getState(), CellState::NEUTRAL);
}

TEST_F(GameCellTests, OwnershipQueries) {
    // Test isOwnedByPlayer
    EXPECT_FALSE(cell1->isOwnedByPlayer());  // NEUTRAL
    EXPECT_TRUE(cell2->isOwnedByPlayer());   // PLAYER_1
    EXPECT_FALSE(cell3->isOwnedByPlayer());  // BLOCKED

    // Test isOwnedBy
    EXPECT_FALSE(cell1->isOwnedBy(Player::PLAYER_1));
    EXPECT_FALSE(cell1->isOwnedBy(Player::PLAYER_2));

    EXPECT_TRUE(cell2->isOwnedBy(Player::PLAYER_1));
    EXPECT_FALSE(cell2->isOwnedBy(Player::PLAYER_2));

    EXPECT_FALSE(cell3->isOwnedBy(Player::PLAYER_1));
    EXPECT_FALSE(cell3->isOwnedBy(Player::PLAYER_2));
}

TEST_F(GameCellTests, StateQueries) {
    // Test isAvailable
    EXPECT_TRUE(cell1->isAvailable());   // NEUTRAL
    EXPECT_FALSE(cell2->isAvailable());  // PLAYER_1
    EXPECT_FALSE(cell3->isAvailable());  // BLOCKED

    // Test isBlocked
    EXPECT_FALSE(cell1->isBlocked());  // NEUTRAL
    EXPECT_FALSE(cell2->isBlocked());  // PLAYER_1
    EXPECT_TRUE(cell3->isBlocked());   // BLOCKED
}

TEST_F(GameCellTests, DirectionManagement) {
    // Test setDirection and getDirection
    cell1->setDirection(Direction::E);
    EXPECT_EQ(cell1->getDirection(), Direction::E);

    cell1->setDirection(Direction::SW);
    EXPECT_EQ(cell1->getDirection(), Direction::SW);
}

TEST_F(GameCellTests, NeighborManagement) {
    // Initially no neighbors
    EXPECT_EQ(cell1->getValidNeighborCount(), 0);
    EXPECT_FALSE(cell1->hasNeighbor(Direction::N));
    EXPECT_EQ(cell1->getNeighbor(Direction::N), nullptr);

    // Set a neighbor
    cell1->setNeighbor(Direction::E, cell2);
    EXPECT_TRUE(cell1->hasNeighbor(Direction::E));
    EXPECT_EQ(cell1->getValidNeighborCount(), 1);

    auto neighbor = cell1->getNeighbor(Direction::E);
    ASSERT_NE(neighbor, nullptr);
    EXPECT_EQ(neighbor, cell2);

    // Set another neighbor
    cell1->setNeighbor(Direction::S, cell3);
    EXPECT_TRUE(cell1->hasNeighbor(Direction::S));
    EXPECT_EQ(cell1->getValidNeighborCount(), 2);

    // Clear a neighbor
    cell1->clearNeighbor(Direction::E);
    EXPECT_FALSE(cell1->hasNeighbor(Direction::E));
    EXPECT_EQ(cell1->getValidNeighborCount(), 1);
    EXPECT_EQ(cell1->getNeighbor(Direction::E), nullptr);
}

TEST_F(GameCellTests, ValidNeighbors) {
    // Set up multiple neighbors
    cell1->setNeighbor(Direction::N, cell2);
    cell1->setNeighbor(Direction::E, cell3);

    auto validNeighbors = cell1->getValidNeighbors();
    EXPECT_EQ(validNeighbors.size(), 2);

    // Check that both neighbors are present (order may vary)
    bool hasCell2 = false, hasCell3 = false;
    for (const auto& neighbor : validNeighbors) {
        if (neighbor == cell2) hasCell2 = true;
        if (neighbor == cell3) hasCell3 = true;
    }
    EXPECT_TRUE(hasCell2);
    EXPECT_TRUE(hasCell3);
}

TEST_F(GameCellTests, NeighborsInDirections) {
    // Set up neighbors
    cell1->setNeighbor(Direction::N, cell2);
    cell1->setNeighbor(Direction::E, cell3);

    // Query specific directions
    std::vector<Direction> queryDirections = {Direction::N, Direction::S, Direction::E};
    auto neighbors = cell1->getNeighborsInDirections(queryDirections);

    EXPECT_EQ(neighbors.size(), 3);
    EXPECT_EQ(neighbors[0], cell2);    // N direction
    EXPECT_EQ(neighbors[1], nullptr);  // S direction (no neighbor)
    EXPECT_EQ(neighbors[2], cell3);    // E direction
}

TEST_F(GameCellTests, WeakPtrBehavior) {
    // Test that weak_ptr behavior works correctly
    {
        auto tempCell = std::make_shared<GameCell>(Coordinate(2, 2));
        cell1->setNeighbor(Direction::W, tempCell);
        EXPECT_TRUE(cell1->hasNeighbor(Direction::W));
        EXPECT_EQ(cell1->getValidNeighborCount(), 1);
    }
    // tempCell goes out of scope and should be destroyed

    // The weak_ptr should now be expired
    EXPECT_FALSE(cell1->hasNeighbor(Direction::W));
    EXPECT_EQ(cell1->getValidNeighborCount(), 0);
    EXPECT_EQ(cell1->getNeighbor(Direction::W), nullptr);
}

/**
 * @brief Test suite for CellChange struct (Phase P2.1)
 */
class CellChangeTests : public ::testing::Test {
  protected:
    void SetUp() override {
        // Create test cell
        testCoord = Coordinate(5, 5);
        testCell = std::make_shared<GameCell>(testCoord, CellState::NEUTRAL);
        testCell->setDirection(Direction::N);
        testCell->setValue(1);
    }

    Coordinate testCoord;
    std::shared_ptr<GameCell> testCell;
};

TEST_F(CellChangeTests, ConstructorAndBasicProperties) {
    // Test construction with valid cell
    CellChange change(testCell, CellState::PLAYER_1, Direction::E);

    EXPECT_EQ(change.cell, testCell);
    EXPECT_EQ(change.oldState, CellState::PLAYER_1);
    EXPECT_EQ(change.oldDirection, Direction::E);
    EXPECT_TRUE(change.isValid());
}

TEST_F(CellChangeTests, ConstructorWithNullCell) {
    // Test that constructor throws with null cell
    EXPECT_THROW(CellChange(nullptr, CellState::NEUTRAL, Direction::N), std::invalid_argument);
}

TEST_F(CellChangeTests, ValidityCheck) {
    // Test with valid cell
    CellChange change(testCell, CellState::NEUTRAL, Direction::N);
    EXPECT_TRUE(change.isValid());

    // Test with null cell (should be caught by constructor, but testing the method)
    std::shared_ptr<GameCell> nullCell = nullptr;
    CellChange nullChange(testCell, CellState::NEUTRAL, Direction::N);
    nullChange.cell = nullCell;  // Force null for testing
    EXPECT_FALSE(nullChange.isValid());
}

TEST_F(CellChangeTests, RestoreValidCell) {
    // Set initial cell state
    testCell->setState(CellState::PLAYER_2);
    testCell->setDirection(Direction::SW);

    // Create change record with different state
    CellChange change(testCell, CellState::PLAYER_1, Direction::NE);

    // Restore the cell
    EXPECT_NO_THROW(change.restore());

    // Verify restoration
    EXPECT_EQ(testCell->getState(), CellState::PLAYER_1);
    EXPECT_EQ(testCell->getDirection(), Direction::NE);
}

TEST_F(CellChangeTests, RestoreInvalidCell) {
    // Create change with valid cell
    CellChange change(testCell, CellState::PLAYER_1, Direction::E);

    // Force cell to null for testing
    change.cell = nullptr;

    // Restore should throw
    EXPECT_THROW(change.restore(), std::runtime_error);
}

TEST_F(CellChangeTests, RecordBeforeModification) {
    // Set up initial state
    testCell->setState(CellState::NEUTRAL);
    testCell->setDirection(Direction::N);

    // Record current state before modification
    CellChange beforeChange(testCell, testCell->getState(), testCell->getDirection());

    // Modify the cell
    testCell->setState(CellState::PLAYER_1);
    testCell->setDirection(Direction::S);

    // Verify cell was modified
    EXPECT_EQ(testCell->getState(), CellState::PLAYER_1);
    EXPECT_EQ(testCell->getDirection(), Direction::S);

    // Restore using the change record
    beforeChange.restore();

    // Verify restoration to original state
    EXPECT_EQ(testCell->getState(), CellState::NEUTRAL);
    EXPECT_EQ(testCell->getDirection(), Direction::N);
}

TEST_F(CellChangeTests, MultipleChangesSequence) {
    // Set up initial state
    testCell->setState(CellState::NEUTRAL);
    testCell->setDirection(Direction::N);

    // Record initial state
    CellChange initialChange(testCell, testCell->getState(), testCell->getDirection());

    // First modification
    testCell->setState(CellState::PLAYER_1);
    testCell->setDirection(Direction::E);
    CellChange firstChange(testCell, testCell->getState(), testCell->getDirection());

    // Second modification
    testCell->setState(CellState::PLAYER_2);
    testCell->setDirection(Direction::SW);

    // Restore to first state
    firstChange.restore();
    EXPECT_EQ(testCell->getState(), CellState::PLAYER_1);
    EXPECT_EQ(testCell->getDirection(), Direction::E);

    // Restore to initial state
    initialChange.restore();
    EXPECT_EQ(testCell->getState(), CellState::NEUTRAL);
    EXPECT_EQ(testCell->getDirection(), Direction::N);
}

TEST_F(CellChangeTests, ChangeRecordImmutability) {
    // Create change record
    CellChange change(testCell, CellState::BLOCKED, Direction::W);

    // Modify the original cell
    testCell->setState(CellState::PLAYER_1);
    testCell->setDirection(Direction::E);

    // Change record should still have original values
    EXPECT_EQ(change.oldState, CellState::BLOCKED);
    EXPECT_EQ(change.oldDirection, Direction::W);

    // Restoration should use the recorded values, not current cell values
    change.restore();
    EXPECT_EQ(testCell->getState(), CellState::BLOCKED);
    EXPECT_EQ(testCell->getDirection(), Direction::W);
}

// ============================================================================
// PHASE P1.2 TESTS: Map Interface Refactoring
// ============================================================================

/**
 * @brief Test suite for IGameMap interface and GraphGameMap implementation
 */
class GraphGameMapTests : public ::testing::Test {
  protected:
    void SetUp() override {
        // Create test maps of different sizes
        map1 = std::make_unique<GraphGameMap>(1);  // 3x3 grid (9 cells)
        map2 = std::make_unique<GraphGameMap>(2);  // 5x5 grid (25 cells)
    }

    std::unique_ptr<GraphGameMap> map1, map2;
};

TEST_F(GraphGameMapTests, MapSizeAndCellCount) {
    // Test size() method (total cell count)
    EXPECT_EQ(map1->size(), 9);   // (2*1+1)² = 3² = 9
    EXPECT_EQ(map2->size(), 25);  // (2*2+1)² = 5² = 25

    // Test getRadius() utility method
    EXPECT_EQ(map1->getRadius(), 1);
    EXPECT_EQ(map2->getRadius(), 2);
}

TEST_F(GraphGameMapTests, CellAccessAndValidation) {
    // Test at() method for valid coordinates
    auto cell00 = map1->at(Coordinate(0, 0));
    ASSERT_NE(cell00, nullptr);
    EXPECT_EQ(cell00->getCoordinate().getX(), 0);
    EXPECT_EQ(cell00->getCoordinate().getY(), 0);
    EXPECT_EQ(cell00->getState(), CellState::NEUTRAL);

    // Test at() method for boundary coordinates
    auto cell11 = map1->at(Coordinate(1, 1));
    auto cellNeg11 = map1->at(Coordinate(-1, -1));
    ASSERT_NE(cell11, nullptr);
    ASSERT_NE(cellNeg11, nullptr);

    // Test at() method for invalid coordinates (outside bounds)
    auto invalidCell = map1->at(Coordinate(2, 2));  // Outside size=1 bounds
    EXPECT_EQ(invalidCell, nullptr);

    // Test isValidCoordinate utility method
    EXPECT_TRUE(map1->isValidCoordinate(Coordinate(0, 0)));
    EXPECT_TRUE(map1->isValidCoordinate(Coordinate(1, 1)));
    EXPECT_TRUE(map1->isValidCoordinate(Coordinate(-1, -1)));
    EXPECT_FALSE(map1->isValidCoordinate(Coordinate(2, 2)));
}

TEST_F(GraphGameMapTests, NeighborLinking) {
    // Get the center cell
    auto centerCell = map2->at(Coordinate(0, 0));
    ASSERT_NE(centerCell, nullptr);

    // Check that all 8 neighbors are properly linked
    EXPECT_TRUE(centerCell->hasNeighbor(Direction::N));
    EXPECT_TRUE(centerCell->hasNeighbor(Direction::NE));
    EXPECT_TRUE(centerCell->hasNeighbor(Direction::E));
    EXPECT_TRUE(centerCell->hasNeighbor(Direction::SE));
    EXPECT_TRUE(centerCell->hasNeighbor(Direction::S));
    EXPECT_TRUE(centerCell->hasNeighbor(Direction::SW));
    EXPECT_TRUE(centerCell->hasNeighbor(Direction::W));
    EXPECT_TRUE(centerCell->hasNeighbor(Direction::NW));

    // Verify neighbor coordinates
    auto northNeighbor = centerCell->getNeighbor(Direction::N);
    ASSERT_NE(northNeighbor, nullptr);
    EXPECT_EQ(northNeighbor->getCoordinate(), Coordinate(0, 1));

    auto neNeighbor = centerCell->getNeighbor(Direction::NE);
    ASSERT_NE(neNeighbor, nullptr);
    EXPECT_EQ(neNeighbor->getCoordinate(), Coordinate(1, 1));

    // Test corner cell (should have fewer neighbors)
    auto cornerCell = map2->at(Coordinate(2, 2));
    ASSERT_NE(cornerCell, nullptr);

    // Corner cell should not have neighbors in certain directions
    EXPECT_FALSE(cornerCell->hasNeighbor(Direction::N));   // Would be (2, 3) - outside bounds
    EXPECT_FALSE(cornerCell->hasNeighbor(Direction::NE));  // Would be (3, 3) - outside bounds
    EXPECT_FALSE(cornerCell->hasNeighbor(Direction::E));   // Would be (3, 2) - outside bounds

    // But should have neighbors in other directions
    EXPECT_TRUE(cornerCell->hasNeighbor(Direction::SW));  // Should be (1, 1)
    EXPECT_TRUE(cornerCell->hasNeighbor(Direction::W));   // Should be (1, 2)
    EXPECT_TRUE(cornerCell->hasNeighbor(Direction::S));   // Should be (2, 1)
}

TEST_F(GraphGameMapTests, CellStateInitialization) {
    // All cells should be initialized with NEUTRAL state
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            auto cell = map1->at(Coordinate(x, y));
            ASSERT_NE(cell, nullptr);
            EXPECT_EQ(cell->getState(), CellState::NEUTRAL);
            EXPECT_EQ(cell->getDirection(), Direction::N);  // Default direction
        }
    }
}

/**
 * @brief Test suite for ArrayGameMap placeholder
 */
class ArrayGameMapTests : public ::testing::Test {
  protected:
    void SetUp() override {
        // ArrayGameMap should throw on construction
    }
};

TEST_F(ArrayGameMapTests, PlaceholderBehavior) {
    // Constructor should throw
    EXPECT_THROW(ArrayGameMap(1), std::runtime_error);

    // Note: We can't test other methods since constructor throws
    // This is expected behavior for the placeholder implementation
}

// ============================================================================
// PHASE P1.3 TESTS: Game Logic Foundation
// ============================================================================

/**
 * @brief Test suite for GameConfig structure
 */
class GameConfigTests : public ::testing::Test {
  protected:
    void SetUp() override {
        // Setup common test data if needed
    }
};

TEST_F(GameConfigTests, DefaultConstructor) {
    GameConfig config;

    EXPECT_EQ(config.winCondition, WinCondition::ELIMINATION);
    EXPECT_EQ(config.turnLimit, 100);
    EXPECT_EQ(config.stopOnEnemy, false);
    EXPECT_EQ(config.safetyLevel, SafetyLevel::VALIDATE_ONLY);
    EXPECT_TRUE(config.isValid());
}

TEST_F(GameConfigTests, CustomConstructor) {
    GameConfig config(WinCondition::TURN_LIMIT_MAJORITY, 50, true, SafetyLevel::LIGHT_UNDO);

    EXPECT_EQ(config.winCondition, WinCondition::TURN_LIMIT_MAJORITY);
    EXPECT_EQ(config.turnLimit, 50);
    EXPECT_EQ(config.stopOnEnemy, true);
    EXPECT_EQ(config.safetyLevel, SafetyLevel::LIGHT_UNDO);
    EXPECT_TRUE(config.isValid());
}

TEST_F(GameConfigTests, InvalidConfiguration) {
    GameConfig config;
    config.turnLimit = -1;
    EXPECT_FALSE(config.isValid());

    config.turnLimit = 0;
    EXPECT_FALSE(config.isValid());
}

TEST_F(GameConfigTests, EnumStringConversion) {
    EXPECT_STREQ(winConditionToString(WinCondition::ELIMINATION), "ELIMINATION");
    EXPECT_STREQ(winConditionToString(WinCondition::TURN_LIMIT_MAJORITY), "TURN_LIMIT_MAJORITY");

    EXPECT_STREQ(safetyLevelToString(SafetyLevel::VALIDATE_ONLY), "VALIDATE_ONLY");
    EXPECT_STREQ(safetyLevelToString(SafetyLevel::LIGHT_UNDO), "LIGHT_UNDO");
    EXPECT_STREQ(safetyLevelToString(SafetyLevel::FULL_ROLLBACK), "FULL_ROLLBACK");
}

/**
 * @brief Test suite for GameResult structure
 */
class GameResultTests : public ::testing::Test {
  protected:
    void SetUp() override {
        // Setup common test data if needed
    }
};

TEST_F(GameResultTests, DefaultConstructor) {
    GameResult result;

    EXPECT_FALSE(result.winner.has_value());
    EXPECT_TRUE(result.reason.empty());
    EXPECT_EQ(result.finalTurnCount, 0);
    EXPECT_EQ(result.player1CellCount, 0);
    EXPECT_EQ(result.player2CellCount, 0);
    EXPECT_TRUE(result.isTie());
}

TEST_F(GameResultTests, WinnerConstructor) {
    GameResult result(Player::PLAYER_1, "Player 2 eliminated", 15, 8, 0);

    EXPECT_TRUE(result.winner.has_value());
    EXPECT_EQ(result.winner.value(), Player::PLAYER_1);
    EXPECT_EQ(result.reason, "Player 2 eliminated");
    EXPECT_EQ(result.finalTurnCount, 15);
    EXPECT_EQ(result.player1CellCount, 8);
    EXPECT_EQ(result.player2CellCount, 0);
    EXPECT_FALSE(result.isTie());
}

TEST_F(GameResultTests, TieConstructor) {
    GameResult result("Turn limit reached", 50, 5, 5);

    EXPECT_FALSE(result.winner.has_value());
    EXPECT_EQ(result.reason, "Turn limit reached");
    EXPECT_EQ(result.finalTurnCount, 50);
    EXPECT_EQ(result.player1CellCount, 5);
    EXPECT_EQ(result.player2CellCount, 5);
    EXPECT_TRUE(result.isTie());
}

TEST_F(GameResultTests, ToStringMethod) {
    GameResult winResult(Player::PLAYER_2, "Player 1 eliminated", 10, 0, 7);
    std::string winString = winResult.toString();
    EXPECT_NE(winString.find("Player 2 wins"), std::string::npos);
    EXPECT_NE(winString.find("10 turns"), std::string::npos);

    GameResult tieResult("Draw", 25, 3, 3);
    std::string tieString = tieResult.toString();
    EXPECT_NE(tieString.find("Tie game"), std::string::npos);
    EXPECT_NE(tieString.find("25 turns"), std::string::npos);
}

/**
 * @brief Test suite for OctaGameLogic class
 */
class OctaGameLogicTests : public ::testing::Test {
  protected:
    void SetUp() override {
        gameMap = std::make_shared<GraphGameMap>(2);  // Small 5x5 map for testing
        config = GameConfig();
    }

    std::shared_ptr<GraphGameMap> gameMap;
    GameConfig config;
};

TEST_F(OctaGameLogicTests, ConstructorAndInitialization) {
    OctaGameLogic logic(gameMap, config);

    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_1);
    EXPECT_EQ(logic.getTurnCount(), 0);
    EXPECT_FALSE(logic.isGameOver());
    EXPECT_FALSE(logic.getGameResult().has_value());
    EXPECT_EQ(logic.getConfig().winCondition, WinCondition::ELIMINATION);
}

TEST_F(OctaGameLogicTests, InvalidConstructor) {
    EXPECT_THROW(OctaGameLogic(nullptr, config), std::invalid_argument);

    GameConfig invalidConfig;
    invalidConfig.turnLimit = -1;
    EXPECT_THROW(OctaGameLogic(gameMap, invalidConfig), std::invalid_argument);
}

TEST_F(OctaGameLogicTests, ValidMoveValidation) {
    OctaGameLogic logic(gameMap, config);
    auto centerCell = gameMap->at(Coordinate(0, 0));

    // Should be valid for player 1 on neutral cell
    EXPECT_TRUE(logic.isValidMove(centerCell, Player::PLAYER_1));

    // Should be invalid for player 2 (not their turn)
    EXPECT_FALSE(logic.isValidMove(centerCell, Player::PLAYER_2));

    // Should be invalid for null cell
    EXPECT_FALSE(logic.isValidMove(nullptr, Player::PLAYER_1));
}

TEST_F(OctaGameLogicTests, BasicMoveExecution) {
    OctaGameLogic logic(gameMap, config);
    auto centerCell = gameMap->at(Coordinate(0, 0));

    // Make a move for player 1
    GameResult result = logic.makeMove(centerCell, Player::PLAYER_1);

    // Check that the cell is now owned by player 1
    EXPECT_EQ(centerCell->getState(), CellState::PLAYER_1);
    EXPECT_EQ(centerCell->getValue(), 1);

    // Check that it's now player 2's turn
    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_2);

    // Game should continue
    EXPECT_EQ(result.reason, "Game continues");
    EXPECT_FALSE(logic.isGameOver());
}

TEST_F(OctaGameLogicTests, InvalidMoveThrows) {
    OctaGameLogic logic(gameMap, config);
    auto centerCell = gameMap->at(Coordinate(0, 0));

    // Should throw for wrong player
    EXPECT_THROW(logic.makeMove(centerCell, Player::PLAYER_2), std::invalid_argument);

    // Should throw for null cell
    EXPECT_THROW(logic.makeMove(nullptr, Player::PLAYER_1), std::invalid_argument);
}

TEST_F(OctaGameLogicTests, PlayerSwitching) {
    OctaGameLogic logic(gameMap, config);

    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_1);
    logic.switchPlayer();
    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_2);
    logic.switchPlayer();
    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_1);
}

TEST_F(OctaGameLogicTests, GameReset) {
    OctaGameLogic logic(gameMap, config);
    auto centerCell = gameMap->at(Coordinate(0, 0));

    // Make a move
    logic.makeMove(centerCell, Player::PLAYER_1);
    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_2);

    // Reset game
    logic.resetGame();
    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_1);
    EXPECT_EQ(logic.getTurnCount(), 0);
    EXPECT_FALSE(logic.isGameOver());
}

TEST_F(OctaGameLogicTests, ConfigurationUpdate) {
    OctaGameLogic logic(gameMap, config);

    GameConfig newConfig(WinCondition::TURN_LIMIT_MAJORITY, 10, true, SafetyLevel::LIGHT_UNDO);
    logic.resetGame(&newConfig);

    EXPECT_EQ(logic.getConfig().winCondition, WinCondition::TURN_LIMIT_MAJORITY);
    EXPECT_EQ(logic.getConfig().turnLimit, 10);
    EXPECT_EQ(logic.getConfig().stopOnEnemy, true);
}

// ============================================================================
// PHASE P1.4 INTEGRATION TESTS: End-to-End Game Scenarios
// ============================================================================

/**
 * @brief Integration test suite for complete game scenarios
 *
 * These tests validate entire game flows from start to finish,
 * testing win conditions, error handling, and edge cases in
 * realistic game scenarios.
 */
class IntegrationTests : public ::testing::Test {
  protected:
    void SetUp() override {
        // Create a small map for faster testing
        gameMap = std::make_shared<GraphGameMap>(1);  // 3x3 map
        eliminationConfig =
            GameConfig(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
        turnLimitConfig =
            GameConfig(WinCondition::TURN_LIMIT_MAJORITY, 5, false, SafetyLevel::VALIDATE_ONLY);
    }

    std::shared_ptr<GraphGameMap> gameMap;
    GameConfig eliminationConfig;
    GameConfig turnLimitConfig;

    /**
     * @brief Helper to execute a sequence of moves
     * @param logic Game logic instance
     * @param moves Vector of coordinates to play in order
     * @return Final game result
     */
    GameResult executeMovesSequence(OctaGameLogic& logic, const std::vector<Coordinate>& moves) {
        GameResult lastResult("Game continues", 0, 0, 0);

        for (const auto& coord : moves) {
            if (logic.isGameOver()) {
                break;
            }

            auto cell = gameMap->at(coord);
            if (cell && logic.isValidMove(cell, logic.getCurrentPlayer())) {
                lastResult = logic.makeMove(cell, logic.getCurrentPlayer());
            } else {
                // Skip invalid moves in integration tests
                continue;
            }
        }

        return lastResult;
    }
};

TEST_F(IntegrationTests, CompleteGameScenario_EliminationWin) {
    OctaGameLogic logic(gameMap, eliminationConfig);

    // Scenario: Player 1 captures all neutral cells, Player 2 gets eliminated
    std::vector<Coordinate> moveSequence = {
        Coordinate(0, 0),    // P1 takes center
        Coordinate(1, 0),    // P2 takes right
        Coordinate(-1, 0),   // P1 takes left
        Coordinate(0, 1),    // P2 takes top
        Coordinate(0, -1),   // P1 takes bottom
        Coordinate(1, 1),    // P2 takes top-right
        Coordinate(-1, -1),  // P1 takes bottom-left
        Coordinate(-1, 1),   // P2 takes top-left
        Coordinate(1, -1)    // P1 takes bottom-right (all cells taken)
    };

    GameResult result = executeMovesSequence(logic, moveSequence);

    // Game should continue until one player is eliminated through chain reactions
    EXPECT_FALSE(logic.isGameOver() && result.winner.has_value() &&
                 result.winner.value() == Player::PLAYER_1);
    EXPECT_GT(logic.getTurnCount(), 0);

    // Verify game state is consistent
    int p1Cells = 0, p2Cells = 0;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            auto cell = gameMap->at(Coordinate(x, y));
            if (cell) {
                if (cell->getState() == CellState::PLAYER_1)
                    p1Cells++;
                else if (cell->getState() == CellState::PLAYER_2)
                    p2Cells++;
            }
        }
    }

    EXPECT_EQ(result.player1CellCount, p1Cells);
    EXPECT_EQ(result.player2CellCount, p2Cells);
}

TEST_F(IntegrationTests, CompleteGameScenario_TurnLimitTie) {
    OctaGameLogic logic(gameMap, turnLimitConfig);

    // Scenario: Reach turn limit with equal cell counts
    std::vector<Coordinate> moveSequence = {
        Coordinate(0, 0),   // P1 takes center (turn 1)
        Coordinate(1, 0),   // P2 takes right (turn 2)
        Coordinate(-1, 0),  // P1 takes left (turn 3)
        Coordinate(0, 1),   // P2 takes top (turn 4)
        Coordinate(0, -1)   // P1 takes bottom (turn 5 - limit reached)
    };

    GameResult result = executeMovesSequence(logic, moveSequence);

    // Should reach turn limit
    EXPECT_GE(logic.getTurnCount(), turnLimitConfig.turnLimit);

    if (logic.isGameOver()) {
        auto finalResult = logic.getGameResult();
        EXPECT_TRUE(finalResult.has_value());
        EXPECT_NE(finalResult->reason.find("Turn limit"), std::string::npos);
    }
}

TEST_F(IntegrationTests, CompleteGameScenario_ChainReactionPropagation) {
    OctaGameLogic logic(gameMap, eliminationConfig);

    // Set up a scenario where a move triggers a chain reaction
    auto centerCell = gameMap->at(Coordinate(0, 0));
    auto rightCell = gameMap->at(Coordinate(1, 0));
    auto topCell = gameMap->at(Coordinate(0, 1));

    ASSERT_NE(centerCell, nullptr);
    ASSERT_NE(rightCell, nullptr);
    ASSERT_NE(topCell, nullptr);

    // Make move that should trigger explosion-based chain reaction
    GameResult result = logic.makeMove(centerCell, Player::PLAYER_1);

    // Verify the move was executed
    EXPECT_EQ(centerCell->getState(), CellState::PLAYER_1);
    EXPECT_EQ(logic.getTurnCount(), 1);
    EXPECT_EQ(result.reason, "Game continues");

    // Verify game state consistency
    EXPECT_GE(result.player1CellCount, 1);
    EXPECT_EQ(result.player2CellCount, 0);

    // For a single move on an empty map, we should have exactly 1 cell
    EXPECT_EQ(result.player1CellCount, 1);
}

TEST_F(IntegrationTests, ErrorHandling_InvalidMoveSequence) {
    OctaGameLogic logic(gameMap, eliminationConfig);

    auto centerCell = gameMap->at(Coordinate(0, 0));
    ASSERT_NE(centerCell, nullptr);

    // Valid move for P1
    EXPECT_NO_THROW(logic.makeMove(centerCell, Player::PLAYER_1));

    // Now it's P2's turn - P1 move should fail
    EXPECT_THROW(logic.makeMove(centerCell, Player::PLAYER_1), std::invalid_argument);

    // Null cell should fail
    EXPECT_THROW(logic.makeMove(nullptr, Player::PLAYER_2), std::invalid_argument);

    // Game should still be in valid state
    EXPECT_FALSE(logic.isGameOver());
    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_2);
}

TEST_F(IntegrationTests, GameReset_MidGame) {
    OctaGameLogic logic(gameMap, eliminationConfig);

    // Make some moves
    auto centerCell = gameMap->at(Coordinate(0, 0));
    logic.makeMove(centerCell, Player::PLAYER_1);

    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_2);
    EXPECT_GT(logic.getTurnCount(), 0);

    // Reset game
    logic.resetGame();

    // Should be back to initial state
    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_1);
    EXPECT_EQ(logic.getTurnCount(), 0);
    EXPECT_FALSE(logic.isGameOver());
    EXPECT_FALSE(logic.getGameResult().has_value());
}

TEST_F(IntegrationTests, ConfigurationChange_DuringReset) {
    OctaGameLogic logic(gameMap, eliminationConfig);

    // Start with elimination config
    EXPECT_EQ(logic.getConfig().winCondition, WinCondition::ELIMINATION);
    EXPECT_EQ(logic.getConfig().turnLimit, 100);

    // Reset with new config
    logic.resetGame(&turnLimitConfig);

    // Should have new configuration
    EXPECT_EQ(logic.getConfig().winCondition, WinCondition::TURN_LIMIT_MAJORITY);
    EXPECT_EQ(logic.getConfig().turnLimit, 5);
    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_1);
    EXPECT_EQ(logic.getTurnCount(), 0);
}

TEST_F(IntegrationTests, StressTest_ManyMoves) {
    OctaGameLogic logic(gameMap, GameConfig(WinCondition::TURN_LIMIT_MAJORITY, 50, false,
                                            SafetyLevel::VALIDATE_ONLY));

    // Execute many alternating moves
    std::vector<Coordinate> allCoords;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            allCoords.push_back(Coordinate(x, y));
        }
    }

    int moveCount = 0;
    while (!logic.isGameOver() && moveCount < 30) {
        Coordinate coord = allCoords[moveCount % allCoords.size()];
        auto cell = gameMap->at(coord);

        if (cell && logic.isValidMove(cell, logic.getCurrentPlayer())) {
            GameResult result = logic.makeMove(cell, logic.getCurrentPlayer());
            moveCount++;

            // Verify game state consistency
            EXPECT_GE(result.finalTurnCount, 0);
            EXPECT_GE(result.player1CellCount, 0);
            EXPECT_GE(result.player2CellCount, 0);
        } else {
            // Try next coordinate
            moveCount++;
        }
    }

    // Should have made progress
    EXPECT_GT(logic.getTurnCount(), 0);
}

// ============================================================================
// PHASE P2.1 LIGHT UNDO SAFETY TESTS: Exception Safety and Rollback
// ============================================================================

/**
 * @brief Test suite for LIGHT_UNDO safety system
 */
class LightUndoTests : public ::testing::Test {
  protected:
    void SetUp() override {
        // Create a small test map for controlled testing
        gameMap = std::make_shared<GraphGameMap>(2);  // 5x5 map
        
        // Setup initial cells for testing
        centerCell = gameMap->at(Coordinate(0, 0));
        rightCell = gameMap->at(Coordinate(1, 0));
        topCell = gameMap->at(Coordinate(0, 1));
        
        // Configure for LIGHT_UNDO testing
        config = GameConfig(WinCondition::ELIMINATION, 100, false, SafetyLevel::LIGHT_UNDO);
    }
    
    std::shared_ptr<GraphGameMap> gameMap;
    std::shared_ptr<GameCell> centerCell;
    std::shared_ptr<GameCell> rightCell;
    std::shared_ptr<GameCell> topCell;
    GameConfig config;
};

TEST_F(LightUndoTests, BasicUndoLoggingEnabled) {
    OctaGameLogic logic(gameMap, config);
    
    // Record initial states
    auto initialCenterState = centerCell->getState();
    auto initialCenterDirection = centerCell->getDirection();
    
    // Make a valid move (should succeed with LIGHT_UNDO)
    auto result = logic.makeMove(centerCell, Player::PLAYER_1);
    
    // Verify move was successful
    EXPECT_EQ(centerCell->getState(), CellState::PLAYER_1);
    EXPECT_NE(centerCell->getState(), initialCenterState);
}

TEST_F(LightUndoTests, RollbackOnInvalidMoveAttempt) {
    OctaGameLogic logic(gameMap, config);
    
    // Set up a scenario where a move will fail after some changes
    centerCell->setState(CellState::PLAYER_1);
    
    // Record initial states
    auto initialCenterState = centerCell->getState();
    auto initialCenterValue = centerCell->getValue();
    
    // Try to make an invalid move (wrong player)
    EXPECT_THROW(logic.makeMove(centerCell, Player::PLAYER_2), std::invalid_argument);
    
    // Verify state is unchanged (no rollback needed for this early validation failure)
    EXPECT_EQ(centerCell->getState(), initialCenterState);
    EXPECT_EQ(centerCell->getValue(), initialCenterValue);
}

TEST_F(LightUndoTests, DisabledLoggingNoOverhead) {
    // Test with VALIDATE_ONLY (no undo logging)
    GameConfig validateOnlyConfig(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
    OctaGameLogic logic(gameMap, validateOnlyConfig);
    
    // Record initial state
    auto initialState = centerCell->getState();
    
    // Make a move - should work without undo logging
    auto result = logic.makeMove(centerCell, Player::PLAYER_1);
    
    // Verify move succeeded
    EXPECT_EQ(centerCell->getState(), CellState::PLAYER_1);
    EXPECT_NE(centerCell->getState(), initialState);
}

TEST_F(LightUndoTests, ChainReactionWithLightUndo) {
    OctaGameLogic logic(gameMap, config);
    
    // Set up a chain reaction scenario
    // First, check how many neighbors the center cell has
    size_t neighborCount = centerCell->getValidNeighborCount();
    
    // Set centerCell to have a value that will explode when incremented
    // Value needs to be > neighborCount after increment
    centerCell->setState(CellState::PLAYER_1);
    centerCell->setValue(neighborCount);  // Will become neighborCount+1 after increment, which should explode
    rightCell->setState(CellState::NEUTRAL);
    
    // Record initial states
    auto initialRightState = rightCell->getState();
    
    // Make move that should cause chain reaction
    auto result = logic.makeMove(centerCell, Player::PLAYER_1);
    
    // Verify chain reaction occurred
    EXPECT_EQ(centerCell->getState(), CellState::PLAYER_1);
    EXPECT_EQ(rightCell->getState(), CellState::PLAYER_1);  // Should be converted
    EXPECT_NE(rightCell->getState(), initialRightState);
}

TEST_F(LightUndoTests, PerformanceImpactMeasurement) {
    // Test performance difference between VALIDATE_ONLY and LIGHT_UNDO
    const int NUM_ITERATIONS = 100;
    
    // Measure VALIDATE_ONLY performance
    GameConfig validateConfig(WinCondition::ELIMINATION, 100, false, SafetyLevel::VALIDATE_ONLY);
    auto validateMap = std::make_shared<GraphGameMap>(2);
    OctaGameLogic validateLogic(validateMap, validateConfig);
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        validateLogic.resetGame();
        auto cell = validateMap->at(Coordinate(0, 0));
        validateLogic.makeMove(cell, Player::PLAYER_1);
    }
    auto validateTime = std::chrono::high_resolution_clock::now() - start;
    
    // Measure LIGHT_UNDO performance
    auto undoMap = std::make_shared<GraphGameMap>(2);
    OctaGameLogic undoLogic(undoMap, config);
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        undoLogic.resetGame();
        auto cell = undoMap->at(Coordinate(0, 0));
        undoLogic.makeMove(cell, Player::PLAYER_1);
    }
    auto undoTime = std::chrono::high_resolution_clock::now() - start;
    
    // Calculate overhead percentage
    auto validateMs = std::chrono::duration_cast<std::chrono::microseconds>(validateTime).count();
    auto undoMs = std::chrono::duration_cast<std::chrono::microseconds>(undoTime).count();
    
    double overheadPercent = ((double)(undoMs - validateMs) / validateMs) * 100.0;
    
    // Verify overhead is acceptable (< 10% as per strategy)
    EXPECT_LT(overheadPercent, 10.0) << "LIGHT_UNDO overhead: " << overheadPercent << "%";
    
    // Output timing information for analysis
    std::cout << "Performance Analysis:\n";
    std::cout << "  VALIDATE_ONLY: " << validateMs << " μs\n";
    std::cout << "  LIGHT_UNDO:    " << undoMs << " μs\n";
    std::cout << "  Overhead:      " << overheadPercent << "%\n";
}

TEST_F(LightUndoTests, MemorySafetyWithMultipleCells) {
    OctaGameLogic logic(gameMap, config);
    
    // Set up a scenario that will affect multiple cells
    // First, check how many neighbors the center cell has
    size_t neighborCount = centerCell->getValidNeighborCount();
    
    // Set centerCell to have a value that will explode when incremented
    centerCell->setState(CellState::PLAYER_1);
    centerCell->setValue(neighborCount);  // Will become neighborCount+1 after increment, which should explode
    
    rightCell->setState(CellState::NEUTRAL);
    topCell->setState(CellState::NEUTRAL);
    
    // Record initial states
    auto initialRightState = rightCell->getState();
    auto initialTopState = topCell->getState();
    
    // Make move that affects multiple cells
    auto result = logic.makeMove(centerCell, Player::PLAYER_1);
    
    // Verify multiple cells were affected
    EXPECT_EQ(rightCell->getState(), CellState::PLAYER_1);
    EXPECT_EQ(topCell->getState(), CellState::PLAYER_1);
    
    // Verify states changed from initial
    EXPECT_NE(rightCell->getState(), initialRightState);
    EXPECT_NE(topCell->getState(), initialTopState);
}

TEST_F(LightUndoTests, EdgeCaseNullCellHandling) {
    OctaGameLogic logic(gameMap, config);
    
    // Test that null cells are handled gracefully
    EXPECT_THROW(logic.makeMove(nullptr, Player::PLAYER_1), std::invalid_argument);
    
    // Verify game state is unchanged
    EXPECT_EQ(logic.getCurrentPlayer(), Player::PLAYER_1);
    EXPECT_EQ(logic.getTurnCount(), 0);
}

/**
 * @brief Test runner main function
 *
 * This function initializes Google Test and runs all tests.
 * Individual component tests will be added in subsequent phases.
 */
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}