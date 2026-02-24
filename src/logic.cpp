#include "board.hpp"

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
    return valid_moves.find(std::make_pair(r, c)) != valid_moves.end();
}

int Board::get_valid_moves() {
    valid_moves.clear();
    uint64_t player_bits = current_turn == BLACK ? black_moves : white_moves;
    uint64_t opp_bits = current_turn == BLACK ? white_moves : black_moves;
    uint64_t empty = ~(black_moves | white_moves);

    uint64_t moves = 0;

    for (int d = 0; d < 8; ++d) {
        uint64_t mask = 0;
        uint64_t candidates = player_bits;
        for (int step = 0; step < 6; ++step) {
            if (dirs[d].shift > 0)
                candidates = (candidates & dirs[d].mask) << dirs[d].shift;
            else
                candidates = (candidates & dirs[d].mask) >> -dirs[d].shift;
            mask |= candidates & opp_bits;
            candidates &= opp_bits;
            if (!candidates) break;
        }
        // Final shift
        if (dirs[d].shift > 0)
            mask = (mask & dirs[d].mask) << dirs[d].shift;
        else
            mask = (mask & dirs[d].mask) >> -dirs[d].shift;
        moves |= mask & empty;
    }

    // Fill valid_moves set from bitboard
    for (int i = 0; i < 64; ++i) {
        if ((moves >> i) & 1) {
            int r = i / 8;
            int c = i % 8;
            valid_moves.insert(std::make_pair(r, c));
        }
    }
    // Debug print all valid moves
    cout << "Valid moves: ";
    for (const auto& mv : valid_moves) {
        cout << char('A' + mv.second) << (mv.first + 1) << " ";
    }
    cout << endl;
    return valid_moves.size();
}

int Board::process_move(int r, int c, bool player) {
    uint64_t move_bit = 1ULL << (8 * r + c);
    uint64_t player_bits = player == BLACK ? black_moves : white_moves;
    uint64_t opp_bits = player == BLACK ? white_moves : black_moves;

    uint64_t to_flip = 0;

    for (int d = 0; d < 8; ++d) {
        uint64_t mask = 0;
        uint64_t cur = move_bit;
        for (int step = 0; step < 7; ++step) {
            // Edge masking and shifting
            if (dirs[d].shift > 0)
                cur = (cur & dirs[d].mask) << dirs[d].shift;
            else
                cur = (cur & dirs[d].mask) >> -dirs[d].shift;

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
