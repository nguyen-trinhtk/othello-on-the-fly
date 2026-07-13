#pragma once
#include "board.h"
#include "types.h"
#include "move.h"
#include "eval/eval.h"
#include "rules.h"
#include "trans_table.h"
#include "zobrist.h"
#include "util/thread-pool.h"
#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>
#include <thread>

class AIEngine {
public:
    AIEngine();
    explicit AIEngine(int search_depth);
    AIEngine(int search_depth, std::unique_ptr<IEvaluator> evaluator);
    void set_evaluator(std::unique_ptr<IEvaluator> evaluator);
    void set_parallel(bool enabled);

    std::optional<Move> best_move(const Board& board, Player player);

    void reset_search_stats();
    std::uint64_t last_search_nodes() const;

private:
    int m_search_depth = 5;
    bool m_parallel = true;
    std::unique_ptr<IEvaluator> m_evaluator;
    ThreadPool m_pool;
    std::atomic<std::uint64_t> m_search_nodes{0};

    int negamax(const Board& board, int depth, int alpha, int beta,
                Player player, TranspositionTable& tt);
};
