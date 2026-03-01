
#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <unordered_set>

#include "constants.hpp"

using namespace std;

namespace othello
{
    namespace board
    {

        // Hash function for pair<int, int>
        struct pair_hash
        {
            size_t operator()(const pair<int, int> &p) const
            {
                return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
            }
        };

        class Board
        {
        private:
            uint64_t white_moves;
            uint64_t black_moves;

            int current_turn;
            unordered_set<std::pair<int, int>, pair_hash> valid_moves; // Faster lookup

            // Zobrist table for hashing
            static uint64_t zobrist_table[8][8][2]; // 8x8 board, 2 players

        public:
            Board();

            // Board operations
            inline int get_square(int r, int c) const
            {
                bool black = (black_moves >> (8 * r + c)) & 1;
                bool white = (white_moves >> (8 * r + c)) & 1;
                if (black)
                {
                    return BLACK;
                }
                else if (white)
                {
                    return WHITE;
                }
                else
                    return EMPTY;
            }

            inline string print_square(int disc)
            {
                if (disc == BLACK)
                    return BLACK_DISC;
                else if (disc == WHITE)
                    return WHITE_DISC;
                else if (disc == EMPTY)
                    return EMPTY_SQUARE;
                else
                {
                    cerr << MSG_ERR_INVALID_DISC << endl;
                    return INVALID_SQUARE;
                }
            }

            inline int set_square(int r, int c, int disc)
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
                    cerr << MSG_ERR_INVALID_DISC << endl;
                    return ERR_INVALID_DISC;
                }
                return OK;
            }

            // Evaluate the board: positive if more discs for 'player', negative if fewer
            inline int evaluate(int player) const
            {
                // Count bits for each player
                int black_count = __builtin_popcountll(black_moves);
                int white_count = __builtin_popcountll(white_moves);
                return player == BLACK ? (black_count - white_count) : (white_count - black_count);
            }

            // Getters for disc counts
            inline int get_black_count() const
            {
                return __builtin_popcountll(black_moves);
            }
            inline int get_white_count() const
            {
                return __builtin_popcountll(white_moves);
            }

            // Moves
            // Returns true if neither player has valid moves
            inline bool is_game_over()
            {
                // Save current turn
                int original_turn = current_turn;
                // Check for black
                current_turn = BLACK;
                int black_moves_count = compute_valid_moves();
                // Check for white
                current_turn = WHITE;
                int white_moves_count = compute_valid_moves();
                // Restore turn
                current_turn = original_turn;
                return black_moves_count == 0 && white_moves_count == 0;
            }

            // Getter for valid_moves
            inline const unordered_set<std::pair<int, int>, pair_hash> &get_valid_moves() const
            {
                return valid_moves;
            }
            inline int get_current_player()
            {
                return current_turn;
            }

            inline void set_current_player(int player)
            {
                current_turn = player;
            }

            int compute_valid_moves();
            bool is_valid_move(int r, int c, int player);
            int process_move(int r, int c, int player);
            int print_board();
            pair<int, int> parse_move(); // Loop for user to enter move

            // Game end
            void sum_game_stats();
            ~Board();

            // Zobrist hashing
            uint64_t get_hash() const;
            static void init_zobrist();
        };

    } // namespace board
} // namespace othello

#endif