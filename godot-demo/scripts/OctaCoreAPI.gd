extends RefCounted
class_name OctaCoreAPI

# Game state structure
class GameState:
	var winner: int = 0
	var turn_count: int = 0
	var player1_cells: int = 0
	var player2_cells: int = 0
	var game_over: bool = false

# Cell info structure with arrow direction
class CellInfo:
	var state: int = 0      # 0 = neutral, 1 = player1, 2 = player2
	var direction: int = 0  # 0-5 for hexagonal (6 directions)
	var x: int = 0
	var y: int = 0

# 6 directions for hexagonal grid (60-degree increments) - PRIMARY MODE
const HEX_DIRECTIONS = [
	Vector2i(1, 0),    # 0: East (→)
	Vector2i(0, -1),   # 1: Northeast (↗)  
	Vector2i(-1, -1),  # 2: Northwest (↖)
	Vector2i(-1, 0),   # 3: West (←)
	Vector2i(0, 1),    # 4: Southwest (↙)
	Vector2i(1, 1)     # 5: Southeast (↘)
]

const HEX_DIRECTION_NAMES = ["E", "NE", "NW", "W", "SW", "SE"]
const HEX_DIRECTION_SYMBOLS = ["→", "↗", "↖", "←", "↙", "↘"]

# 8 directions for octagonal grid (legacy/fallback)
const OCTA_DIRECTIONS = [
	Vector2i(0, -1),   # 0: North (↑)
	Vector2i(1, -1),   # 1: Northeast (↗)
	Vector2i(1, 0),    # 2: East (→)
	Vector2i(1, 1),    # 3: Southeast (↘)
	Vector2i(0, 1),    # 4: South (↓)
	Vector2i(-1, 1),   # 5: Southwest (↙)
	Vector2i(-1, 0),   # 6: West (←)
	Vector2i(-1, -1)   # 7: Northwest (↖)
]

const OCTA_DIRECTION_NAMES = ["N", "NE", "E", "SE", "S", "SW", "W", "NW"]

# C API bindings using Godot's native interface
static var _lib_path = "res://lib/liboctacore.dylib"
static var _handle = null

static func _static_init():
	# Load the native library
	if OS.has_feature("macos"):
		_lib_path = "res://lib/liboctacore.dylib"
	elif OS.has_feature("linux"):
		_lib_path = "res://lib/liboctacore.so"
	elif OS.has_feature("windows"):
		_lib_path = "res://lib/octacore.dll"
	
	print("Loading Octa-Core library from: ", _lib_path)

# Game instance management
var _game_handle = null
var _hexagonal_mode: bool = true  # Default to hexagonal (original format)

func _init():
	print("Initializing Octa-Core Hexagonal Contagion API (Original Format)")

func create_game(map_size: int) -> bool:
	print("Creating hexagonal contagion game with map size: ", map_size)
	
	_game_handle = {
		"map_size": map_size, 
		"cells": {}, 
		"current_player": 1, 
		"turn_count": 0, 
		"game_over": false
	}
	
	# Initialize hexagonal grid
	for x in range(-map_size, map_size + 1):
		for y in range(-map_size, map_size + 1):
			var cell = CellInfo.new()
			cell.x = x
			cell.y = y
			cell.state = 0  # NEUTRAL
			# Random initial arrow direction (6 directions for hex)
			cell.direction = randi() % 6
			_game_handle.cells[Vector2i(x, y)] = cell
	
	# Set initial player positions
	if map_size >= 1:
		# Player 1 starts in top-left area
		var p1_cell = _game_handle.cells[Vector2i(-1, -1)]
		p1_cell.state = 1
		p1_cell.direction = 5  # Southeast arrow initially
		
		# Player 2 starts in bottom-right area  
		var p2_cell = _game_handle.cells[Vector2i(1, 1)]
		p2_cell.state = 2
		p2_cell.direction = 2  # Northwest arrow initially
	
	return true

func destroy_game():
	_game_handle = null
	print("Hexagonal game destroyed")

func get_game_state() -> GameState:
	var state = GameState.new()
	if _game_handle == null:
		return state
		
	state.turn_count = _game_handle.turn_count
	state.game_over = _game_handle.game_over
	
	# Count cells for each player
	for cell_info in _game_handle.cells.values():
		if cell_info.state == 1:
			state.player1_cells += 1
		elif cell_info.state == 2:
			state.player2_cells += 1
	
	# Win condition check
	if state.player1_cells == 0 and state.player2_cells > 0:
		state.winner = 2
		state.game_over = true
	elif state.player2_cells == 0 and state.player1_cells > 0:
		state.winner = 1
		state.game_over = true
	elif state.turn_count > 50:
		state.game_over = true
		if state.player1_cells > state.player2_cells:
			state.winner = 1
		elif state.player2_cells > state.player1_cells:
			state.winner = 2
		else:
			state.winner = -1  # Draw
	
	return state

func get_current_player() -> int:
	if _game_handle == null:
		return 1
	return _game_handle.current_player

func is_game_over() -> bool:
	if _game_handle == null:
		return false
	return _game_handle.game_over

