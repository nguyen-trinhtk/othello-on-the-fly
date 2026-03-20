#include "board/board.hpp"

namespace othello
{
    namespace board
    {
        bool Board::is_valid_move(int r, int c, Disc player) const
        {
            if (r < 0 || r >= 8 || c < 0 || c >= 8)
            {
                return false;
            }
            if (!is_player(player))
            {
                return false;
            }
            return valid_moves.find(Move(r, c)) != valid_moves.end();
        }

        int Board::compute_valid_moves()
        {
            valid_moves.clear();
            std::uint64_t player_bits = current_turn == BLACK ? black_moves : white_moves;
            std::uint64_t opp_bits = current_turn == BLACK ? white_moves : black_moves;
            const std::uint64_t empty = ~(black_moves | white_moves);

            std::uint64_t moves = 0;

            for (int d = 0; d < 8; ++d)
            {
                std::uint64_t mask = 0;
                std::uint64_t candidates = player_bits;
                for (int step = 0; step < 6; ++step)
                {
                    if (dirs[d].shift > 0)
                        candidates = (candidates & dirs[d].mask) << dirs[d].shift;
                    else
                        candidates = (candidates & dirs[d].mask) >> -dirs[d].shift;
                    mask |= candidates & opp_bits;
                    candidates &= opp_bits;
                    if (!candidates)
                        break;
                }
                // Final shift
                if (dirs[d].shift > 0)
                    mask = (mask & dirs[d].mask) << dirs[d].shift;
                else
                    mask = (mask & dirs[d].mask) >> -dirs[d].shift;
                moves |= mask & empty;
            }

            // Fill valid_moves set from bitboard
            for (int i = 0; i < 64; ++i)
            {
                if ((moves >> i) & 1)
                {
                    int r = i / 8;
                    int c = i % 8;
                    valid_moves.insert(Move(r, c));
                }
            }
            return valid_moves.size();
        }

        int Board::process_move(Move &move, Disc player)
        {
            const int r = move.row;
            const int c = move.col;
            if (!is_valid_move(r, c, player))
                return ERR_INVALID_MOVE;

            const std::uint64_t move_bit = 1ULL << (8 * r + c);
            const std::uint64_t player_bits = player == BLACK ? black_moves : white_moves;
            const std::uint64_t opp_bits = player == BLACK ? white_moves : black_moves;

            std::uint64_t to_flip = 0;

            for (int d = 0; d < 8; ++d)
            {
                std::uint64_t mask = 0;
                std::uint64_t cur = move_bit;

                for (int step = 0; step < 7; ++step)
                {
                    if (dirs[d].shift > 0)
                        cur = (cur & dirs[d].mask) << dirs[d].shift;
                    else
                        cur = (cur & dirs[d].mask) >> -dirs[d].shift;

                    if (!cur)
                        break; // shifted off board

                    if (cur & opp_bits)
                    {
                        mask |= cur;
                    }
                    else
                    {
                        if ((cur & player_bits) && mask)
                            to_flip |= mask;
                        break;
                    }
                }
            }

            // Record flipped discs
            move.flipped_discs.clear();
            for (int i = 0; i < 64; ++i)
            {
                if ((to_flip >> i) & 1)
                    move.flipped_discs.push_back({i / 8, i % 8});
            }

            // Place disc and flip
            if (player == BLACK)
            {
                black_moves |= move_bit | to_flip;
                white_moves &= ~to_flip;
            }
            else
            {
                white_moves |= move_bit | to_flip;
                black_moves &= ~to_flip;
            }

            return OK;
        }

        std::vector<Move> Board::get_moves_for_current_state()
        {
            const std::uint64_t hash = get_hash();
            const auto it = move_gen_cache.find(hash);
            if (it != move_gen_cache.end())
            {
                return it->second;
            }
            std::vector<Move> moves;
            // Generate moves for current player
            compute_valid_moves();
            for (const auto &m : valid_moves)
            {
                Move move = m;
                Board child = *this;
                if (child.process_move(move, current_turn) == OK)
                {
                    moves.push_back(move);
                }
            }
            move_gen_cache[hash] = moves;
            return moves;
        }
    }
}
