#include "board.hpp"
#include "constants.hpp"

int Board::start_game() {
    cout << DEBUG << MSG_STARTING_GAME << endl;
    bool game_over = false;
    while (!game_over) {
        print_board();
        int valid_moves_count = get_valid_moves();
        if (valid_moves_count == 0) {
            cout << DEBUG << "No valid moves for " << ((current_turn == BLACK) ? "Black" : "White") << ". Skipping turn." << endl;
            current_turn = !current_turn; // Skip turn
            valid_moves_count = get_valid_moves();
            if (valid_moves_count == 0) {
                cout << DEBUG << "No valid moves for both players. Game over." << endl;
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
    return OK;
}
