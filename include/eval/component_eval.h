#pragma once
#include "eval/eval.h"
#include "board.h"
#include "types.h"

class ComponentEvaluator : public IEvaluator {
    public:
        ComponentEvaluator() = default;
        // Weighted sum
        int evaluate(const Board& board, Player player) override;

    private:
        // Component heuristics        
        int coin_parity(const Board& board, Player player);
        int mobility(const Board& board, Player player);
        int corner(const Board& board, Player player);
        int stability(const Board& board, Player player);

        // Helper
        int derive_heuristic_value(int current_player_value, int opponent_value);
        int derive_signed_heuristic_value(int current_player_value, int opponent_value);
        int count_corners(const Board& board, Player player);
        int stability_score(const Board& board, Player player);

        // Member vars
        int m_coin_parity_weight = 25;
        int m_mobility_weight = 5;
        int m_corner_weight = 30;
        int m_stability_weight = 25;

};