#include "board.hpp"
#include "constants.hpp"

pair<int, int> Board::parse_move() {
    // Loop for user to enter move
    string move_str;
    string current_player = (current_turn == BLACK) ? "Black" : "White";
    int row, col;
    cout << current_player << "'s turn! " << endl;

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

        // Check if move is valid according to Othello rules
        if (!is_valid_move(row, col, current_turn)) {
            if (get_square(row, col) != EMPTY) {
                cout << MSG_ERR_SQUARE_OCCUPIED << endl;
            } else {
                cout << MSG_ERR_INVALID_MOVE << endl;
            }
            continue;
        }
        cout << current_player << " placed disc at " << move_str << endl;
        break;
    }
    return make_pair(row, col);
}

bool Board::is_valid_move(int r, int c, bool player) {
    uint64_t move_bit = 1ULL << (8 * r + c);
    if ((black_moves | white_moves) & move_bit) return false; // Not empty

    uint64_t player_bits = player == BLACK ? black_moves : white_moves;
    uint64_t opp_bits = player == BLACK ? white_moves : black_moves;

    // Directional shift amounts for N, S, E, W, NE, NW, SE, SW
    const int dir_shifts[8] = {8, -8, 1, -1, 9, 7, -7, -9};
    const uint64_t notA = 0xfefefefefefefefeULL;
    const uint64_t notH = 0x7f7f7f7f7f7f7f7fULL;

    for (int d = 0; d < 8; ++d) {
        uint64_t mask = 0;
        uint64_t cur = move_bit;
        for (int step = 0; step < 7; ++step) {
            // Edge masking for left/right directions
            if ((d == 2 || d == 4 || d == 6) && (cur & notH) == 0) break;
            if ((d == 3 || d == 5 || d == 7) && (cur & notA) == 0) break;

            // Shift in direction
            if (dir_shifts[d] > 0)
                cur <<= dir_shifts[d];
            else
                cur >>= -dir_shifts[d];

            if ((cur & opp_bits) != 0) {
                mask |= cur;
            } else {
                if ((cur & player_bits) != 0 && mask != 0)
                    return true;
                break;
            }
        }
    }
    return false;
}

int Board::get_valid_moves() {
    valid_moves.clear();
    // Scan the whole board for valid moves
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
    uint64_t move_bit = 1ULL << (8 * r + c);
    uint64_t player_bits = player == BLACK ? black_moves : white_moves;
    uint64_t opp_bits = player == BLACK ? white_moves : black_moves;

    // Directional shift amounts for N, S, E, W, NE, NW, SE, SW
    const int dir_shifts[8] = {8, -8, 1, -1, 9, 7, -7, -9};
    const uint64_t notA = 0xfefefefefefefefeULL;
    const uint64_t notH = 0x7f7f7f7f7f7f7f7fULL;

    uint64_t to_flip = 0;

    for (int d = 0; d < 8; ++d) {
        uint64_t mask = 0;
        uint64_t cur = move_bit;
        for (int step = 0; step < 7; ++step) {
            // Edge masking for left/right directions
            if ((d == 2 || d == 4 || d == 6) && (cur & notH) == 0) break;
            if ((d == 3 || d == 5 || d == 7) && (cur & notA) == 0) break;

            // Shift in direction
            if (dir_shifts[d] > 0)
                cur <<= dir_shifts[d];
            else
                cur >>= -dir_shifts[d];

            if ((cur & opp_bits) != 0) {
                mask |= cur;
            } else {
                if ((cur & player_bits) != 0 && mask != 0) {
                    to_flip |= mask;
                }
                break;
            }
        }
    }

    // Place the new disc
    if (player == BLACK) {
        black_moves |= move_bit | to_flip;
        white_moves &= ~to_flip;
    } else {
        white_moves |= move_bit | to_flip;
        black_moves &= ~to_flip;
    }
    cout << "I'm new" << endl;
    return OK;
}
