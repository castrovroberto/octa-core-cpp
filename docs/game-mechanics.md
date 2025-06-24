<!---
  Comprehensive Game Mechanics Documentation
  Covers core mechanics: board layout, turns, contagion, win conditions.
-->
# Game Mechanics Overview

## Table of Contents
- Introduction
- Board & Cell Layout
- Directions & Adjacency
- Turn-Based Mechanics
- Contagion & Chain Reaction
- Stopping Conditions
- Win Conditions
- Game Modes
- Octagonal Version Adaptation
- Glossary

## Introduction
This document synthesizes the core mechanics of the directional contagion game ("Hex Contagion" / "Viral Arrows"). Players compete on a cell grid, rotating arrows to spread their virus and capture territory through recursive chain reactions.

## Board & Cell Layout
- **Grid Types**: Originally a hexagonal grid (rings of hexes). Example sizes:
  - 5 rings (61 hexes) for quick games
  - 7 rings (127 hexes) standard mode
  - 9 rings (271 hexes) extended play
- **Cell Structure**:
  - Each cell stores:
    - **State**: neutral, Player 1 (Red), Player 2 (Blue)
    - **Arrow Direction**: one of the discrete orientations (6 for hex, 4 for square, 8 for octagon)
    - **Coordinates**: axial (q, r) for hex; grid indexes for other shapes

## Directions & Adjacency
- **Hexagonal Grid**: 6 neighbors (↖, ↗, →, ↘, ↙, ←)
- **Square / Octagonal Grid**: 8 neighbors (↑, ↗, →, ↘, ↓, ↙, ←, ↖)
- **Coordinate Systems**:
  - **Axial (hex)**: Pair (q, r) with neighbor offsets
  - **Matrix (square/octagon)**: Row/column with 8-directional offsets

## Turn-Based Mechanics
1. **Player Turn**: Players alternate.
2. **Move Selection**:
   - **Hex Contagion**: select a **controlled** cell → rotate its arrow clockwise by one increment.
   - **Bacteria-Style**: select a **neutral** cell → capture and automatically rotate by one increment.
3. **Arrow Update**: On capture or rotation, the cell’s arrow orientation changes to point at a new adjacent cell.
4. **Chain Initiation**: The moved or newly captured cell becomes the start of a propagation chain.

## Contagion & Chain Reaction
Players capture adjacent cells by following arrow directions:
1. **Capture**: Change the target cell’s state to the active player.
2. **Rotate** (if applicable): Advance the arrow by one increment.
3. **Propagate**: From the most recently captured cell, identify the next target in its arrow direction.
4. **Repeat**: Continue capturing and rotating (if automatic) until no further captures are possible.

## Stopping Conditions
A chain stops when the arrow points to:
- An **edge** or off-grid location
- A cell **already owned** by the current player (no new capture)
- A cell **captured earlier this turn** (loop prevention)
- Optionally, an **enemy cell** if rules disallow stealing, or treated as stop on first contact

## Win Conditions
- **Primary**: One player **eliminates** all opponent cells.
- **Alternative**: After a fixed turn limit (e.g., 50 turns), the player controlling **>50%** of cells wins.
- **Board Exhaustion**: When no neutral cells remain, the majority owner wins; ties possible.

## Game Modes
- **Quick Play**: Small grid, fast victory condition (e.g., first to 30 cells).
- **Classic**: Standard grid with full recursive contagion.
- **Challenge**: Large grid, deep strategy, turn limits for scoring.
- **AI / Multiplayer**: Practice vs. computer or online matches.

## Octagonal Version Adaptation
To adapt mechanics to an octagonal (8-neighbor) layout:
1. **Grid Generation**: Create a square matrix with 8-direction connectivity.
2. **Arrow Directions**: 8 possible arrows (45° increments).
3. **Adjacency Logic**: Use 8 offset vectors for neighbor lookup.
4. **Rotation Increment**: Rotate arrows by 45° (index +1 mod 8).
5. **Chain Algorithm**: Identical logic, using 8-way propagation.
6. **Visual/UI Update**: Redraw cell shapes, adjust spacing, update directional assets.

## Glossary
- **Cell**: Basic board unit with state and arrow.
- **State**: Ownership (neutral, Red, Blue).
- **Arrow Direction**: Orientation pointing to neighbor.
- **Propagation**: Recursive capture of cells following arrows.
- **Chain Reaction**: Series of captures triggered by one move.
- **Turn**: Single player’s action cycle.
- **Win Condition**: Criteria to end the game and declare a winner.