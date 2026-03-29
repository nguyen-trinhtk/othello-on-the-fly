#ifndef ALPHABETA_HPP
#define ALPHABETA_HPP

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "board/board.hpp"

namespace othello
{
    namespace engine
    {
        struct SearchOptions
        {
            int max_depth = ALPHABETA_DEPTH;
            std::optional<std::chrono::milliseconds> time_limit{};
            std::optional<std::uint64_t> node_limit{};
            std::size_t transposition_table_size = 1u << 20;
            bool iterative_deepening = true;
            bool parallel_root = false;
            int parallel_root_min_depth = 5;
            int parallel_root_min_moves = 5;
            int parallel_root_max_workers = 0;
            int parallel_root_batch_scale = 2;
        };

        struct SearchStats
        {
            int completed_depth = 0;
            std::uint64_t nodes_searched = 0;
            std::uint64_t tt_hits = 0;
            std::uint64_t beta_cutoffs = 0;
            std::chrono::microseconds elapsed{};
        };

        struct SearchResult
        {
            std::optional<othello::board::Move> best_move;
            int score = ALPHABETA_MIN;
            SearchStats stats{};
            bool completed = true;
        };

        int alphabeta(othello::board::Board board, int depth, int alpha, int beta, Disc perspective_player);
        SearchResult find_best_move(const othello::board::Board &board, int depth);
        SearchResult find_best_move(const othello::board::Board &board, const SearchOptions &options);

    } // namespace engine
} // namespace othello

#endif
