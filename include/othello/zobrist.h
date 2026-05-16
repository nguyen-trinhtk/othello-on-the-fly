#pragma once

#include "board.h"
#include "types.h"
#include <cstdint>

// Zobrist key
uint64_t zobrist_key(const Board &board, Player side_to_move);
