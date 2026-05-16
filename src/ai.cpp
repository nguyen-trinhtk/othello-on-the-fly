#include "ai.h"

#include <algorithm>
#include <limits>

constexpr int NEG_INF = std::numeric_limits<int>::min();
constexpr int POS_INF = std::numeric_limits<int>::max();

std::optional<Move> AIEngine::best_move(const Board& board, Player player)
{
    m_tt.clear(); // TODO: cache TT?

    std::vector<Move> moves = Rules::get_all_valid_moves(board, player);
    if (moves.empty()) { // No valid moves
        return std::nullopt;
    }

    Move best = moves.front();
    int best_score = NEG_INF;

    // For each move
    for (const Move& move : moves) {
        Board copy = board.clone();
        // Apply and negamax
        Rules::apply_move(copy, player, move);

        int score = -negamax(copy, m_search_depth - 1,
                              NEG_INF, POS_INF,
                              opponent(player));

        // Update best move
        if (score > best_score) {
            best_score = score;
            best = move;
        }
    }

    return best; // Return best move
}

int AIEngine::negamax(const Board& board, int depth, int alpha, int beta, Player player)
{
    /* 
        alpha: Max's floor
        beta: Min's ceiling
    */
    if (depth == 0) {
        // Base case
        return Eval::score(board, player);
    }

    // Get Zobrist key
    const std::uint64_t key = zobrist_key(board, player);
    // Look up TT
    int tt_score = 0;
    if (m_tt.probe(key, depth, alpha, beta, tt_score))
        return tt_score;

    // Get all valid moves
    std::vector<Move> moves = Rules::get_all_valid_moves(board, player);

    if (moves.empty()) {
        // No valid moves for player
        if (!Rules::has_valid_move(board, opponent(player))) {
            // No valid moves for op
            const int s = Eval::score(board, player);
            m_tt.store(key, depth, s, alpha, beta);
            return s;
        }

        // Update alpha 
        const int alpha_orig = alpha; // previous alpha
        const int v = -negamax(board, depth - 1, -beta, -alpha, opponent(player));
        // Store in TT
        m_tt.store(key, depth, v, alpha_orig, beta);
        return v;
    }

    int max_eval = NEG_INF;
    const int alpha_orig = alpha;

    for (const Move& move : moves) {
        // For each move
        Board copy = board.clone();
        Rules::apply_move(copy, player, move);

        // Negamax
        int eval = -negamax(copy, depth - 1, -beta, -alpha, opponent(player));

        max_eval = std::max(max_eval, eval);
        // Update alpha
        alpha = std::max(alpha, eval);

        if (alpha >= beta) {
            break;
        }
    }
    // Store in TT
    m_tt.store(key, depth, max_eval, alpha_orig, beta);
    return max_eval; // Best score
}
