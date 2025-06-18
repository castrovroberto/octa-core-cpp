#ifndef OCTA_CORE_GAMEENGINE_H
#define OCTA_CORE_GAMEENGINE_H

#include <iostream>
#include <memory>

#include "../map/IGameMap.h"

// Forward declarations to avoid circular dependencies
// class GameLogic;
// class CLIView;

/**
 * @class GameEngine
 * @brief The main game engine that orchestrates the game components.
 *
 * The GameEngine uses the IGameMap interface, making it independent of
 * the underlying map implementation (graph-based, array-based, etc.).
 */
class GameEngine {
  public:
    /**
     * @brief Constructor to initialize the engine with its components.
     * @param map The game map (can be any implementation of IGameMap).
     */
    explicit GameEngine(std::shared_ptr<IGameMap> map);

    // Future constructor for when other components are added:
    // GameEngine(std::shared_ptr<IGameMap> map, std::shared_ptr<GameLogic> logic,
    // std::shared_ptr<CLIView> view);

    /**
     * @brief Main game loop.
     */
    void run();

    /**
     * @brief Gets information about the current map.
     */
    void displayMapInfo() const;

  private:
    // Store a pointer to the interface, not the concrete class!
    std::shared_ptr<IGameMap> gameMap_;

    // Future components:
    // std::shared_ptr<GameLogic> gameLogic_;
    // std::shared_ptr<CLIView> view_;
};

#endif  // OCTA_CORE_GAMEENGINE_H
