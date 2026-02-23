#ifndef CONSTANTS_H
#define CONSTANTS_H

// Board related macros
#define BOARD_SIZE 8
#define BLACK 0
#define WHITE 1
#define EMPTY -1

// - Square states
#define BLACK_DISC "●"
#define WHITE_DISC "○"
#define EMPTY_SQUARE " "
#define INVALID_SQUARE "?"

// - Directions
const int DIRECTION_X[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
const int DIRECTION_Y[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

// - Initial board state
#define INIT_BOARD_BLACK ((1ULL << (8 * 3 + 4)) | (1ULL << (8 * 4 + 3)))
#define INIT_BOARD_WHITE  ((1ULL << (8 * 3 + 3)) | (1ULL << (8 * 4 + 4)))

// Error
// Exit code
#define OK 0
#define ERR_COLLISION -1
#define ERR_NO_MOVES_LEFT -2
#define ERR_INVALID_DISC -3
#define ERR_SQUARE_OCCUPIED -4

// Message
// - Success messages
#define MSG_CREATED_BOARD "Board successfully created"
#define MSG_PRINTED_BOARD "Board printed"
#define MSG_DESTRUCT_BOARD "Cleaning board..."
#define MSG_STARTING_GAME "Starting game..."

// - Error messages
#define MSG_ERR_COLLISION "Error: collision detected, please debug!"
#define MSG_ERR_NO_MOVES_LEFT "Error: no moves left. Concluding game..."
#define MSG_ERR_CREATE_BOARD "Error: failed to create BOARD, please debug!"
#define MSG_ERR_INVALID_DISC "Error: Invalid disc value. Use 0 for white, 1 for black, -1 for empty."

// - - - Parsing error messages
#define MSG_ERR_INVALID_MOVE_FORMAT "Invalid move format, try again!"
#define MSG_ERR_OUT_OF_BOUNDS "Row or column index out of bounds, try again"
#define MSG_ERR_SQUARE_OCCUPIED "Square already occupied, try again!"
#define MSG_ERR_INVALID_MOVE "Invalid move, try again!"

// Formatting
#define DEBUG "[DEBUG] "
#define LN_BRK "\n"

#endif