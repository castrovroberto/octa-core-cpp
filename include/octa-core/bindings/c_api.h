#ifndef OCTA_CORE_C_API_H
#define OCTA_CORE_C_API_H

#ifdef __cplusplus
extern "C" {
#endif

// Basic types for C API
typedef struct {
    int x;
    int y;
} octa_coordinate_t;

typedef struct {
    int winner;  // 0 = no winner, 1 = player1, 2 = player2, -1 = draw
    int turn_count;
    int player1_cells;
    int player2_cells;
    int game_over;
} octa_game_state_t;

typedef struct {
    int state;      // 0 = neutral, 1 = player1, 2 = player2, 3 = blocked
    int value;      // Cell value (number of dots)
    int x;          // Cell coordinate x
    int y;          // Cell coordinate y
} octa_cell_info_t;

// Opaque handles for C++ objects
typedef void* octa_game_handle_t;

// Game management functions
octa_game_handle_t octa_create_game(int map_size);
void octa_destroy_game(octa_game_handle_t game);
void octa_reset_game(octa_game_handle_t game);

// Game state queries
octa_game_state_t octa_get_game_state(octa_game_handle_t game);
int octa_get_current_player(octa_game_handle_t game);
int octa_is_game_over(octa_game_handle_t game);

// Cell operations
int octa_is_valid_move(octa_game_handle_t game, int x, int y);
int octa_make_move(octa_game_handle_t game, int x, int y);
octa_cell_info_t octa_get_cell_info(octa_game_handle_t game, int x, int y);

// Map information
int octa_get_map_size(octa_game_handle_t game);
int octa_get_total_cells(octa_game_handle_t game);

// Utility functions for Godot integration
int octa_get_all_cells(octa_game_handle_t game, octa_cell_info_t* cells, int max_cells);

#ifdef __cplusplus
}
#endif

#endif // OCTA_CORE_C_API_H 