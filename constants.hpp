#ifndef CONSTANTS_H
#define CONSTANTS_H

// Board related macros
#define BOARD_SIZE 8
#define BLACK_DISC "●"
#define WHITE_DISC "○"
#define EMPTY_SQUARE " "

#define BLACK 1
#define WHITE 0

// Error
// Exit code
#define OK 0
#define ERR_NO_MVS_LFT -1

// Message
// - Success messages
#define MSG_CRT_BRD "Board successfully created"
#define MSG_PRNT_BRD "Board printing"
#define MSG_DSTRCT_BRD "Cleaning board..."

// - Error messages
#define MSG_NO_MVS_LFT "Error: no moves left. Concluding game..."

// Formatting
#define DEBUG "[DEBUG] "
#define LN_BRK "\n"

#endif