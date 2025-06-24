extends Control

# Game API
var octa_api: OctaCoreAPI
var map_size: int = 3  # Increased to 3 for better honeycomb demonstration
var cell_size: float = 70.0  # Reduced slightly for better fit
var cell_buttons: Dictionary = {}

# Custom hexagonal button class for honeycomb layout
class HexagonalButton extends Button:
	var arrow_direction: int = 0
	var cell_state: int = 0
	
	func _ready():
		custom_minimum_size = Vector2(80, 80)
		flat = true
		
	func _draw():
		# Draw hexagonal shape (6 sides) for true honeycomb tessellation
		var center = size / 2
		var radius = min(size.x, size.y) * 0.48  # Increased from 0.45 for better coverage
		
		# Create hexagon points (6 sides, 60 degrees between points)
		# Start with flat-top orientation for authentic honeycomb look
		var points = PackedVector2Array()
		for i in range(6):
			var angle = TAU * i / 6 - PI / 6  # Offset by -30° for flat-top hexagon
			var x = center.x + radius * cos(angle)
			var y = center.y + radius * sin(angle)
			points.append(Vector2(x, y))
		
		# Fill hexagon based on cell state
		var fill_color: Color
		match cell_state:
			0:  # NEUTRAL
				fill_color = Color.LIGHT_GRAY
			1:  # PLAYER 1 (Red)  
				fill_color = Color.LIGHT_CORAL
			2:  # PLAYER 2 (Blue)
				fill_color = Color.LIGHT_BLUE
			_:
				fill_color = Color.GRAY
		
		# Draw filled hexagon
		draw_colored_polygon(points, fill_color)
		
		# Draw hexagon border for clear tessellation
		for i in range(6):
			var p1 = points[i]
			var p2 = points[(i + 1) % 6]
			draw_line(p1, p2, Color.BLACK, 1.5)  # Slightly thinner lines for cleaner look
		
		# Draw arrow in center
		_draw_hex_arrow(center, radius * 0.65)  # Slightly larger arrows
	
	func _draw_hex_arrow(center: Vector2, length: float):
		# Arrow directions for hexagon: E, NE, NW, W, SW, SE
		var hex_angles = [0, -PI/3, -2*PI/3, PI, 2*PI/3, PI/3]  # 60-degree increments
		var angle = hex_angles[arrow_direction % 6]
		
		# Calculate arrow end point
		var end_point = center + Vector2(cos(angle), sin(angle)) * length
		
		# Draw arrow shaft
		draw_line(center, end_point, Color.BLACK, 3.0)
		
		# Draw arrow head
		var head_angle1 = angle + 2 * PI / 3
		var head_angle2 = angle - 2 * PI / 3
		var head_length = length * 0.3
		
		var head_point1 = end_point + Vector2(cos(head_angle1), sin(head_angle1)) * head_length
		var head_point2 = end_point + Vector2(cos(head_angle2), sin(head_angle2)) * head_length
		
		draw_line(end_point, head_point1, Color.BLACK, 3.0)
		draw_line(end_point, head_point2, Color.BLACK, 3.0)
	
	func update_hex_cell(state: int, direction: int):
		cell_state = state
		arrow_direction = direction % 6  # Ensure 6-direction
		queue_redraw()

# UI References (will be updated for new layout)
@onready var cell_layer: Control = $VBoxContainer/HBoxContainer/GamePanel/CellLayer
@onready var status_label: Label = $VBoxContainer/HBoxContainer/InfoPanel/StatusLabel
@onready var player_label: Label = $VBoxContainer/HBoxContainer/InfoPanel/PlayerLabel
@onready var score_label: Label = $VBoxContainer/HBoxContainer/InfoPanel/ScoreLabel
@onready var new_game_button: Button = $VBoxContainer/HBoxContainer/InfoPanel/NewGameButton
@onready var quit_button: Button = $VBoxContainer/HBoxContainer/InfoPanel/QuitButton

func _ready():
	print("Hexagonal Honeycomb Contagion Game Controller starting...")
	
	# Initialize the game API
	octa_api = OctaCoreAPI.new()
	
	# Connect buttons
	new_game_button.pressed.connect(_on_new_game_pressed)
	quit_button.pressed.connect(_on_quit_pressed)
	
	# Start a new game
	_start_new_game()

func _start_new_game():
	print("Starting new hexagonal honeycomb contagion game...")
	
	# Clear existing buttons
	for child in cell_layer.get_children():
		child.queue_free()
	cell_buttons.clear()
	
	# Create game
	if not octa_api.create_game(map_size):
		print("Failed to create hexagonal game!")
		return
	
	# Create hexagonal buttons with honeycomb layout
	_create_honeycomb_layout()
	
	# Initial update
	_update_display()

