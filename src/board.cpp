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
string Board::get_square(int r, int c) {
    bool black = (black_moves >> (8 * r + c)) & 1;
    bool white = (white_moves >> (8 * r + c)) & 1;
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

pair<int, int> Board::parse_move() {
    // Loop for user to enter move
    string move_str;
    string current_player = (current_turn == BLACK) ? "Black" : "White";
    int row, col;
    cout << current_player << "'s turn! ";

    while (1) {
        cout << "Enter your move: ";
        cin >> move_str;

        // Normalize input
        move_str.erase(remove(move_str.begin(), move_str.end(), ' '), move_str.end());
        // Normalize first character to uppercase
        move_str[0] = toupper(move_str[0]);

        // Validate move format
        if (move_str.length() != 2 || !isalpha(move_str[0]) || !isdigit(move_str[1])) {
            cout << MSG_ERR_INVALID_MOVE_FORMAT << endl;
            continue;
        }
        // Check if first character is a letter and second is a digit
        if ((move_str[0] < 'A' || move_str[0] > 'H') || (move_str[1] < '1' || move_str[1] > '8')) {
            cout << MSG_ERR_OUT_OF_BOUNDS << endl;
            continue;
        }

        // Parse move
        col = move_str[0] - 'A';
        row = move_str[1] - '1';

        // Check if square is empty
        if (get_square(row, col) != EMPTY_SQUARE) {
            cout << MSG_ERR_SQUARE_OCCUPIED << endl;
            continue;
        }

        cout << DEBUG << current_player << " placed disc at " << move_str << endl;
        break;
    }

    return make_pair(row, col);
}


int Board::start_game() {
    print_board();
    check_collision();
    parse_move();
    print_board();
    return OK;
}

Board::~Board() {
    cout << DEBUG << MSG_DESTRUCT_BOARD << endl;
}

