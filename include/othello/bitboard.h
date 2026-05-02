#pragma once

#include <cstdint>

struct Bitboards
{
    uint64_t player;
    uint64_t opponent;
};

namespace Bitboard
{
    uint64_t bit_at(int row, int col);
    uint64_t bit_at_index(int index);

    uint64_t occupied(const Bitboards &bitboards);
    uint64_t empty(const Bitboards &bitboards);

    uint64_t valid_moves_mask(const Bitboards &bitboards);
    uint64_t flip_mask(const Bitboards &bitboards, uint64_t move_bit);

    int pop_lsb_index(uint64_t &bits);
}
