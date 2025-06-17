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

// Include the new core components
#include "octa-core/core/Enums.h"
#include "octa-core/core/Direction.h"
#include "octa-core/core/GameCell.h"
#include "octa-core/map/IGameMap.h"
#include "octa-core/map/GraphGameMap.h"
#include "octa-core/map/ArrayGameMap.h"
#include "octa-core/model/GameConfig.h"
#include "octa-core/logic/IGameLogic.h"
#include "octa-core/logic/OctaGameLogic.h"

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
    EXPECT_FALSE(cell1->isBlocked());    // NEUTRAL
    EXPECT_FALSE(cell2->isBlocked());    // PLAYER_1
    EXPECT_TRUE(cell3->isBlocked());     // BLOCKED
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
    EXPECT_TRUE(cornerCell->hasNeighbor(Direction::SW));   // Should be (1, 1)
    EXPECT_TRUE(cornerCell->hasNeighbor(Direction::W));    // Should be (1, 2)
    EXPECT_TRUE(cornerCell->hasNeighbor(Direction::S));    // Should be (2, 1)
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
        gameMap = std::make_shared<GraphGameMap>(2); // Small 5x5 map for testing
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