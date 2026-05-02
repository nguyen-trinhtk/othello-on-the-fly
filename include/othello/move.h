#pragma once
#include <cstdint>

struct Position
{
    int row;
    int col;

    // Works only for 0 <= row, col < 8
    int to_board_index() const { return (row << 3) | col; } // for bitboard indexing
    bool operator==(const Position &other) const // Util: check equal
    {
        return row == other.row && col == other.col;
    }
};

struct Move {
    Position pos;
    // int score; // later, for move ordering
};