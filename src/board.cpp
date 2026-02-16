#include "board.hpp"
#include "constants.hpp"

Board::Board() {
    this->white_moves = INIT_BOARD_WHITE;
    this->black_moves = INIT_BOARD_BLACK;

    this->current_turn = BLACK; // Black first

    if (check_collision() != OK) {
        cout << DEBUG << MSG_ERR_CREATE_BOARD << endl;
        return;
    }
    cout << DEBUG << MSG_CREATED_BOARD << endl;
}

// Basic board operations
int Board::get_square(int r, int c) {
    bool black = (black_moves >> (8 * r + c)) & 1;
    bool white = (white_moves >> (8 * r + c)) & 1;
    if (black) {
        return BLACK;
    } else if (white) {
        return WHITE;
    } else return EMPTY;
}
string Board::print_square(int disc) {
    if (disc == BLACK) return BLACK_DISC;
    else if (disc == WHITE) return WHITE_DISC;
    else if (disc == EMPTY) return EMPTY_SQUARE;
    else {
        cout << DEBUG << MSG_ERR_INVALID_DISC << endl;
        return INVALID_SQUARE;
    }
}
int Board::set_square(int r, int c, int disc) {
    if (disc == BLACK) {
        black_moves |= (1ULL << (8 * r + c));
        white_moves &= ~(1ULL << (8 * r + c));
    } else if (disc == WHITE) {
        white_moves |= (1ULL << (8 * r + c));
        black_moves &= ~(1ULL << (8 * r + c));
    } else {
        cout << DEBUG << MSG_ERR_INVALID_DISC << endl;
        return ERR_INVALID_DISC;
    }
    return OK;
}
int Board::print_board() {
    cout << LN_BRK; 
    // Print column header
    cout << "   ";
    for (char col = 'A'; col < 'A' + BOARD_SIZE; ++col) {
        cout << "| " << col << " ";
    }
    cout << endl;
    // Print each rows
    string divider((BOARD_SIZE + 1) * 4 - 1, '-');
    for (int row = 0; row < BOARD_SIZE; ++row) {
        cout << divider << endl;
        cout << " " << row + 1 << " ";
        for (int col = 0; col < BOARD_SIZE; ++col) {
            cout << "| " << print_square(get_square(row, col)) << " ";
        }
        cout << endl;
    }
    cout << LN_BRK;
    cout << DEBUG << MSG_PRINTED_BOARD << endl;
    return OK;
}
int Board::clear_board() {
    white_moves = 0;
    black_moves = 0;
    return OK;
}
int Board::check_collision() {
    if (white_moves & black_moves) {
        cout << DEBUG << MSG_ERR_COLLISION << endl;
        return ERR_COLLISION;
    }

    return OK;
}



int Board::start_game() {
}

Board::~Board() {
    cout << DEBUG << MSG_DESTRUCT_BOARD << endl;
}

