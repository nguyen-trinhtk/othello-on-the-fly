#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>
#include <cstdint>

using namespace std;

class Board {
    private: 
        uint64_t white_moves;
        uint64_t black_moves;

        bool current_turn;
        vector<pair<int, int> > valid_moves; // Later: cache?

        // Board operations
        int get_square(int r, int c);
        string print_square(int disc);
        int set_square(int r, int c, int disc);
        int print_board();  
        int clear_board();
        int check_collision();
        
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
        ~Board();
};

#endif