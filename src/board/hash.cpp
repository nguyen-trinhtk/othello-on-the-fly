#include "board/board.hpp"

#include <random>

namespace othello
{
    namespace board
    {
        namespace
        {
            constexpr std::uint64_t BLACK_TO_MOVE_HASH = 0xABCDEF1234567890ULL;
            constexpr std::uint64_t WHITE_TO_MOVE_HASH = 0x1234567890ABCDEFULL;
        } // namespace

        std::uint64_t Board::zobrist_table[8][8][2];

        void Board::init_zobrist()
        {
            std::mt19937_64 rng(0xDEADBEEF); // Fixed seed for reproducibility
            std::uniform_int_distribution<std::uint64_t> dist;
            for (int r = 0; r < 8; ++r)
            {
                for (int c = 0; c < 8; ++c)
                {
                    for (int p = 0; p < 2; ++p)
                    {
                        zobrist_table[r][c][p] = dist(rng);
                    }
                }
            }
        }

        void Board::ensure_zobrist_initialized()
        {
            static const bool initialized = []() {
                Board::init_zobrist();
                return true;
            }();
            (void)initialized;
        }

        void Board::xor_square_hash(int r, int c, Disc disc) noexcept
        {
            if (disc == BLACK || disc == WHITE)
            {
                hash_value ^= zobrist_table[r][c][to_index(disc)];
            }
        }

        void Board::xor_mask_hash(std::uint64_t mask, Disc disc) noexcept
        {
            while (mask != 0)
            {
                const int square = __builtin_ctzll(mask);
                xor_square_hash(square / BOARD_SIZE, square % BOARD_SIZE, disc);
                mask &= (mask - 1);
            }
        }

        void Board::transfer_mask_hash(std::uint64_t mask, Disc from, Disc to) noexcept
        {
            xor_mask_hash(mask, from);
            xor_mask_hash(mask, to);
        }

        void Board::rebuild_hash()
        {
            ensure_zobrist_initialized();

            hash_value = 0;
            for (int r = 0; r < 8; ++r)
            {
                for (int c = 0; c < 8; ++c)
                {
                    const Disc sq = get_square(r, c);
                    if (sq == BLACK)
                    {
                        xor_square_hash(r, c, BLACK);
                    }
                    else if (sq == WHITE)
                    {
                        xor_square_hash(r, c, WHITE);
                    }
                }
            }

            hash_value ^= (current_turn == BLACK ? BLACK_TO_MOVE_HASH : WHITE_TO_MOVE_HASH);
        }

        int Board::set_square(int r, int c, Disc disc)
        {
            if (!is_in_bounds(r, c))
            {
                return ERR_OUT_OF_BOUNDS;
            }
            if (!is_player(disc) && disc != EMPTY)
            {
                return ERR_INVALID_DISC;
            }

            const Disc previous = get_square(r, c);
            if (previous == disc)
            {
                return OK;
            }

            ensure_zobrist_initialized();

            if (previous != EMPTY)
            {
                xor_square_hash(r, c, previous);
            }

            const std::uint64_t bit = bit_at(r, c);
            if (disc == BLACK)
            {
                black_moves |= bit;
                white_moves &= ~bit;
            }
            else if (disc == WHITE)
            {
                white_moves |= bit;
                black_moves &= ~bit;
            }
            else
            {
                black_moves &= ~bit;
                white_moves &= ~bit;
            }

            if (disc != EMPTY)
            {
                xor_square_hash(r, c, disc);
            }

            return OK;
        }

        void Board::set_current_player(Disc player)
        {
            if (current_turn == player)
            {
                return;
            }

            ensure_zobrist_initialized();
            hash_value ^= (current_turn == BLACK ? BLACK_TO_MOVE_HASH : WHITE_TO_MOVE_HASH);
            current_turn = player;
            hash_value ^= (current_turn == BLACK ? BLACK_TO_MOVE_HASH : WHITE_TO_MOVE_HASH);
        }

        std::uint64_t Board::get_hash() const
        {
            return hash_value;
        }

    } // namespace board
} // namespace othello
