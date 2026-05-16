#pragma once
#include "board.h"
#include "types.h"
#include "move.h"
#include "eval.h"
#include "rules.h"
#include "trans_table.h"
#include "zobrist.h"
#include <optional>

class AIEngine {
public:
    AIEngine() = default;
    explicit AIEngine(int search_depth) : m_search_depth(search_depth) {}
    ~AIEngine() = default;

    // Return current best move
    std::optional<Move> best_move(const Board& board, Player player);

private:
    int m_search_depth = 5;
    TranspositionTable m_tt;
    int negamax(const Board& board, int depth, int alpha, int beta, Player player);
};
