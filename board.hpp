#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include <cstdint>

using namespace std;


class Board {
    // attributes
    private: 
        uint64_t white_moves;
        uint64_t black_moves;

        // int current_turn; // 0 for white, 1 for black

        // // Internal game methods
        // // Basic set up
        string get_square(int r, int c);
        int set_square(int r, int c, bool disc);
        int print_board();  
        int clear_board();
        int check_collision();
        
        // // Game logic
        // void generate_all_valid_moves(); // Later: optmz this step knowing last flip?
        // void parse_move(); // Loop for user to enter move
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