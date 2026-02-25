#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

// --- AI/Search Constants ---
constexpr int ALPHABETA_DEPTH = 5;
constexpr int ALPHABETA_MIN = -1000000;
constexpr int ALPHABETA_MAX = 1000000;

// --- Bitboard Directions ---
#include <cstdint>
struct Dir
{
	int shift;
	uint64_t mask;
};
constexpr uint64_t notA = 0xfefefefefefefefeULL;
constexpr uint64_t notH = 0x7f7f7f7f7f7f7f7fULL;
constexpr Dir dirs[8] = {
	{-8, 0xFFFFFFFFFFFFFFFFULL}, // N
	{8, 0xFFFFFFFFFFFFFFFFULL},	 // S
	{1, notH},					 // E
	{-1, notA},					 // W
	{-7, notH},					 // NE
	{-9, notA},					 // NW
	{9, notH},					 // SE
	{7, notA},					 // SW
};
constexpr int dir_shifts[8] = {-8, 8, 1, -1, -7, -9, 9, 7};

// --- Board Constants ---
constexpr int BOARD_SIZE = 8;
constexpr int BLACK = 0;
constexpr int WHITE = 1;
constexpr int EMPTY = -1;
constexpr char BLACK_DISC[] = "●";
constexpr char WHITE_DISC[] = "○";
constexpr char EMPTY_SQUARE[] = " ";
constexpr char INVALID_SQUARE[] = "?";

// --- Directions ---
constexpr int DIRECTION_X[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
constexpr int DIRECTION_Y[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

// --- Initial Board State ---
constexpr uint64_t INIT_BOARD_BLACK = ((1ULL << (8 * 3 + 4)) | (1ULL << (8 * 4 + 3)));
constexpr uint64_t INIT_BOARD_WHITE = ((1ULL << (8 * 3 + 3)) | (1ULL << (8 * 4 + 4)));

// --- Error Codes ---
constexpr int OK = 0;
constexpr int ERR_COLLISION = -1;
constexpr int ERR_NO_MOVES_LEFT = -2;
constexpr int ERR_INVALID_DISC = -3;
constexpr int ERR_SQUARE_OCCUPIED = -4;

// --- Messages ---
constexpr char MSG_CREATED_BOARD[] = "Board successfully created";
constexpr char MSG_PRINTED_BOARD[] = "Board printed";
constexpr char MSG_DESTRUCT_BOARD[] = "Cleaning board...";
constexpr char MSG_STARTING_GAME[] = "Starting game...";
constexpr char MSG_ERR_COLLISION[] = "Error: collision detected, please debug!";
constexpr char MSG_ERR_NO_MOVES_LEFT[] = "Error: no moves left. Concluding game...";
constexpr char MSG_ERR_CREATE_BOARD[] = "Error: failed to create BOARD, please debug!";
constexpr char MSG_ERR_INVALID_DISC[] = "Error: Invalid disc value. Use 0 for white, 1 for black, -1 for empty.";
constexpr char MSG_ERR_INVALID_MOVE_FORMAT[] = "Invalid move format, try again!";
constexpr char MSG_ERR_OUT_OF_BOUNDS[] = "Row or column index out of bounds, try again";
constexpr char MSG_ERR_SQUARE_OCCUPIED[] = "Square already occupied, try again!";
constexpr char MSG_ERR_INVALID_MOVE[] = "Invalid move, try again!";

// --- Formatting ---
constexpr char DEBUG[] = "[DEBUG] ";
constexpr char LN_BRK[] = "\n";

#endif