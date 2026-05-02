#pragma once
#include "board.h"
#include "types.h"

namespace Eval {
    int score(const Board& board, Player player); // weighted sum
    
    // Individual components
    // int mobility(const Board& board, Player player);
    // int corner_control(const Board& board, Player player);
}