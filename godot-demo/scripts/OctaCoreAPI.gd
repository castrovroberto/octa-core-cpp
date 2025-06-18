extends RefCounted
class_name OctaCoreAPI

# Game state structure
class GameState:
	var winner: int = 0
	var turn_count: int = 0
	var player1_cells: int = 0
	var player2_cells: int = 0
	var game_over: bool = false

# Cell info structure  
class CellInfo:
	var state: int = 0
	var value: int = 0
	var x: int = 0
	var y: int = 0

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

func _init():
	# For this demo, we'll simulate the C API calls with GDScript
	# In a full implementation, we would use Godot's FFI or GDExtension
	print("Initializing Octa-Core API wrapper")

func create_game(map_size: int) -> bool:
	print("Creating game with map size: ", map_size)
	# Simulate successful game creation
	_game_handle = {"map_size": map_size, "cells": {}, "current_player": 1, "turn_count": 0, "game_over": false}
	
	# Initialize a simple 3x3 grid for demo
	for x in range(-map_size, map_size + 1):
		for y in range(-map_size, map_size + 1):
			var cell = CellInfo.new()
			cell.x = x
			cell.y = y
			cell.state = 0  # NEUTRAL
			cell.value = 0
			_game_handle.cells[Vector2i(x, y)] = cell
	
	# Set initial player positions for demo
	if map_size >= 1:
		_game_handle.cells[Vector2i(-1, -1)].state = 1  # PLAYER_1
		_game_handle.cells[Vector2i(-1, -1)].value = 1
		_game_handle.cells[Vector2i(1, 1)].state = 2   # PLAYER_2
		_game_handle.cells[Vector2i(1, 1)].value = 1
	
	return true

func destroy_game():
	_game_handle = null
	print("Game destroyed")

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
	
	# Simple win condition check
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
	if _game_handle == null:
		return false
	
	var pos = Vector2i(x, y)
	if not _game_handle.cells.has(pos):
		return false
	
	var cell = _game_handle.cells[pos]
	var current_player = _game_handle.current_player
	
	# Valid move if cell is owned by current player or neutral
	return cell.state == current_player or cell.state == 0

func make_move(x: int, y: int) -> bool:
	if _game_handle == null or _game_handle.game_over:
		return false
	
	if not is_valid_move(x, y):
		return false
	
	var pos = Vector2i(x, y)
	var cell = _game_handle.cells[pos]
	var current_player = _game_handle.current_player
	
	# Simple move: increment cell value and set ownership
	cell.state = current_player
	cell.value += 1
	
	# Simple chain reaction: if value > 3, spread to neighbors
	if cell.value > 3:
		_trigger_explosion(x, y, current_player)
	
	# Switch players
	_game_handle.current_player = 3 - current_player  # Toggle between 1 and 2
	_game_handle.turn_count += 1
	
	# Check game state
	var state = get_game_state()
	_game_handle.game_over = state.game_over
	
	print("Move made at (", x, ",", y, ") by player ", current_player)
	return true

func _trigger_explosion(x: int, y: int, player: int):
	var pos = Vector2i(x, y)
	var cell = _game_handle.cells[pos]
	
	# Reset exploding cell
	cell.value = 0
	cell.state = 0  # NEUTRAL
	
	# Spread to neighbors
	var directions = [Vector2i(-1, -1), Vector2i(-1, 0), Vector2i(-1, 1),
					  Vector2i(0, -1),                    Vector2i(0, 1),
					  Vector2i(1, -1),  Vector2i(1, 0),   Vector2i(1, 1)]
	
	for dir in directions:
		var neighbor_pos = pos + dir
		if _game_handle.cells.has(neighbor_pos):
			var neighbor = _game_handle.cells[neighbor_pos]
			neighbor.state = player
			neighbor.value += 1
			# Simple chain reaction
			if neighbor.value > 3:
				_trigger_explosion(neighbor_pos.x, neighbor_pos.y, player)

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
		cell_info.value = cell.value
	
	return cell_info

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