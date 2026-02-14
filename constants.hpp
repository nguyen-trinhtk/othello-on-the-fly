#ifndef CONSTANTS_H
#define CONSTANTS_H

// Board related macros
#define BOARD_SIZE 8
#define BLACK 1
#define WHITE 0

// - Square states
#define BLACK_DISC "●"
#define WHITE_DISC "○"
#define EMPTY_SQUARE " "

// - Initial board state
#define INIT_BOARD_BLACK ((1ULL << (8 * 3 + 4)) | (1ULL << (8 * 4 + 3)))
#define INIT_BOARD_WHITE  ((1ULL << (8 * 3 + 3)) | (1ULL << (8 * 4 + 4)))

// Error
// Exit code
#define OK 0
#define ERR_COLLISION -1
#define ERR_NO_MOVES_LEFT -2

// Message
// - Success messages
#define MSG_CREATED_BOARD "Board successfully created"
#define MSG_PRINTED_BOARD "Board printed"
#define MSG_DESTRUCT_BOARD "Cleaning board..."

// - Error messages
#define MSG_ERR_COLLISION "Error: collision detected, please debug!"
#define MSG_ERR_NO_MOVES_LEFT "Error: no moves left. Concluding game..."
#define MSG_ERR_CREATE_BOARD "Error: failed to create BOARD, please debug!"

// Formatting
#define DEBUG "[DEBUG] "
#define LN_BRK "\n"

#endif