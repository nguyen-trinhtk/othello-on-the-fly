#pragma once
#include "board.h"
#include "types.h"
#include "move.h"
#include "eval.h"
#include "rules.h"
#include <optional>

class AIEngine {
public:
    AIEngine() = default;
    explicit AIEngine(int search_depth) : m_search_depth(search_depth) {}
    ~AIEngine() = default;

    std::optional<Move> best_move(const Board& board, Player player);

private:
    int m_search_depth = 5;
    int negamax(const Board& board, int depth, int alpha, int beta, Player player) const;
    /*
    when implementing:
        Board copy = board.clone();
        copy.apply_move(player, move);
        int score = negamax(copy, depth - 1, -beta, -alpha, opponent(player));
    */
};
