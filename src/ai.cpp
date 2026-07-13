#include "ai.h"

#include "eval/mask_eval.h"

#include <algorithm>
#include <limits>
#include <thread>
#include <vector>

constexpr int NEG_INF = std::numeric_limits<int>::min();
constexpr int POS_INF = std::numeric_limits<int>::max();

static unsigned pool_size()
{
    return std::max(1u, std::thread::hardware_concurrency());
}

// Constructors
AIEngine::AIEngine()
    : m_search_depth(5),
      m_evaluator(std::make_unique<MaskEvaluator>()),
      m_pool(pool_size())
{
}

AIEngine::AIEngine(int search_depth)
    : m_search_depth(search_depth),
      m_evaluator(std::make_unique<MaskEvaluator>()),
      m_pool(pool_size())
{
}

AIEngine::AIEngine(int search_depth, std::unique_ptr<IEvaluator> evaluator)
    : m_search_depth(search_depth),
      m_evaluator(evaluator ? std::move(evaluator) : std::make_unique<MaskEvaluator>()),
      m_pool(pool_size())
{
}

void AIEngine::set_evaluator(std::unique_ptr<IEvaluator> evaluator)
{
    m_evaluator = evaluator ? std::move(evaluator) : std::make_unique<MaskEvaluator>();
}

void AIEngine::set_parallel(bool enabled)
{
    m_parallel = enabled;
}

void AIEngine::reset_search_stats()
{
    m_search_nodes = 0;
}

std::uint64_t AIEngine::last_search_nodes() const
{
    return m_search_nodes.load();
}

// Main entry
std::optional<Move> AIEngine::best_move(const Board& board, Player player)
{
    reset_search_stats();
    std::vector<Move> moves = Rules::get_all_valid_moves(board, player);
    if (moves.empty())
        return std::nullopt;

    if (!m_parallel) {
        TranspositionTable tt;
        Move best = moves.front();
        int best_score = NEG_INF;

        for (const Move& move : moves) {
            Board copy = board.clone();
            Rules::apply_move(copy, player, move);
            int score = -negamax(copy, m_search_depth - 1,
                                 NEG_INF, POS_INF, opponent(player), tt);
            if (score > best_score) {
                best_score = score;
                best = move;
            }
        }
        return best;
    }

    const std::size_t n = moves.size();
    std::vector<int> scores(n, NEG_INF);

    for (std::size_t i = 0; i < n; ++i) {
        m_pool.submit([&, i] {
            TranspositionTable local_tt;
            Board copy = board.clone();
            Rules::apply_move(copy, player, moves[i]);
            scores[i] = -negamax(copy, m_search_depth - 1,
                                 NEG_INF, POS_INF, opponent(player), local_tt);
        });
    }

    m_pool.wait();

    auto it = std::max_element(scores.begin(), scores.end());
    return moves[static_cast<std::size_t>(std::distance(scores.begin(), it))];
}

int AIEngine::negamax(const Board& board, int depth, int alpha, int beta,
                      Player player, TranspositionTable& tt)
{
    ++m_search_nodes;

    if (depth == 0)
        return m_evaluator->evaluate(board, player);

    const std::uint64_t key = zobrist_key(board, player);
    int tt_score = 0;
    if (tt.probe(key, depth, alpha, beta, tt_score))
        return tt_score;

    std::vector<Move> moves = Rules::get_all_valid_moves(board, player);

    if (moves.empty()) {
        if (!Rules::has_valid_move(board, opponent(player))) {
            const int s = m_evaluator->evaluate(board, player);
            tt.store(key, depth, s, alpha, beta);
            return s;
        }

        const int alpha_orig = alpha;
        const int v = -negamax(board, depth - 1, -beta, -alpha, opponent(player), tt);
        tt.store(key, depth, v, alpha_orig, beta);
        return v;
    }

    int max_eval = NEG_INF;
    const int alpha_orig = alpha;

    for (const Move& move : moves) {
        Board copy = board.clone();
        Rules::apply_move(copy, player, move);

        int eval = -negamax(copy, depth - 1, -beta, -alpha, opponent(player), tt);

        max_eval = std::max(max_eval, eval);
        alpha = std::max(alpha, eval);

        if (alpha >= beta)
            break;
    }

    tt.store(key, depth, max_eval, alpha_orig, beta);
    return max_eval;
}
