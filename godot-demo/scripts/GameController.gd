extends Control

# Game API
var octa_api: OctaCoreAPI
var map_size: int = 2
var cell_size: float = 80.0
var cell_buttons: Dictionary = {}

# UI References
@onready var grid_container: GridContainer = $VBoxContainer/GamePanel/GridContainer
@onready var status_label: Label = $VBoxContainer/InfoPanel/StatusLabel
@onready var player_label: Label = $VBoxContainer/InfoPanel/PlayerLabel
@onready var score_label: Label = $VBoxContainer/InfoPanel/ScoreLabel
@onready var new_game_button: Button = $VBoxContainer/InfoPanel/NewGameButton
@onready var quit_button: Button = $VBoxContainer/InfoPanel/QuitButton

func _ready():
	print("Game Controller starting...")
	
	# Initialize the game API
	octa_api = OctaCoreAPI.new()
	
	# Connect buttons
	new_game_button.pressed.connect(_on_new_game_pressed)
	quit_button.pressed.connect(_on_quit_pressed)
	
	# Start a new game
	_start_new_game()

func _start_new_game():
	print("Starting new game...")
	
	# Clear existing buttons
	for child in grid_container.get_children():
		child.queue_free()
	cell_buttons.clear()
	
	# Create game
	if not octa_api.create_game(map_size):
		print("Failed to create game!")
		return
	
	# Setup grid
	var grid_size = map_size * 2 + 1
	grid_container.columns = grid_size
	
	# Create cell buttons
	for y in range(-map_size, map_size + 1):
		for x in range(-map_size, map_size + 1):
			var button = Button.new()
			button.custom_minimum_size = Vector2(cell_size, cell_size)
			button.text = ""
			
			# Store coordinates
			var coord = Vector2i(x, y)
			button.set_meta("coord", coord)
			cell_buttons[coord] = button
			
			# Connect button signal
			button.pressed.connect(_on_cell_pressed.bind(x, y))
			
			grid_container.add_child(button)
	
	# Initial update
	_update_display()

func _on_cell_pressed(x: int, y: int):
	print("Cell pressed: (", x, ", ", y, ")")
	
	if octa_api.is_game_over():
		print("Game is over!")
		return
	
	if not octa_api.is_valid_move(x, y):
		print("Invalid move!")
		return
	
	# Make the move
	if octa_api.make_move(x, y):
		print("Move successful!")
		_update_display()
		
		# Check if game ended
		if octa_api.is_game_over():
			_show_game_over()
	else:
		print("Move failed!")

func _update_display():
	# Update all cell buttons
	for coord in cell_buttons.keys():
		var button = cell_buttons[coord]
		var cell_info = octa_api.get_cell_info(coord.x, coord.y)
		
		# Update button appearance based on cell state
		_update_cell_button(button, cell_info)
	
	# Update status labels
	var state = octa_api.get_game_state()
	var current_player = octa_api.get_current_player()
	
	player_label.text = "Current Player: " + ("Player 1" if current_player == 1 else "Player 2")
	score_label.text = "Score - Player 1: %d | Player 2: %d" % [state.player1_cells, state.player2_cells]
	status_label.text = "Turn: %d" % state.turn_count
	
	if state.game_over:
		if state.winner == 1:
			status_label.text += " | PLAYER 1 WINS!"
		elif state.winner == 2:
			status_label.text += " | PLAYER 2 WINS!"
		elif state.winner == -1:
			status_label.text += " | DRAW!"

func _update_cell_button(button: Button, cell_info: OctaCoreAPI.CellInfo):
	# Set button text to show cell value
	if cell_info.value > 0:
		button.text = str(cell_info.value)
	else:
		button.text = ""
	
	# Set button color based on player ownership
	match cell_info.state:
		0:  # NEUTRAL
			button.modulate = Color.WHITE
			button.disabled = false
		1:  # PLAYER_1
			button.modulate = Color.LIGHT_BLUE
			button.disabled = false
		2:  # PLAYER_2
			button.modulate = Color.LIGHT_CORAL
			button.disabled = false
		3:  # BLOCKED
			button.modulate = Color.GRAY
			button.disabled = true

func _show_game_over():
	var state = octa_api.get_game_state()
	var message = ""
	
	if state.winner == 1:
		message = "Player 1 Wins!"
	elif state.winner == 2:
		message = "Player 2 Wins!"
	else:
		message = "It's a Draw!"
	
	print("Game Over: ", message)
	
	# Disable all cell buttons
	for button in cell_buttons.values():
		button.disabled = true

func _on_new_game_pressed():
	print("New game requested")
	_start_new_game()

func _on_quit_pressed():
	print("Quit requested")
	get_tree().quit() 