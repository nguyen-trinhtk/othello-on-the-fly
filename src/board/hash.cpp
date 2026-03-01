
#include "board/board.hpp"
#include <random>

namespace othello
{
    namespace board
    {
        uint64_t Board::zobrist_table[8][8][2];
        
        void Board::init_zobrist()
        {
            std::mt19937_64 rng(0xDEADBEEF); // Fixed seed for reproducibility
            std::uniform_int_distribution<uint64_t> dist;
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

        uint64_t Board::get_hash() const
        {
            uint64_t h = 0;
            for (int r = 0; r < 8; ++r)
            {
                for (int c = 0; c < 8; ++c)
                {
                    int sq = get_square(r, c);
                    if (sq == BLACK)
                    {
                        h ^= zobrist_table[r][c][0];
                    }
                    else if (sq == WHITE)
                    {
                        h ^= zobrist_table[r][c][1];
                    }
                }
            }
            // Include current player in hash
            h ^= (current_turn == BLACK ? 0xABCDEF1234567890ULL : 0x1234567890ABCDEFULL);
            return h;
        }

    } // namespace board
} // namespace othello
