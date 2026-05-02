#pragma once
#include <optional>

enum class Player { BLACK, WHITE };

// Get opponent player
inline Player opponent(Player p) {
    return p == Player::BLACK ? Player::WHITE : Player::BLACK;
}

enum class GameError {
    CELL_OCCUPIED,
    INVALID_MOVE,
    OUT_OF_BOUNDS,
};

using OptError = std::optional<GameError>;

