#include "board/board.hpp"

#include <algorithm>

namespace othello
{
    namespace board
    {
        namespace
        {
            [[nodiscard]] std::uint64_t bit_at(int row, int col) noexcept
            {
                return 1ULL << (BOARD_SIZE * row + col);
            }

            [[nodiscard]] std::uint64_t mask_for_discs(const std::vector<std::pair<int, int>> &discs) noexcept
            {
                std::uint64_t mask = 0;
                for (const auto &[row, col] : discs)
                {
                    mask |= bit_at(row, col);
                }
                return mask;
            }
        } // namespace

        std::unordered_set<Move, MoveHash> Board::generate_valid_moves(Disc player) const
        {
            std::unordered_set<Move, MoveHash> valid_moves;
            if (!is_player(player))
            {
                return valid_moves;
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

            for (int square = 0; square < BOARD_SIZE * BOARD_SIZE; ++square)
            {
                if ((moves >> square) & 1ULL)
                {
                    valid_moves.emplace(square / BOARD_SIZE, square % BOARD_SIZE);
                }
            }
            return valid_moves;
        }

        std::vector<std::pair<int, int>> Board::collect_flipped_discs(const Move &move, Disc player) const
        {
            if (!is_player(player) || !Board::is_in_bounds(move.row, move.col) || get_square(move.row, move.col) != EMPTY)
            {
                return {};
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

            if (to_flip == 0)
            {
                return {};
            }

            std::vector<std::pair<int, int>> flipped_discs;
            flipped_discs.reserve(__builtin_popcountll(to_flip));
            for (int square = 0; square < BOARD_SIZE * BOARD_SIZE; ++square)
            {
                if ((to_flip >> square) & 1ULL)
                {
                    flipped_discs.emplace_back(square / BOARD_SIZE, square % BOARD_SIZE);
                }
            }
            return flipped_discs;
        }

        bool Board::has_valid_moves(Disc player) const
        {
            return !generate_valid_moves(player).empty();
        }

        bool Board::is_valid_move(int r, int c, Disc player) const
        {
            if (!Board::is_in_bounds(r, c) || !is_player(player))
            {
                return false;
            }
            const auto valid_moves = generate_valid_moves(player);
            return valid_moves.find(Move(r, c)) != valid_moves.end();
        }

        int Board::compute_valid_moves() const
        {
            return static_cast<int>(generate_valid_moves(current_turn).size());
        }

        int Board::process_move(const Move &move, Disc player)
        {
            std::vector<std::pair<int, int>> flipped_discs = move.flipped_discs;
            if (flipped_discs.empty())
            {
                flipped_discs = collect_flipped_discs(move, player);
            }
            if (flipped_discs.empty())
            {
                return ERR_INVALID_MOVE;
            }

            const std::uint64_t move_bit = bit_at(move.row, move.col);
            const std::uint64_t to_flip = mask_for_discs(flipped_discs);

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

        int Board::undo_move(const Move &move, Disc player)
        {
            if (!is_player(player) || !Board::is_in_bounds(move.row, move.col) || move.flipped_discs.empty())
            {
                return ERR_INVALID_MOVE;
            }

            const std::uint64_t move_bit = bit_at(move.row, move.col);
            const std::uint64_t to_flip = mask_for_discs(move.flipped_discs);

            if (player == BLACK)
            {
                black_moves &= ~(move_bit | to_flip);
                white_moves |= to_flip;
            }
            else
            {
                white_moves &= ~(move_bit | to_flip);
                black_moves |= to_flip;
            }

            return OK;
        }

        std::vector<Move> Board::get_moves_for_current_state() const
        {
            std::vector<Move> moves;
            const auto valid_moves = generate_valid_moves(current_turn);
            moves.reserve(valid_moves.size());
            for (const auto &candidate : valid_moves)
            {
                Move move = candidate;
                move.flipped_discs = collect_flipped_discs(move, current_turn);
                moves.push_back(std::move(move));
            }

            std::sort(
                moves.begin(),
                moves.end(),
                [](const Move &lhs, const Move &rhs)
                {
                    if (lhs.row != rhs.row)
                    {
                        return lhs.row < rhs.row;
                    }
                    return lhs.col < rhs.col;
                });
            return moves;
        }
    }
}
