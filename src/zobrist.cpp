#include "zobrist.h"

#include <array>
#include <bit>
#include <cstdint>
#include <random>

namespace
{
    struct Tables {
        // Tables of random values for each square
        std::array<uint64_t, 64> black{};
        std::array<uint64_t, 64> white{};
        uint64_t side_black = 0; // player to move
    };

    Tables make_tables() {
        std::mt19937_64 rng(0x4F7468656C6C6FULL); // Fixed seed -> reproducible
        Tables t;
        for (int i = 0; i < 64; ++i) {
            // For each square: generate random value
            t.black[static_cast<std::size_t>(i)] = rng();
            t.white[static_cast<std::size_t>(i)] = rng();
        }
        t.side_black = rng(); // Random val for player to move
        return t;
    }

    const Tables &tables() {
        // Lazy initialization
        static const Tables t = make_tables();
        return t;
    }

    void xor_bits(uint64_t bits, const std::array<uint64_t, 64> &tab, uint64_t &key) {
        while (bits != 0)
        {
            // For each bit: xor with random value
            const int sq = std::countr_zero(bits);
            key ^= tab[static_cast<std::size_t>(sq)];
            bits &= bits - 1;
        }
    }
} // namespace

uint64_t zobrist_key(const Board &board, Player side_to_move)
{
    const Tables &t = tables(); 
    uint64_t key = 0;
    // Xor discs
    xor_bits(board.discs(Player::BLACK), t.black, key);
    xor_bits(board.discs(Player::WHITE), t.white, key);
    if (side_to_move == Player::BLACK)
        // Xor player to move
        key ^= t.side_black;
    return key;
}
