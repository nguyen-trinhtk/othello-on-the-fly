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
// Hash function for pair<int, int>
struct pair_hash {
    size_t operator()(const pair<int, int>& p) const {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};

class Board {
    private: 
        uint64_t white_moves;
        uint64_t black_moves;

        bool current_turn;
        unordered_set<std::pair<int, int>, pair_hash> valid_moves; // Faster lookup

        // Board operations
        inline int get_square(int r, int c) {
            bool black = (black_moves >> (8 * r + c)) & 1;
            bool white = (white_moves >> (8 * r + c)) & 1;
            if (black) {
                return BLACK;
            } else if (white) {
                return WHITE;
            } else return EMPTY;
        }

        inline string print_square(int disc) {
            if (disc == BLACK) return BLACK_DISC;
            else if (disc == WHITE) return WHITE_DISC;
            else if (disc == EMPTY) return EMPTY_SQUARE;
            else {
                cout << MSG_ERR_INVALID_DISC << endl;
                return INVALID_SQUARE;
            }
        }

        inline int set_square(int r, int c, int disc) {
            if (disc == BLACK) {
                black_moves |= (1ULL << (8 * r + c));
                white_moves &= ~(1ULL << (8 * r + c));
            } else if (disc == WHITE) {
                white_moves |= (1ULL << (8 * r + c));
                black_moves &= ~(1ULL << (8 * r + c));
            } else {
                cout << MSG_ERR_INVALID_DISC << endl;
                return ERR_INVALID_DISC;
            }
            return OK;
        }

        int print_board();
        void clear_terminal();
        
        // Game logic
        pair<int, int> parse_move(); // Loop for user to enter move
        bool is_valid_move(int r, int c, bool player);

        // This will return the number of valid moves and populate the valid_moves vector. Later: optmz this step knowing last flip?// Later: optmz this step knowing last flip?
        int get_valid_moves();
        int process_move(int r, int c, bool player); 

        // Game end
        void sum_game_stats();

    // Public methods
    public: 
        Board();
        int start_game();
        int autoplay();
        ~Board();
};

#endif