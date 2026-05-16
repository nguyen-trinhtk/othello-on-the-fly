#pragma once
#include "eval/eval.h"
#include "board.h"
#include "types.h"
#include <bit>

class CountEvaluator : public IEvaluator {
    public:
        int evaluate(const Board& board, Player player) override;
};

int CountEvaluator::evaluate(const Board& board, Player player) {
    // Pure count difference
    // Lightweight drop in
    const int my_discs = std::popcount(board.discs(player));
    const int opponent_discs = std::popcount(board.discs(opponent(player)));
    return my_discs - opponent_discs;
}