#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include <cstdint>
// #include <utility>

using namespace std;


class Board {
    // attributes
    private: 
        uint64_t white_moves;
        uint64_t black_moves;

        bool current_turn;
        vector<pair<int, int>> valid_moves; // Later: cache?

        // // Internal game methods
        // // Basic set up
        string get_square(int r, int c);
        int set_square(int r, int c, bool disc);
        int print_board();  
        int clear_board();
        int check_collision();
        
        // // Game logic
        int get_valid_moves(); // Later: optmz this step knowing last flip?
        pair<int, int> parse_move(); // Loop for user to enter move
        // void process_move(); // Apply move and flip stuffs
        
        // // Game end
        // void sum_game_stats();

    // public methods
    public: 
        Board();
        int start_game();
        // void quit_game();
        ~Board();
};

#endif