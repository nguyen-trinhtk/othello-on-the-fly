#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <cstddef>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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

            Move() : row(-1), col(-1) {}
            Move(int r, int c) : row(r), col(c) {}

            bool operator==(const Move &other) const
            {
                return row == other.row && col == other.col;
            }
        };

        // Hash function for Move
        struct MoveHash
        {
            std::size_t operator()(const Move &m) const
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
            std::unordered_set<Move, MoveHash> valid_moves; // Faster lookup

            // Move generation cache: board hash -> vector of moves
            std::unordered_map<std::uint64_t, std::vector<Move>> move_gen_cache;

            // Zobrist table for hashing
            static std::uint64_t zobrist_table[8][8][2]; // 8x8 board, 2 players
            static void init_zobrist();
        public:
            Board();

            // Board operations
            [[nodiscard]] inline Disc get_square(int r, int c) const
            {
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
                if (disc == BLACK)
                {
                    black_moves |= (1ULL << (8 * r + c));
                    white_moves &= ~(1ULL << (8 * r + c));
                }
                else if (disc == WHITE)
                {
                    white_moves |= (1ULL << (8 * r + c));
                    black_moves &= ~(1ULL << (8 * r + c));
                }
                else if (disc == EMPTY)
                {
                    black_moves &= ~(1ULL << (8 * r + c));
                    white_moves &= ~(1ULL << (8 * r + c));
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
            [[nodiscard]] inline bool is_game_over()
            {
                const Disc original_turn = current_turn;
                const auto original_valid_moves = valid_moves;

                // Check for black
                current_turn = BLACK;
                const int black_moves_count = compute_valid_moves();

                // Check for white
                current_turn = WHITE;
                const int white_moves_count = compute_valid_moves();

                // Restore observable board state
                current_turn = original_turn;
                valid_moves = original_valid_moves;

                return black_moves_count == 0 && white_moves_count == 0;
            }

            // Getter for valid_moves
            [[nodiscard]] inline const std::unordered_set<Move, MoveHash> &get_valid_moves() const
            {
                return valid_moves;
            }
            [[nodiscard]] inline Disc get_current_player() const
            {
                return current_turn;
            }

            inline void set_current_player(Disc player)
            {
                current_turn = player;
            }

            int compute_valid_moves();
            bool is_valid_move(int r, int c, Disc player) const;
            int process_move(Move &move, Disc player);
            std::vector<Move> get_moves_for_current_state();

            // Zobrist hashing
            [[nodiscard]] std::uint64_t get_hash() const;
        };

    } // namespace board
} // namespace othello

#endif
