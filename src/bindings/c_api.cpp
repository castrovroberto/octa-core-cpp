#include "octa-core/bindings/c_api.h"
#include "octa-core/logic/OctaGameLogic.h"
#include "octa-core/map/GraphGameMap.h"
#include "octa-core/model/GameConfig.h"
#include <memory>
#include <exception>

// Internal structure to hold C++ objects
struct GameWrapper {
    std::shared_ptr<GraphGameMap> map;
    std::shared_ptr<OctaGameLogic> logic;
    int mapSize;
};

// Helper function to convert C++ enums to C integers
int playerToInt(Player player) {
    switch (player) {
        case Player::PLAYER_1: return 1;
        case Player::PLAYER_2: return 2;
        default: return 0;
    }
}

int cellStateToInt(CellState state) {
    switch (state) {
        case CellState::NEUTRAL: return 0;
        case CellState::PLAYER_1: return 1;
        case CellState::PLAYER_2: return 2;
        case CellState::BLOCKED: return 3;
        default: return 0;
    }
}

CellState intToCellState(int state) {
    switch (state) {
        case 1: return CellState::PLAYER_1;
        case 2: return CellState::PLAYER_2;
        case 3: return CellState::BLOCKED;
        default: return CellState::NEUTRAL;
    }
}

Player intToPlayer(int player) {
    switch (player) {
        case 1: return Player::PLAYER_1;
        case 2: return Player::PLAYER_2;
        default: return Player::PLAYER_1; // Default fallback
    }
}

// C API Implementation
extern "C" {

octa_game_handle_t octa_create_game(int map_size) {
    try {
        auto wrapper = new GameWrapper();
        wrapper->mapSize = map_size;
        wrapper->map = std::make_shared<GraphGameMap>(map_size);
        
        GameConfig config;
        config.safetyLevel = SafetyLevel::VALIDATE_ONLY; // Use basic safety for demo
        wrapper->logic = std::make_shared<OctaGameLogic>(wrapper->map, config);
        
        return static_cast<octa_game_handle_t>(wrapper);
    } catch (...) {
        return nullptr;
    }
}

void octa_destroy_game(octa_game_handle_t game) {
    if (game) {
        auto wrapper = static_cast<GameWrapper*>(game);
        delete wrapper;
    }
}

void octa_reset_game(octa_game_handle_t game) {
    if (!game) return;
    
    try {
        auto wrapper = static_cast<GameWrapper*>(game);
        wrapper->logic->resetGame();
    } catch (...) {
        // Ignore errors for demo
    }
}

octa_game_state_t octa_get_game_state(octa_game_handle_t game) {
    octa_game_state_t state = {0};
    
    if (!game) return state;
    
    try {
        auto wrapper = static_cast<GameWrapper*>(game);
        auto result = wrapper->logic->getGameResult();
        
        state.turn_count = wrapper->logic->getTurnCount();
        state.game_over = wrapper->logic->isGameOver() ? 1 : 0;
        
        if (result.has_value()) {
            if (result->winner.has_value()) {
                state.winner = playerToInt(result->winner.value());
            } else {
                state.winner = -1; // Draw
            }
        }
        
        // Count cells for each player
        int player1_count = 0, player2_count = 0;
        for (int x = -wrapper->mapSize; x <= wrapper->mapSize; x++) {
            for (int y = -wrapper->mapSize; y <= wrapper->mapSize; y++) {
                auto cell = wrapper->map->at(Coordinate(x, y));
                if (cell) {
                    if (cell->getState() == CellState::PLAYER_1) player1_count++;
                    else if (cell->getState() == CellState::PLAYER_2) player2_count++;
                }
            }
        }
        state.player1_cells = player1_count;
        state.player2_cells = player2_count;
        
    } catch (...) {
        // Return default state on error
    }
    
    return state;
}

int octa_get_current_player(octa_game_handle_t game) {
    if (!game) return 1;
    
    try {
        auto wrapper = static_cast<GameWrapper*>(game);
        return playerToInt(wrapper->logic->getCurrentPlayer());
    } catch (...) {
        return 1;
    }
}

int octa_is_game_over(octa_game_handle_t game) {
    if (!game) return 0;
    
    try {
        auto wrapper = static_cast<GameWrapper*>(game);
        return wrapper->logic->isGameOver() ? 1 : 0;
    } catch (...) {
        return 0;
    }
}

int octa_is_valid_move(octa_game_handle_t game, int x, int y) {
    if (!game) return 0;
    
    try {
        auto wrapper = static_cast<GameWrapper*>(game);
        auto cell = wrapper->map->at(Coordinate(x, y));
        if (!cell) return 0;
        
        return wrapper->logic->isValidMove(cell, wrapper->logic->getCurrentPlayer()) ? 1 : 0;
    } catch (...) {
        return 0;
    }
}

int octa_make_move(octa_game_handle_t game, int x, int y) {
    if (!game) return 0;
    
    try {
        auto wrapper = static_cast<GameWrapper*>(game);
        auto cell = wrapper->map->at(Coordinate(x, y));
        if (!cell) return 0;
        
        auto result = wrapper->logic->makeMove(cell, wrapper->logic->getCurrentPlayer());
        return 1; // Success
    } catch (...) {
        return 0; // Failure
    }
}

octa_cell_info_t octa_get_cell_info(octa_game_handle_t game, int x, int y) {
    octa_cell_info_t info = {0};
    info.x = x;
    info.y = y;
    
    if (!game) return info;
    
    try {
        auto wrapper = static_cast<GameWrapper*>(game);
        auto cell = wrapper->map->at(Coordinate(x, y));
        if (cell) {
            info.state = cellStateToInt(cell->getState());
            info.value = cell->getValue();
        }
    } catch (...) {
        // Return default info on error
    }
    
    return info;
}

int octa_get_map_size(octa_game_handle_t game) {
    if (!game) return 0;
    
    try {
        auto wrapper = static_cast<GameWrapper*>(game);
        return wrapper->mapSize;
    } catch (...) {
        return 0;
    }
}

int octa_get_total_cells(octa_game_handle_t game) {
    if (!game) return 0;
    
    try {
        auto wrapper = static_cast<GameWrapper*>(game);
        return static_cast<int>(wrapper->map->size());
    } catch (...) {
        return 0;
    }
}

int octa_get_all_cells(octa_game_handle_t game, octa_cell_info_t* cells, int max_cells) {
    if (!game || !cells || max_cells <= 0) return 0;
    
    try {
        auto wrapper = static_cast<GameWrapper*>(game);
        int count = 0;
        
        for (int x = -wrapper->mapSize; x <= wrapper->mapSize && count < max_cells; x++) {
            for (int y = -wrapper->mapSize; y <= wrapper->mapSize && count < max_cells; y++) {
                auto cell = wrapper->map->at(Coordinate(x, y));
                if (cell) {
                    cells[count].x = x;
                    cells[count].y = y;
                    cells[count].state = cellStateToInt(cell->getState());
                    cells[count].value = cell->getValue();
                    count++;
                }
            }
        }
        
        return count;
    } catch (...) {
        return 0;
    }
}

} // extern "C" 