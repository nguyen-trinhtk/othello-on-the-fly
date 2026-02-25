#include "board.hpp"

Board::Board() {
    this->white_moves = INIT_BOARD_WHITE;
    this->black_moves = INIT_BOARD_BLACK;
    this->current_turn = BLACK; // Black first
    cout << MSG_CREATED_BOARD << endl;
}

void Board::clear_terminal() {
    // ANSI escape code to clear screen and move cursor to top-left
    std::cout << "\033[2J\033[H";
}

int Board::print_board() {
    clear_terminal();
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
    // cout << DEBUG << MSG_PRINTED_BOARD << endl;
    return OK;
}

void Board::sum_game_stats()
{
    int black_count = get_black_count();
    int white_count = get_white_count();
    cout << "Final Score: Black = " << black_count << ", White = " << white_count << endl;
    if (black_count > white_count)
    {
        cout << "Black wins!" << endl;
    }
    else if (white_count > black_count)
    {
        cout << "White wins!" << endl;
    }
    else
    {
        cout << "It's a draw!" << endl;
    }
}

Board::~Board() {
    // cout << DEBUG << MSG_DESTRUCT_BOARD << endl;
}

