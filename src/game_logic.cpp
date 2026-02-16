#include "board.hpp"
#include "constants.hpp"

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
        if (get_square(row, col) != EMPTY) {
            cout << MSG_ERR_SQUARE_OCCUPIED << endl;
            continue;
        }

        cout << DEBUG << current_player << " placed disc at " << move_str << endl;
        break;
    }

    return make_pair(row, col);
}

bool Board::is_valid_move(int r, int c, bool player) {

    if (get_square(r, c) != EMPTY) {
        return false;
    }

    for (int i = 0; i < 8; ++i) {
        int dr = DIRECTION_X[i];
        int dc = DIRECTION_Y[i];

        // Neighbor in direction (dr, dc)
        int nr = r + dr;
        int nc = c + dc;

        // Check if neighbor is opponent's disc
        if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE && get_square(nr, nc) == !player) {
            // Traverse in the same direction
            while (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                if (get_square(nr, nc) == EMPTY) {
                    break; // No disc to flip in this direction
                }
                if (get_square(nr, nc) == player) {
                    return true; // Valid move found
                }
                nr += dr;
                nc += dc;
            }
        }
    }
    return false;
}

int Board::get_valid_moves() {
    valid_moves.clear();

    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (is_valid_move(r, c, current_turn)) {
                valid_moves.emplace_back(r, c);
            }
        }
    }

    return valid_moves.size();
}

int Board::process_move(int r, int c, bool player) {
    set_square(r, c, player);

    for (int i = 0; i < 8; ++i) {
        int dr = DIRECTION_X[i];
        int dc = DIRECTION_Y[i];

        // Neighbor in direction (dr, dc)
        int nr = r + dr;
        int nc = c + dc;

        // Check if neighbor is opponent's disc
        if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE && get_square(nr, nc) == !player) {
            // Traverse in the same direction
            while (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                if (get_square(nr, nc) == EMPTY) {
                    break; // No disc to flip in this direction
                }
                if (get_square(nr, nc) == player) {
                    // Flip discs in this direction
                    int flip_r = r + dr;
                    int flip_c = c + dc;
                    while (flip_r != nr || flip_c != nc) {
                        set_square(flip_r, flip_c, player);
                        flip_r += dr;
                        flip_c += dc;
                    }
                    break;
                }
                nr += dr;
                nc += dc;
            }
        }
    }

    return OK;
}
