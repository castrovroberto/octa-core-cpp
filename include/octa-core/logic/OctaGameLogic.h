/**
 * @file OctaGameLogic.h
 * @brief Concrete implementation of IGameLogic for Octa-Core game mechanics
 *
 * This file contains the OctaGameLogic class, which implements the full
 * Octa-Core game rules including chain reactions, win conditions, and
 * turn management. This is the primary game logic implementation.
 *
 * Created in Phase P1.3 according to SRD v3.2 specifications.
 */

#ifndef OCTA_CORE_OCTAGAMELOGIC_H
#define OCTA_CORE_OCTAGAMELOGIC_H

#include "IGameLogic.h"

#include <memory>
#include <queue>
#include <vector>

#include "../map/IGameMap.h"

/**
 * @class OctaGameLogic
 * @brief Concrete implementation of Octa-Core game logic
 *
 * This class implements the complete Octa-Core game mechanics:
 * - Chain reaction algorithm with octagonal propagation
 * - Win condition checking (elimination and turn limit)
 * - Move validation and execution
 * - Game state management
 * - Configurable safety levels
 *
 * The implementation uses a breadth-first search approach for chain
 * reactions and maintains game state through member variables.
 *
 * Key features:
 * - Exception-safe move execution
 * - Configurable chain reaction behavior
 * - Comprehensive win condition support
 * - Efficient neighbor traversal using Direction enum
 */
class OctaGameLogic : public IGameLogic {
  private:
    /// The game map containing all cells
    std::shared_ptr<IGameMap> gameMap_;

    /// Current game configuration
    GameConfig config_;

    /// Current active player
    Player currentPlayer_;

    /// Number of completed turns
    int turnCount_;

    /// Current game result (if game is over)
    mutable std::optional<GameResult> gameResult_;

    /// Flag indicating if game has ended
    mutable bool gameOver_;

  public:
    /**
     * @brief Constructor with game map and configuration
     * @param gameMap Shared pointer to the game map
     * @param config Game configuration settings
     * @throws std::invalid_argument If gameMap is null or config is invalid
     *
     * Initializes the game logic with:
     * - Player 1 as starting player
     * - Turn count at 0
     * - Game not over
     * - No game result yet
     */
    explicit OctaGameLogic(std::shared_ptr<IGameMap> gameMap,
                           const GameConfig& config = GameConfig());

    /**
     * @brief Destructor
     *
     * Default destructor is sufficient as we use smart pointers
     * for automatic memory management.
     */
    ~OctaGameLogic() override = default;

    // IGameLogic interface implementation
    bool isValidMove(std::shared_ptr<const GameCell> cell, Player player) const override;
    GameResult makeMove(std::shared_ptr<GameCell> cell, Player player) override;
    bool isGameOver() const override;
    Player getCurrentPlayer() const override;
    void switchPlayer() override;
    int getTurnCount() const override;
    const GameConfig& getConfig() const override;
    std::optional<GameResult> getGameResult() const override;
    void resetGame(const GameConfig* newConfig = nullptr) override;

  private:
    /**
     * @brief Executes the chain reaction algorithm starting from a cell
     * @param startCell The cell where the chain reaction begins
     * @param player The player making the move
     * @return Vector of cells that were affected by the chain reaction
     *
     * This method implements the core chain reaction mechanics:
     * 1. Increments the starting cell's value
     * 2. If cell becomes unstable (value > neighbor count), it explodes
     * 3. Explosion spreads to all neighbors, potentially causing more explosions
     * 4. Process continues until all cells are stable
     *
     * The algorithm uses breadth-first search to ensure fair propagation
     * and respects the stopOnEnemy configuration setting.
     */
    std::vector<std::shared_ptr<GameCell>> executeChainReaction(std::shared_ptr<GameCell> startCell,
                                                                Player player);

    /**
     * @brief Checks if a cell should explode based on its value and neighbors
     * @param cell The cell to check for instability
     * @return True if the cell should explode, false otherwise
     *
     * A cell explodes when its value exceeds the number of valid neighbors.
     * This is the core mechanic that drives chain reactions.
     */
    bool shouldExplode(std::shared_ptr<const GameCell> cell) const;

    /**
     * @brief Explodes a cell, distributing its value to neighbors
     * @param cell The cell to explode
     * @param player The player who owns the explosion
     * @param affectedCells Vector to track cells affected by the explosion
     *
     * This method:
     * 1. Resets the exploding cell to value 0
     * 2. Distributes energy to all valid neighbors
     * 3. Converts enemy neighbors to the current player's ownership
     * 4. Tracks all affected cells for further chain reaction processing
     *
     * Respects the stopOnEnemy configuration setting.
     */
    void explodeCell(std::shared_ptr<GameCell> cell, Player player,
                     std::vector<std::shared_ptr<GameCell>>& affectedCells);

    /**
     * @brief Checks all win conditions and updates game state
     * @return GameResult if game has ended, nullopt otherwise
     *
     * This method checks:
     * - Elimination condition: One player has no cells
     * - Turn limit condition: Maximum turns reached
     * - Updates gameOver_ flag and gameResult_ if game ends
     *
     * Called after each move to determine if the game should end.
     */
    std::optional<GameResult> checkWinConditions() const;

    /**
     * @brief Counts cells owned by a specific player
     * @param player The player whose cells to count
     * @return Number of cells owned by the player
     *
     * Used for win condition checking and game result statistics.
     * Iterates through all cells in the game map.
     */
    int countPlayerCells(Player player) const;

    /**
     * @brief Validates that the game configuration is correct
     * @param config The configuration to validate
     * @throws std::invalid_argument If configuration is invalid
     *
     * Checks configuration validity using GameConfig::isValid()
     * and throws descriptive error messages for invalid settings.
     */
    void validateConfig(const GameConfig& config) const;

    /**
     * @brief Initializes game state to starting conditions
     *
     * Sets up initial game state:
     * - Current player to PLAYER_1
     * - Turn count to 0
     * - Game not over
     * - No game result
     *
     * Called by constructor and resetGame().
     */
    void initializeGameState();
};

#endif  // OCTA_CORE_OCTAGAMELOGIC_H