#ifndef ALPHABETA_INTERNAL_HPP
#define ALPHABETA_INTERNAL_HPP

#include <array>
#include <chrono>
#include <cstddef>
#include <vector>

#include "engine/alphabeta.hpp"
#include "engine/trans_table.hpp"

namespace othello
{
    namespace engine
    {
        namespace detail
        {
            using Clock = std::chrono::steady_clock;

            inline constexpr int MAX_SEARCH_PLY = BOARD_SIZE * BOARD_SIZE;

            struct SearchContext
            {
                explicit SearchContext(const SearchOptions &search_options)
                    : options(search_options),
                      transposition_table(search_options.transposition_table_size),
                      start_time(Clock::now())
                {
                }

                SearchOptions options;
                SearchStats stats{};
                TranspositionTable transposition_table;
                Clock::time_point start_time;
                bool aborted = false;
                std::array<std::array<TTMove, 2>, MAX_SEARCH_PLY> killer_moves{};
                std::array<std::array<int, BOARD_SIZE * BOARD_SIZE>, 2> history_scores{};
            };

            struct RootMoveResult
            {
                othello::board::Move move;
                int score = ALPHABETA_MIN;
                SearchStats stats{};
                bool completed = true;
                bool exact = true;
                bool failed_high = false;
                std::size_t move_order = 0;
            };

            [[nodiscard]] std::size_t parallel_root_worker_count(
                const SearchOptions &options,
                std::size_t remaining_root_moves) noexcept;

            [[nodiscard]] std::size_t parallel_root_batch_size(
                const SearchOptions &options,
                std::size_t remaining_root_moves) noexcept;

            [[nodiscard]] int search_subtree(
                othello::board::Board &board,
                int depth,
                int alpha,
                int beta,
                Disc perspective_player,
                SearchContext &context,
                int ply);

            void evaluate_parallel_root_batch(
                std::vector<RootMoveResult> &results,
                const othello::board::Board &root_board,
                const std::vector<othello::board::Move> &moves,
                Disc perspective_player,
                int depth,
                int alpha,
                int beta,
                const SearchOptions &options,
                std::size_t first_parallel_move,
                std::size_t batch_size,
                bool use_scout_window);
        } // namespace detail
    } // namespace engine
} // namespace othello

#endif
