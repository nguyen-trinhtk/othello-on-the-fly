#pragma once

#include "board.h"
#include "types.h"
#include <optional>

struct GameOutcome
{
    int black_count{};
    int white_count{};
    std::optional<Player> winner; // tie will be nullopt
};

GameOutcome outcome_from_board(const Board &board);
