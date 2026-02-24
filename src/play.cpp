#include "board.hpp"
#include <unistd.h>

void Board::sum_game_stats() {
    // Count discs using bit manipulation
    int black_count = __builtin_popcountll(black_moves);
    int white_count = __builtin_popcountll(white_moves);
    cout << "Final Score: Black = " << black_count << ", White = " << white_count << endl;
    if (black_count > white_count) {
        cout << "Black wins!" << endl;
    } else if (white_count > black_count) {
        cout << "White wins!" << endl;
    } else {
        cout << "It's a draw!" << endl;
    }
}

int Board::start_game() {
    cout << MSG_STARTING_GAME << endl;
    bool game_over = false;
    while (!game_over) {
        print_board();
        int valid_moves_count = get_valid_moves();
        if (valid_moves_count == 0) {
            cout << "No valid moves for " << ((current_turn == BLACK) ? "Black" : "White") << ". Skipping turn." << endl;
            current_turn = !current_turn; // Skip turn
            valid_moves_count = get_valid_moves();
            if (valid_moves_count == 0) {
                // No valid moves for both players, game over
                cout << "No valid moves for both players. Game over." << endl;
                game_over = true;
                continue;
            }
        }
        pair<int, int> move = parse_move();
        process_move(move.first, move.second, current_turn);
        current_turn = !current_turn; // Switch turn
    }

    // Print final board
    print_board();
    sum_game_stats();
    return OK;
}

int Board::autoplay() {
    cout << "[DEBUG] Starting autoplay..." << endl;
    bool game_over = false;
    while (!game_over) {
        print_board();
        int valid_moves_count = get_valid_moves();
        if (valid_moves_count == 0) {
            cout << "No valid moves for " << ((current_turn == BLACK) ? "Black" : "White") << ". Skipping turn." << endl;
            current_turn = !current_turn;
            valid_moves_count = get_valid_moves();
            if (valid_moves_count == 0) {
                cout << "No valid moves for both players. Game over." << endl;
                game_over = true;
                continue;
            }
        }
        // Pick the first valid move
        pair<int, int> move = *valid_moves.begin();
        cout << ((current_turn == BLACK) ? "Black" : "White") << " plays: " << char('A' + move.second) << (move.first + 1) << endl;
        process_move(move.first, move.second, current_turn);
        current_turn = !current_turn;
        // // Sleep for 0.5 seconds between moves
        // #ifdef _WIN32
        // Sleep(500);
        // #else
        // usleep(500000);
        // #endif
    }
    print_board();
    sum_game_stats();
    return 0;
}