func _create_honeycomb_layout():
	print("Creating authentic honeycomb tessellation...")
	
	# True honeycomb tessellation spacing calculations
	var hex_width = cell_size * 0.866   # Width of hexagon (√3/2 * size)
	var hex_height = cell_size          # Height of hexagon
	
	# Honeycomb spacing (hexagons touch each other)
	var horizontal_spacing = hex_width * 0.75  # 3/4 of width for touching hexagons
	var vertical_spacing = hex_height * 0.866  # √3/2 of height for vertical spacing
	var row_offset = hex_width * 0.375         # Half of horizontal spacing for stagger
	
	# Center the entire honeycomb grid
	var center_offset = Vector2(cell_layer.size.x / 2, cell_layer.size.y / 2)
	
	for y in range(-map_size, map_size + 1):
		for x in range(-map_size, map_size + 1):
			var button = HexagonalButton.new()
			
			# Store coordinates
			var coord = Vector2i(x, y)
			button.set_meta("coord", coord)
			cell_buttons[coord] = button
			
			# Connect button signal
			button.pressed.connect(_on_hex_cell_pressed.bind(x, y))
			
			cell_layer.add_child(button)
			
			# True honeycomb positioning (tight tessellation)
			var col = x + map_size
			var row = y + map_size
			
			# Calculate position for honeycomb tessellation
			var px = col * horizontal_spacing
			var py = row * vertical_spacing
			
			# Stagger every other row for authentic honeycomb pattern
			if row % 2 == 1:
				px += row_offset
			
			# Center the entire honeycomb grid and adjust for hexagon center
			var final_pos = Vector2(px, py) - Vector2(horizontal_spacing * map_size, vertical_spacing * map_size) + center_offset
			
			button.position = final_pos
			button.size = Vector2(cell_size, cell_size)

func _on_hex_cell_pressed(x: int, y: int):
	print("=== HEXAGONAL HONEYCOMB CELL CLICKED: (", x, ", ", y, ") ===")
	
	if octa_api.is_game_over():
		print("Hexagonal game is over!")
		return
	
	if not octa_api.is_valid_move(x, y):
		print("Invalid hexagonal move!")
		return
	
	# Show pre-move state
	var cell_info = octa_api.get_cell_info(x, y)
	print("Before move - State: ", cell_info.state, ", Direction: ", octa_api.get_direction_name(cell_info.direction))
	
	# Make the move (this will trigger hexagonal contagion)
	if octa_api.make_move(x, y):
		print("Hexagonal honeycomb contagion move successful!")
		_update_display()
		
		# Check if game ended
		if octa_api.is_game_over():
			_show_game_over()
	else:
		print("Hexagonal move failed!")

func _update_display():
	# Update all hexagonal cell buttons
	for coord in cell_buttons.keys():
		var button = cell_buttons[coord] as HexagonalButton
		var cell_info = octa_api.get_cell_info(coord.x, coord.y)
		
		# Update hexagonal button with cell state and arrow direction
		button.update_hex_cell(cell_info.state, cell_info.direction)
		
		# Update tooltip
		var arrow_symbol = octa_api.get_direction_symbol(cell_info.direction)
		var direction_name = octa_api.get_direction_name(cell_info.direction)
		
		match cell_info.state:
			0:  # NEUTRAL
				button.tooltip_text = "Neutral Hexagon\nArrow: %s (%s)\nClick to capture" % [arrow_symbol, direction_name]
			1:  # PLAYER_1 (Red)
				button.tooltip_text = "Red Hexagon\nArrow: %s (%s)\nClick to rotate arrow" % [arrow_symbol, direction_name]
			2:  # PLAYER_2 (Blue)
				button.tooltip_text = "Blue Hexagon\nArrow: %s (%s)\nClick to rotate arrow" % [arrow_symbol, direction_name]
	
	# Update status labels
	var state = octa_api.get_game_state()
	var current_player = octa_api.get_current_player()
	
	player_label.text = "Current Player: " + ("Player 1 (Red)" if current_player == 1 else "Player 2 (Blue)")
	score_label.text = "Hexagons - Red: %d | Blue: %d" % [state.player1_cells, state.player2_cells]
	status_label.text = "Turn: %d | Honeycomb Contagion Game" % state.turn_count
	
	if state.game_over:
		if state.winner == 1:
			status_label.text += " | RED WINS!"
		elif state.winner == 2:
			status_label.text += " | BLUE WINS!"
		elif state.winner == -1:
			status_label.text += " | DRAW!"

func _show_game_over():
	var state = octa_api.get_game_state()
	var message = ""
	
	if state.winner == 1:
		message = "Red Player Wins!"
	elif state.winner == 2:
		message = "Blue Player Wins!"
	else:
		message = "It's a Draw!"
	
	print("Hexagonal Game Over: ", message)
	
	# Disable all hexagonal cell buttons
	for button in cell_buttons.values():
		button.disabled = true

func _on_new_game_pressed():
	print("New hexagonal honeycomb contagion game requested")
	_start_new_game()

func _on_quit_pressed():
	print("Quit requested")
	get_tree().quit() 
