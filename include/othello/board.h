#pragma once
#include <cstdint>
#include <functional>
#include <optional>

#include "move.h"
#include "types.h"

class Board
{
    friend struct std::hash<Board>;

private:
    uint64_t m_black_discs = 0; // Bitboard for black disks
    uint64_t m_white_discs = 0; // Bitboard for white disks

public:
    static constexpr int SIZE = 8; // Board size const

    Board clone() const; // Prototype
    uint64_t discs(Player player) const;
    uint64_t occupied_discs() const;
    void apply_bit_move(Player player, uint64_t move_bit, uint64_t flip_mask);
    std::optional<Player> get_disc(const Position &pos) const;
    OptError occupy_position(const Position &pos, Player player);
    OptError flip_disc(const Position &pos, Player player);
};

// Hash function, for trans table
template <>
struct std::hash<Board>
{
    size_t operator()(const Board &board) const
    {
        constexpr uint64_t HASH_CONST = 0x9e3779b97f4a7c15ULL;
        return board.m_black_discs ^ (board.m_white_discs * HASH_CONST);
    }
};
