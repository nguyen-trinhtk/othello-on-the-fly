#pragma once
#include "eval/eval.h"
#include "board.h"
#include "types.h"

class MaskEvaluator : public IEvaluator {
    public:
        int evaluate(const Board& board, Player player) override;
    private:
        static constexpr int m_weight_table[8][8] = {
            {4, -3, 2, 2, 2, 2, -3, 4},
            {-3, -4, -1, -1, -1, -1, -4, -3},
            {2, -1, 1, 0, 0, 1, -1, 2},
            {2, -1, 0, 1, 1, 0, -1, 2},
            {2, -1, 0, 1, 1, 0, -1, 2},
            {2, -1, 1, 0, 0, 1, -1, 2},
            {-3, -4, -1, -1, -1, -1, -4, -3},
            {4, -3, 2, 2, 2, 2, -3, 4}
        };

        int calculate_weighted_mask_value(const Board& board, Player player);
};