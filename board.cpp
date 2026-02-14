#include "board.hpp"
#include "constants.hpp"

Board::Board() {
    cout << DEBUG << MSG_CRT_BRD << endl;
}

string Board::get_square(int r, int c) {
    bool black = (black_moves >> (8 * r + c)) & 1;
    bool white = (white_moves >> (8 * r + c)) & 1;
    // Note that collisions will be handled in another function
    if (black) {
        return BLACK_DISC;
    } else if (white) {
        return WHITE_DISC;
    } else return EMPTY_SQUARE;
}

int Board::set_square(int r, int c, bool disc) {
    if (disc == BLACK) {
        black_moves |= (1ULL << (8 * r + c));
        white_moves &= ~(1ULL << (8 * r + c));
    } else if (disc == WHITE) {
        white_moves |= (1ULL << (8 * r + c));
        black_moves &= ~(1ULL << (8 * r + c));
    }
    return OK;
}

int Board::print_board() {
    // Separate from previous output
    cout << LN_BRK; 

    // Print column header
    cout << "   ";
    for (char col = 'A'; col < 'A' + BOARD_SIZE; ++col) {
        cout << "| " << col << " ";
    }
    cout << endl;

    // Divider line
    string divider((BOARD_SIZE + 1) * 4 - 1, '-');

    // Print each rows
    for (int row = 0; row < BOARD_SIZE; ++row) {
        cout << divider << endl;
        cout << " " << row + 1 << " ";
        for (int col = 0; col < BOARD_SIZE; ++col) {
            cout << "| " << get_square(row, col) << " ";
        }
        cout << endl;
    }

    // Separate from previous output
    cout << LN_BRK;

    // Success
    cout << DEBUG << MSG_PRNT_BRD << endl;
    return OK;
}

int Board::start_game() {
    print_board();
    // set_square(1 - 1, 2 - 1, 1);
    // print_board();
    return OK;
}

Board::~Board() {
    cout << DEBUG << MSG_DSTRCT_BRD << endl;
}

