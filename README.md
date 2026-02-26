# Othello On The Fly

> Check out [Othello Rules](docs/rules.md) for the official game rules.

Welcome to `othello-on-the-fly`! This is a simple, fast, and fun command-line version of Othello (Reversi) written in C++. Play against a friend, challenge the AI, or watch two AIs battle it out.

**Fun fact:** The name comes from its origin—I started developing this game while on a plane, coding on the fly! Now, it's actively updated and improved on the ground.

> 🚧 **Current status:** Fully playable and tested, but the AI still needs rigorous optimization. Please be gentle with the game!

## Quick Start

1. **Build the game:**
   ```bash
   mkdir -p build
   cd build
   cmake ..
   make
   ```
2. **Run the game:**
   ```bash
   ./othello --help
   ```
   (Make sure you are in the `build` directory.)

## Game Modes

Choose how you want to play:

| Command                | Mode                |
|------------------------|---------------------|
| `./othello --autoplay` | AI vs AI            |
| `./othello --pvp`      | Player vs Player    |
| `./othello --pve`      | Player vs AI        |
| `./othello --help`     | Show help message   |

Just run the command you want in your terminal!

## Project Structure

- `src/`     - Source code
- `include/` - Header files
- `build/`   - Build output (created by CMake)
- `docs/`    - Documentation and notes

## Requirements

- CMake >= 3.10
- C++17 compiler (e.g., g++, clang++)

## Documentation

See [Implementation Details](docs/implementation.md) for more detailed design and technical explanations.

