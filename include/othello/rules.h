#pragma once
#include "board.h"
#include "move.h"
#include "types.h"
#include <vector>

namespace Rules
{
    bool is_valid_move(const Board &board, Player player, const Position &pos);
    bool has_valid_move(const Board &board, Player player);
    std::vector<Move> get_all_valid_moves(const Board &board, Player player);
    OptError apply_move(Board &board, Player player, const Move &move);
}
