
# Othello, on-the-fly

A C++ Othello engine that supports both PvP and PvE. 

## Installation

Build `othello-on-the-fly` with CMake:

```bash
    mkdir -p build
    cd build
    cmake ..
    make
```
Then run this binary in `build/`:
```bash
./othello
```
with one of the following options: 
| Command                | Mode                |
|------------------------|---------------------|
| `./othello --autoplay` | AI vs AI            |
| `./othello --pvp`      | Player vs Player    |
| `./othello --pve`      | Player vs AI        |
| `./othello --help`     | Show help message   |
    
## Optimizations

The engine uses a **bitboard representation**, enabling fast board updates and move generation through efficient bitwise operations.

Search is optimized with **alpha–beta pruning**, while a transposition table with Zobrist hashing caches previously evaluated positions to avoid redundant calculations. Move caching is also used to reduce repeated work during search.

**Upcoming Improvements:** heuristic-based move ordering and reinforcement learning to improve position evaluation.

See [Implementation Details](docs/implementation.md) for more detailed design and technical explanations.

## Project Structure

- `src/`     - Source code
- `include/` - Header files
- `build/`   - Build output (created by CMake)
- `docs/`    - Documentation and notes

## Requirements

- CMake >= 3.10
- C++17 compiler (e.g., g++, clang++)

