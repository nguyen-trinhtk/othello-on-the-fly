# Implementation Details

This document explains how othello-on-the-fly is implemented, including design decisions, algorithms, and code structure.

## Structure
- Modular C++ code with clear separation between game logic, board handling, and AI.
- Uses C++17 features for modern syntax and performance.
- Command-line interface for mode selection (PvP, PvE, Autoplay).

## Key Components
- `Game` class: Handles game flow and mode selection.
- `Board` class: Manages board state, move validation, and display.
- `AlphaBeta` (AI): Implements alpha-beta pruning for AI moves (based on minimax algorithm).

## Build System
- Uses CMake for cross-platform builds.

## How Modes Work
- **PvP**: Two players alternate moves via terminal input.
- **PvE**: Player vs AI; AI uses alpha-beta pruning.
- **Autoplay**: AI vs AI; moves are automated.

## Further Details
I will add more technical explanations, diagrams, or code snippets later!
