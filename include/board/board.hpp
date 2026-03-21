#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <cstddef>
#include <unordered_set>
#include <utility>
#include <vector>

#include "constants.hpp"

namespace othello
{
    namespace board
    {

        struct Move
        {
            int row;
            int col;
            std::vector<std::pair<int, int>> flipped_discs;

            Move() noexcept
                : row(-1), col(-1) {}
            Move(int r, int c) noexcept
                : row(r), col(c) {}

            [[nodiscard]] bool operator==(const Move &other) const noexcept
            {
                return row == other.row && col == other.col;
            }
        };

        // Hash function for Move
        struct MoveHash
        {
            [[nodiscard]] std::size_t operator()(const Move &m) const noexcept
            {
                return std::hash<int>()(m.row) ^ (std::hash<int>()(m.col) << 1);
            }
        };

        class Board
        {
        private:
            std::uint64_t white_moves;
            std::uint64_t black_moves;

            Disc current_turn;

            // Zobrist table for hashing
            static std::uint64_t zobrist_table[8][8][2]; // 8x8 board, 2 players
            static void init_zobrist();

            [[nodiscard]] static constexpr bool is_in_bounds(int r, int c) noexcept
            {
                return r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE;
            }

            [[nodiscard]] std::unordered_set<Move, MoveHash> generate_valid_moves(Disc player) const;
            [[nodiscard]] std::vector<std::pair<int, int>> collect_flipped_discs(const Move &move, Disc player) const;
        public:
            Board();

            // Board operations
            [[nodiscard]] inline Disc get_square(int r, int c) const
            {
                if (!is_in_bounds(r, c))
                {
                    return EMPTY;
                }
                const bool black = (black_moves >> (8 * r + c)) & 1;
                const bool white = (white_moves >> (8 * r + c)) & 1;
                if (black)
                {
                    return BLACK;
                }
                else if (white)
                {
                    return WHITE;
                }
                else
                {
                    return EMPTY;
                }
            }

            inline int set_square(int r, int c, Disc disc)
            {
                if (!is_in_bounds(r, c))
                {
                    return ERR_OUT_OF_BOUNDS;
                }
                const std::uint64_t bit = 1ULL << (8 * r + c);
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
                else if (disc == EMPTY)
                {
                    black_moves &= ~bit;
                    white_moves &= ~bit;
                }
                else
                {
                    return ERR_INVALID_DISC;
                }
                return OK;
            }

            // Evaluate the board: positive if more discs for 'player', negative if fewer
            [[nodiscard]] inline int evaluate(Disc player) const
            {
                // Count bits for each player
                const int black_count = __builtin_popcountll(black_moves);
                const int white_count = __builtin_popcountll(white_moves);
                return player == BLACK ? (black_count - white_count) : (white_count - black_count);
            }

            // Getters for disc counts
            [[nodiscard]] inline int get_black_count() const
            {
                return __builtin_popcountll(black_moves);
            }
            [[nodiscard]] inline int get_white_count() const
            {
                return __builtin_popcountll(white_moves);
            }

            // Moves
            // Returns true if neither player has valid moves
            [[nodiscard]] inline bool is_game_over() const
            {
                return !has_valid_moves(BLACK) && !has_valid_moves(WHITE);
            }

            [[nodiscard]] inline std::unordered_set<Move, MoveHash> get_valid_moves() const
            {
                return generate_valid_moves(current_turn);
            }
            [[nodiscard]] inline Disc get_current_player() const
            {
                return current_turn;
            }

            inline void set_current_player(Disc player)
            {
                current_turn = player;
            }

            [[nodiscard]] bool has_valid_moves(Disc player) const;
            int compute_valid_moves() const;
            [[nodiscard]] bool is_valid_move(int r, int c, Disc player) const;
            [[nodiscard]] int process_move(const Move &move, Disc player);
            [[nodiscard]] int undo_move(const Move &move, Disc player);
            [[nodiscard]] std::vector<Move> get_moves_for_current_state() const;

            // Zobrist hashing
            [[nodiscard]] std::uint64_t get_hash() const;
        };

    } // namespace board
} // namespace othello

#endif
