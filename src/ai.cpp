#include "ai.h"

#include <algorithm>
#include <limits>

constexpr int NEG_INF = std::numeric_limits<int>::min();
constexpr int POS_INF = std::numeric_limits<int>::max();

Move AIEngine::best_move(const Board& board, Player player)
{
    // TODO: wire with GameState
    std::vector<Move> moves = Rules::get_all_valid_moves(board, player);
    if (moves.empty())
        return Move{Position{-1, -1}};

    Move best = moves.front();
    int best_score = NEG_INF;

    for (const Move& move : moves) {
        Board copy = board.clone();
        Rules::apply_move(copy, player, move);

        int score = -negamax(copy, m_search_depth - 1,
                              NEG_INF, POS_INF,
                              opponent(player));

        if (score > best_score) {
            best_score = score;
            best = move;
        }
    }

    return best;
}

int AIEngine::negamax(const Board& board, int depth, int alpha, int beta, Player player) const
{
    if (depth == 0) {
        return Eval::score(board, player);
    }

    std::vector<Move> moves = Rules::get_all_valid_moves(board, player);

    if (moves.empty()) {
        std::vector<Move> opp_moves = Rules::get_all_valid_moves(board, opponent(player));

        if (opp_moves.empty()) {
            return Eval::score(board, player);
        }

        return -negamax(board, depth - 1, -beta, -alpha, opponent(player));
    }

    int max_eval = NEG_INF;

    for (const Move& move : moves) {
        Board copy = board.clone();
        Rules::apply_move(copy, player, move);

        int eval = -negamax(copy, depth - 1, -beta, -alpha, opponent(player));

        max_eval = std::max(max_eval, eval);
        alpha = std::max(alpha, eval);

        if (alpha >= beta) {
            break;
        }
    }
    return max_eval;
}