func is_valid_move(x: int, y: int) -> bool:
	if _game_handle == null or _game_handle.game_over:
		return false
	
	var pos = Vector2i(x, y)
	if not _game_handle.cells.has(pos):
		return false
	
	var cell = _game_handle.cells[pos]
	var current_player = _game_handle.current_player
	
	# Valid move: can click on cells you own to rotate their arrows
	# OR can click neutral cells to capture them
	return cell.state == current_player or cell.state == 0

func make_move(x: int, y: int) -> bool:
	if _game_handle == null or _game_handle.game_over:
		return false
	
	if not is_valid_move(x, y):
		return false
	
	var pos = Vector2i(x, y)
	var cell = _game_handle.cells[pos]
	var current_player = _game_handle.current_player
	
	print("=== HEXAGONAL CONTAGION MOVE ===")
	print("Player ", current_player, " selected hex cell (", x, ",", y, ")")
	
	# If cell is neutral, capture it first
	if cell.state == 0:
		cell.state = current_player
		print("Captured neutral hex cell")
	
	# Rotate arrow clockwise (key mechanic!)
	var old_direction = cell.direction
	cell.direction = (cell.direction + 1) % 6  # 6 directions for hexagonal
	print("Rotated hex arrow from ", HEX_DIRECTION_NAMES[old_direction], " to ", HEX_DIRECTION_NAMES[cell.direction])
	
	# Start chain reaction following the arrow direction
	var chain_cells = []
	_execute_hex_contagion_chain(pos, current_player, chain_cells)
	
	print("Hexagonal chain reaction affected ", len(chain_cells), " cells")
	
	# Switch players
	_game_handle.current_player = 3 - current_player  # Toggle between 1 and 2
	_game_handle.turn_count += 1
	
	# Check game state
	var state = get_game_state()
	_game_handle.game_over = state.game_over
	
	return true

func _execute_hex_contagion_chain(start_pos: Vector2i, player: int, chain_cells: Array):
	"""Execute recursive hexagonal contagion following arrow paths"""
	var current_pos = start_pos
	var visited_this_turn = {}
	
	while true:
		# Prevent infinite loops
		if visited_this_turn.has(current_pos):
			print("Loop detected, stopping hex chain")
			break
		
		visited_this_turn[current_pos] = true
		chain_cells.append(current_pos)
		
		var current_cell = _game_handle.cells.get(current_pos)
		if not current_cell:
			print("Invalid hex cell, stopping chain")
			break
		
		# Get the direction this cell points to (6 directions)
		var direction_vector = HEX_DIRECTIONS[current_cell.direction]
		var target_pos = current_pos + direction_vector
		
		print("Hex cell (", current_pos.x, ",", current_pos.y, ") points ", HEX_DIRECTION_NAMES[current_cell.direction], " to (", target_pos.x, ",", target_pos.y, ")")
		
		# Check if target exists
		if not _game_handle.cells.has(target_pos):
			print("Hex arrow points off-grid, stopping chain")
			break
		
		var target_cell = _game_handle.cells[target_pos]
		
		# Check stopping conditions
		if target_cell.state == player:
			print("Hex arrow points to own cell, stopping chain")
			break
		
		# Capture the target cell
		print("Capturing hex cell (", target_pos.x, ",", target_pos.y, ") for player ", player)
		target_cell.state = player
		
		# Rotate target cell's arrow (6-direction automatic progression)
		var old_dir = target_cell.direction
		target_cell.direction = (target_cell.direction + 1) % 6
		print("Auto-rotated hex arrow from ", HEX_DIRECTION_NAMES[old_dir], " to ", HEX_DIRECTION_NAMES[target_cell.direction])
		
		# Continue chain from the newly captured cell
		current_pos = target_pos

func get_cell_info(x: int, y: int) -> CellInfo:
	var cell_info = CellInfo.new()
	cell_info.x = x
	cell_info.y = y
	
	if _game_handle == null:
		return cell_info
	
	var pos = Vector2i(x, y)
	if _game_handle.cells.has(pos):
		var cell = _game_handle.cells[pos]
		cell_info.state = cell.state
		cell_info.direction = cell.direction
	
	return cell_info

func get_direction_name(direction: int) -> String:
	if direction >= 0 and direction < len(HEX_DIRECTION_NAMES):
		return HEX_DIRECTION_NAMES[direction]
	return "?"

func get_direction_symbol(direction: int) -> String:
	if direction >= 0 and direction < len(HEX_DIRECTION_SYMBOLS):
		return HEX_DIRECTION_SYMBOLS[direction]
	return "?"

func get_map_size() -> int:
	if _game_handle == null:
		return 0
	return _game_handle.map_size

func get_all_cells() -> Array[CellInfo]:
	var cells: Array[CellInfo] = []
	if _game_handle == null:
		return cells
	
	for cell in _game_handle.cells.values():
		cells.append(cell)
	
	return cells

# Utility functions
func is_hexagonal_mode() -> bool:
	return _hexagonal_mode

func get_max_directions() -> int:
	return 6 if _hexagonal_mode else 8 