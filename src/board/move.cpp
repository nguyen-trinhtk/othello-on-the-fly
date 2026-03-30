#include "board/board.hpp"

namespace othello
{
    namespace board
    {
        std::uint64_t Board::generate_valid_move_mask(Disc player) const
        {
            if (!is_player(player))
            {
                return 0;
            }

            const std::uint64_t player_bits = player == BLACK ? black_moves : white_moves;
            const std::uint64_t opp_bits = player == BLACK ? white_moves : black_moves;
            const std::uint64_t empty = ~(black_moves | white_moves);

            std::uint64_t moves = 0;

            for (int direction = 0; direction < 8; ++direction)
            {
                std::uint64_t mask = 0;
                std::uint64_t candidates = player_bits;
                for (int step = 0; step < 6; ++step)
                {
                    if (dirs[direction].shift > 0)
                    {
                        candidates = (candidates & dirs[direction].mask) << dirs[direction].shift;
                    }
                    else
                    {
                        candidates = (candidates & dirs[direction].mask) >> -dirs[direction].shift;
                    }
                    mask |= candidates & opp_bits;
                    candidates &= opp_bits;
                    if (!candidates)
                    {
                        break;
                    }
                }

                if (dirs[direction].shift > 0)
                {
                    mask = (mask & dirs[direction].mask) << dirs[direction].shift;
                }
                else
                {
                    mask = (mask & dirs[direction].mask) >> -dirs[direction].shift;
                }
                moves |= mask & empty;
            }

            return moves;
        }

        std::uint64_t Board::collect_flip_mask(const Move &move, Disc player) const
        {
            if (!is_player(player) || !Board::is_in_bounds(move.row, move.col) || get_square(move.row, move.col) != EMPTY)
            {
                return 0;
            }

            const std::uint64_t move_bit = bit_at(move.row, move.col);
            const std::uint64_t player_bits = player == BLACK ? black_moves : white_moves;
            const std::uint64_t opp_bits = player == BLACK ? white_moves : black_moves;

            std::uint64_t to_flip = 0;

            for (int direction = 0; direction < 8; ++direction)
            {
                std::uint64_t mask = 0;
                std::uint64_t current = move_bit;

                for (int step = 0; step < 7; ++step)
                {
                    if (dirs[direction].shift > 0)
                    {
                        current = (current & dirs[direction].mask) << dirs[direction].shift;
                    }
                    else
                    {
                        current = (current & dirs[direction].mask) >> -dirs[direction].shift;
                    }

                    if (!current)
                    {
                        break;
                    }

                    if (current & opp_bits)
                    {
                        mask |= current;
                        continue;
                    }

                    if ((current & player_bits) && mask)
                    {
                        to_flip |= mask;
                    }
                    break;
                }
            }

            return to_flip;
        }

        bool Board::has_valid_moves(Disc player) const
        {
            return generate_valid_move_mask(player) != 0;
        }

        bool Board::is_valid_move(int r, int c, Disc player) const
        {
            if (!Board::is_in_bounds(r, c) || !is_player(player))
            {
                return false;
            }

            return (generate_valid_move_mask(player) & bit_at(r, c)) != 0;
        }

        int Board::compute_valid_moves() const
        {
            return __builtin_popcountll(generate_valid_move_mask(current_turn));
        }

        int Board::process_move(const Move &move, Disc player)
        {
            std::uint64_t to_flip = move.flip_mask;
            if (to_flip == 0)
            {
                to_flip = collect_flip_mask(move, player);
            }
            if (to_flip == 0)
            {
                return ERR_INVALID_MOVE;
            }

            const std::uint64_t move_bit = bit_at(move.row, move.col);

            if (player == BLACK)
            {
                xor_square_hash(move.row, move.col, BLACK);
                transfer_mask_hash(to_flip, WHITE, BLACK);
                black_moves |= move_bit | to_flip;
                white_moves &= ~to_flip;
            }
            else
            {
                xor_square_hash(move.row, move.col, WHITE);
                transfer_mask_hash(to_flip, BLACK, WHITE);
                white_moves |= move_bit | to_flip;
                black_moves &= ~to_flip;
            }

            return OK;
        }

        int Board::undo_move(const Move &move, Disc player)
        {
            if (!is_player(player) || !Board::is_in_bounds(move.row, move.col) || move.flip_mask == 0)
            {
                return ERR_INVALID_MOVE;
            }

            const std::uint64_t move_bit = bit_at(move.row, move.col);
            const std::uint64_t to_flip = move.flip_mask;

            if (player == BLACK)
            {
                xor_square_hash(move.row, move.col, BLACK);
                transfer_mask_hash(to_flip, BLACK, WHITE);
                black_moves &= ~(move_bit | to_flip);
                white_moves |= to_flip;
            }
            else
            {
                xor_square_hash(move.row, move.col, WHITE);
                transfer_mask_hash(to_flip, WHITE, BLACK);
                white_moves &= ~(move_bit | to_flip);
                black_moves |= to_flip;
            }

            return OK;
        }

        std::vector<Move> Board::get_moves_for_current_state() const
        {
            std::uint64_t move_mask = generate_valid_move_mask(current_turn);
            std::vector<Move> moves;
            moves.reserve(__builtin_popcountll(move_mask));

            while (move_mask != 0)
            {
                const int square = __builtin_ctzll(move_mask);
                move_mask &= (move_mask - 1);

                Move move(square / BOARD_SIZE, square % BOARD_SIZE);
                move.flip_mask = collect_flip_mask(move, current_turn);
                moves.push_back(move);
            }

            return moves;
        }
    }
}
