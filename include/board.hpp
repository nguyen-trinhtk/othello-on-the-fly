#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include <cstdint>
// #include <utility>

using namespace std;


class Board {
        // Debug helpers
        void debug_valid_moves();
    // attributes
    private: 
        uint64_t white_moves;
        uint64_t black_moves;

        bool current_turn;
        vector<pair<int, int> > valid_moves; // Later: cache?

        // // Internal game methods
        // // Basic set up
        int get_square(int r, int c);
        string print_square(int disc);
        int set_square(int r, int c, int disc);
        int print_board();  
        int clear_board();
        int check_collision();
        

        // // Game logic
        pair<int, int> parse_move(); // Loop for user to enter move
        bool is_valid_move(int r, int c, bool player);

        // This will return the number of valid moves and populate the valid_moves vector. Later: optmz this step knowing last flip?// Later: optmz this step knowing last flip?
        int get_valid_moves();        // void process_move(); // Apply move and flip stuffs
        int process_move(int r, int c, bool player); // Apply move and flip stuffs
        
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