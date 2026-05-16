#include "eval/mask_eval.h"

int MaskEvaluator::evaluate(const Board& board, Player player) {
    int current_player_value = calculate_weighted_mask_value(board, player);
    int opponent_value = calculate_weighted_mask_value(board, opponent(player));
    return current_player_value - opponent_value;
}

int MaskEvaluator::calculate_weighted_mask_value(const Board& board, Player player) {
    int value = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board.get_disc(Position(i, j)) == player) {
                value += m_weight_table[i][j];
            }
        }
    }
    return value;
}