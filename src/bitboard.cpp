#include "bitboard.h"

#include <bit>

namespace
{
    constexpr uint64_t NOT_FILE_A = 0xfefefefefefefefeULL;
    constexpr uint64_t NOT_FILE_H = 0x7f7f7f7f7f7f7f7fULL;
    constexpr int MAX_CAPTURE_CHAIN = 5;

    using DirectionShift = uint64_t (*)(uint64_t);

    uint64_t shift_north(uint64_t bits)
    {
        return bits >> 8;
    }

    uint64_t shift_south(uint64_t bits)
    {
        return bits << 8;
    }

    uint64_t shift_east(uint64_t bits)
    {
        return (bits << 1) & NOT_FILE_A;
    }

    uint64_t shift_west(uint64_t bits)
    {
        return (bits >> 1) & NOT_FILE_H;
    }

    uint64_t shift_north_east(uint64_t bits)
    {
        return (bits >> 7) & NOT_FILE_A;
    }

    uint64_t shift_north_west(uint64_t bits)
    {
        return (bits >> 9) & NOT_FILE_H;
    }

    uint64_t shift_south_east(uint64_t bits)
    {
        return (bits << 9) & NOT_FILE_A;
    }

    uint64_t shift_south_west(uint64_t bits)
    {
        return (bits << 7) & NOT_FILE_H;
    }

    constexpr DirectionShift DIRECTION_SHIFTS[] = {
        shift_north,
        shift_south,
        shift_east,
        shift_west,
        shift_north_east,
        shift_north_west,
        shift_south_east,
        shift_south_west,
    };
}

uint64_t Bitboard::bit_at(int row, int col)
{
    return 1ULL << ((row << 3) | col);
}

uint64_t Bitboard::bit_at_index(int index)
{
    return 1ULL << index;
}

uint64_t Bitboard::occupied(const Bitboards &bitboards)
{
    return bitboards.player | bitboards.opponent;
}

uint64_t Bitboard::empty(const Bitboards &bitboards)
{
    return ~occupied(bitboards);
}

uint64_t Bitboard::valid_moves_mask(const Bitboards &bitboards)
{
    uint64_t moves = 0;
    const uint64_t empty_bits = empty(bitboards);

    for (DirectionShift shift : DIRECTION_SHIFTS)
    {
        uint64_t candidates = shift(bitboards.player) & bitboards.opponent;
        for (int i = 0; i < MAX_CAPTURE_CHAIN; ++i)
            candidates |= shift(candidates) & bitboards.opponent;

        moves |= shift(candidates) & empty_bits;
    }

    return moves;
}

uint64_t Bitboard::flip_mask(const Bitboards &bitboards, uint64_t move_bit)
{
    uint64_t flips = 0;

    for (DirectionShift shift : DIRECTION_SHIFTS)
    {
        uint64_t ray = shift(move_bit);
        uint64_t directional_flips = 0;

        while ((ray & bitboards.opponent) != 0)
        {
            directional_flips |= ray;
            ray = shift(ray);
        }

        if ((ray & bitboards.player) != 0)
            flips |= directional_flips;
    }

    return flips;
}

int Bitboard::pop_lsb_index(uint64_t &bits)
{
    const int index = static_cast<int>(std::countr_zero(bits));
    bits &= bits - 1;
    return index;
}
