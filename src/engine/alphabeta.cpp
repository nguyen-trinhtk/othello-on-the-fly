#include <algorithm>
#include <chrono>

#include "engine_internal.hpp"

namespace othello
{
    namespace engine
    {
        namespace
        {
            using detail::Clock;
            using detail::MAX_SEARCH_PLY;
            using detail::RootMoveResult;
            using detail::SearchContext;

            constexpr int TT_MOVE_PRIORITY = 2'000'000;
            constexpr int CORNER_PRIORITY = 1'000'000;
            constexpr int KILLER_PRIORITY_PRIMARY = 500'000;
            constexpr int KILLER_PRIORITY_SECONDARY = 400'000;
            constexpr int EDGE_PRIORITY = 50'000;
            constexpr int X_SQUARE_PENALTY = 25'000;

            void accumulate_stats(SearchStats &dst, const SearchStats &src)
            {
                dst.nodes_searched += src.nodes_searched;
                dst.tt_hits += src.tt_hits;
                dst.beta_cutoffs += src.beta_cutoffs;
            }

            [[nodiscard]] bool root_result_is_better(
                int current_best_score,
                std::size_t current_best_move_order,
                const RootMoveResult &candidate,
                bool has_best_move) noexcept
            {
                if (!has_best_move)
                {
                    return true;
                }

                if (candidate.score != current_best_score)
                {
                    return candidate.score > current_best_score;
                }

                return candidate.move_order < current_best_move_order;
            }

            void accept_root_result(
                SearchResult &search_result,
                std::size_t &best_move_order,
                const RootMoveResult &candidate)
            {
                if (root_result_is_better(
                        search_result.score,
                        best_move_order,
                        candidate,
                        search_result.best_move.has_value()))
                {
                    search_result.best_move = candidate.move;
                    search_result.score = candidate.score;
                    best_move_order = candidate.move_order;
                }
            }

            void merge_parallel_root_result(
                SearchContext &context,
                SearchResult &search_result,
                std::size_t &best_move_order,
                const RootMoveResult &move_result)
            {
                accumulate_stats(context.stats, move_result.stats);

                if (!move_result.completed)
                {
                    context.aborted = true;
                    return;
                }

                accept_root_result(search_result, best_move_order, move_result);
            }

            [[nodiscard]] bool parallel_root_preserves_semantics(const SearchOptions &options) noexcept
            {
                return !options.time_limit.has_value() &&
                       !options.node_limit.has_value();
            }

            [[nodiscard]] int effective_parallel_root_min_depth(const SearchOptions &options) noexcept
            {
                return std::max(1, options.parallel_root_min_depth);
            }

            [[nodiscard]] std::size_t effective_parallel_root_min_moves(const SearchOptions &options) noexcept
            {
                return options.parallel_root_min_moves <= 1
                           ? 1U
                           : static_cast<std::size_t>(options.parallel_root_min_moves);
            }

            [[nodiscard]] bool can_use_parallel_root(
                const SearchOptions &options,
                int depth,
                std::size_t root_move_count) noexcept
            {
                return options.parallel_root &&
                       !options.time_limit.has_value() &&
                       !options.node_limit.has_value() &&
                       depth >= effective_parallel_root_min_depth(options) &&
                       root_move_count >= effective_parallel_root_min_moves(options);
            }

            [[nodiscard]] int player_index(Disc player) noexcept
            {
                return player == WHITE ? 1 : 0;
            }

            [[nodiscard]] int square_index(const othello::board::Move &move) noexcept
            {
                return move.row * BOARD_SIZE + move.col;
            }

            [[nodiscard]] TTMove encode_move(const othello::board::Move &move) noexcept
            {
                return {
                    static_cast<std::int8_t>(move.row),
                    static_cast<std::int8_t>(move.col),
                };
            }

            [[nodiscard]] bool same_move(
                const othello::board::Move &move,
                const TTMove &stored_move) noexcept
            {
                return stored_move.is_valid() &&
                       move.row == static_cast<int>(stored_move.row) &&
                       move.col == static_cast<int>(stored_move.col);
            }

            [[nodiscard]] bool is_corner(const othello::board::Move &move) noexcept
            {
                return (move.row == 0 || move.row == BOARD_SIZE - 1) &&
                       (move.col == 0 || move.col == BOARD_SIZE - 1);
            }

            [[nodiscard]] bool is_edge(const othello::board::Move &move) noexcept
            {
                return move.row == 0 ||
                       move.row == BOARD_SIZE - 1 ||
                       move.col == 0 ||
                       move.col == BOARD_SIZE - 1;
            }

            [[nodiscard]] bool is_x_square(const othello::board::Move &move) noexcept
            {
                return (move.row == 1 || move.row == BOARD_SIZE - 2) &&
                       (move.col == 1 || move.col == BOARD_SIZE - 2);
            }

            void update_elapsed(SearchContext &context)
            {
                context.stats.elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                    Clock::now() - context.start_time);
            }

            [[nodiscard]] bool budget_exhausted(SearchContext &context)
            {
                if (context.aborted)
                {
                    return true;
                }

                if (context.options.node_limit.has_value() &&
                    context.stats.nodes_searched >= *context.options.node_limit)
                {
                    context.aborted = true;
                    update_elapsed(context);
                    return true;
                }

                if (context.options.time_limit.has_value() &&
                    (context.stats.nodes_searched == 0 ||
                     (context.stats.nodes_searched & 1023ULL) == 0ULL) &&
                    Clock::now() - context.start_time >= *context.options.time_limit)
                {
                    context.aborted = true;
                    update_elapsed(context);
                    return true;
                }

                return false;
            }

            void record_cutoff(
                SearchContext &context,
                int ply,
                Disc player,
                const othello::board::Move &move,
                int depth)
            {
                ++context.stats.beta_cutoffs;
                context.history_scores[player_index(player)][square_index(move)] += depth * depth;

                if (ply >= MAX_SEARCH_PLY)
                {
                    return;
                }

                const TTMove encoded_move = encode_move(move);
                if (context.killer_moves[ply][0].is_valid() &&
                    context.killer_moves[ply][0].row == encoded_move.row &&
                    context.killer_moves[ply][0].col == encoded_move.col)
                {
                    return;
                }

                context.killer_moves[ply][1] = context.killer_moves[ply][0];
                context.killer_moves[ply][0] = encoded_move;
            }

            [[nodiscard]] int move_score(
                const othello::board::Move &move,
                Disc player,
                int ply,
                const SearchContext &context,
                const TTEntry *transposition_entry)
            {
                int score = static_cast<int>(move.flipped_discs.size());

                if (transposition_entry != nullptr &&
                    transposition_entry->has_best_move() &&
                    same_move(move, transposition_entry->best_move))
                {
                    score += TT_MOVE_PRIORITY;
                }

                if (is_corner(move))
                {
                    score += CORNER_PRIORITY;
                }
                else
                {
                    if (ply < MAX_SEARCH_PLY)
                    {
                        if (same_move(move, context.killer_moves[ply][0]))
                        {
                            score += KILLER_PRIORITY_PRIMARY;
                        }
                        else if (same_move(move, context.killer_moves[ply][1]))
                        {
                            score += KILLER_PRIORITY_SECONDARY;
                        }
                    }

                    if (is_edge(move))
                    {
                        score += EDGE_PRIORITY;
                    }
                    if (is_x_square(move))
                    {
                        score -= X_SQUARE_PENALTY;
                    }
                }

                score += context.history_scores[player_index(player)][square_index(move)];
                return score;
            }

            void order_moves(
                std::vector<othello::board::Move> &moves,
                Disc player,
                int ply,
                const SearchContext &context,
                const TTEntry *transposition_entry)
            {
                std::stable_sort(
                    moves.begin(),
                    moves.end(),
                    [&context, player, ply, transposition_entry](
                        const othello::board::Move &lhs,
                        const othello::board::Move &rhs)
                    {
                        return move_score(lhs, player, ply, context, transposition_entry) >
                               move_score(rhs, player, ply, context, transposition_entry);
                    });
            }

            int search_subtree_impl(
                othello::board::Board &board,
                int depth,
                int alpha,
                int beta,
                Disc perspective_player,
                SearchContext &context,
                int ply)
            {
                if (budget_exhausted(context))
                {
                    return board.evaluate(perspective_player);
                }

                ++context.stats.nodes_searched;
                if (depth == 0)
                {
                    return board.evaluate(perspective_player);
                }

                const int original_alpha = alpha;
                const int original_beta = beta;
                const std::uint64_t hash = board.get_hash();

                TTEntry entry{};
                const bool has_tt_entry = context.transposition_table.lookup(hash, entry);
                if (has_tt_entry)
                {
                    ++context.stats.tt_hits;
                }

                if (has_tt_entry && entry.depth >= depth)
                {
                    switch (entry.type)
                    {
                    case TTEntryType::EXACT:
                        return entry.value;
                    case TTEntryType::LOWERBOUND:
                        alpha = std::max(alpha, entry.value);
                        break;
                    case TTEntryType::UPPERBOUND:
                        beta = std::min(beta, entry.value);
                        break;
                    }

                    if (alpha >= beta)
                    {
                        return entry.value;
                    }
                }

                const Disc current_player = board.get_current_player();
                auto moves = board.get_moves_for_current_state();
                if (moves.empty())
                {
                    if (!board.has_valid_moves(opponent(current_player)))
                    {
                        return board.evaluate(perspective_player);
                    }

                    board.set_current_player(opponent(current_player));
                    const int pass_eval = search_subtree_impl(
                        board,
                        depth,
                        alpha,
                        beta,
                        perspective_player,
                        context,
                        ply + 1);
                    board.set_current_player(current_player);

                    if (!context.aborted)
                    {
                        context.transposition_table.store(hash, pass_eval, depth, TTEntryType::EXACT);
                    }
                    return pass_eval;
                }

                order_moves(
                    moves,
                    current_player,
                    ply,
                    context,
                    has_tt_entry ? &entry : nullptr);

                const bool maximizing = current_player == perspective_player;
                int best_eval = maximizing ? ALPHABETA_MIN : ALPHABETA_MAX;
                const othello::board::Move *best_move = nullptr;

                for (const auto &candidate : moves)
                {
                    if (budget_exhausted(context))
                    {
                        break;
                    }

                    if (board.process_move(candidate, current_player) != OK)
                    {
                        continue;
                    }
                    board.set_current_player(opponent(current_player));

                    const int eval = search_subtree_impl(
                        board,
                        depth - 1,
                        alpha,
                        beta,
                        perspective_player,
                        context,
                        ply + 1);

                    board.set_current_player(current_player);
                    if (board.undo_move(candidate, current_player) != OK)
                    {
                        context.aborted = true;
                        update_elapsed(context);
                        return best_move != nullptr ? best_eval : board.evaluate(perspective_player);
                    }

                    if (context.aborted)
                    {
                        break;
                    }

                    if (maximizing)
                    {
                        if (best_move == nullptr || eval > best_eval)
                        {
                            best_eval = eval;
                            best_move = &candidate;
                        }
                        alpha = std::max(alpha, eval);
                    }
                    else
                    {
                        if (best_move == nullptr || eval < best_eval)
                        {
                            best_eval = eval;
                            best_move = &candidate;
                        }
                        beta = std::min(beta, eval);
                    }

                    if (beta <= alpha)
                    {
                        record_cutoff(context, ply, current_player, candidate, depth);
                        break;
                    }
                }

                if (context.aborted)
                {
                    return best_move != nullptr ? best_eval : board.evaluate(perspective_player);
                }

                TTEntryType entry_type = TTEntryType::EXACT;
                if (best_eval <= original_alpha)
                {
                    entry_type = TTEntryType::UPPERBOUND;
                }
                else if (best_eval >= original_beta)
                {
                    entry_type = TTEntryType::LOWERBOUND;
                }

                if (best_move != nullptr)
                {
                    context.transposition_table.store(hash, best_eval, depth, entry_type, best_move);
                    return best_eval;
                }

                return board.evaluate(perspective_player);
            }

            SearchResult search_at_depth(
                othello::board::Board &board,
                int depth,
                SearchContext &context)
            {
                SearchResult result{};
                const Disc perspective_player = board.get_current_player();
                result.score = board.evaluate(perspective_player);

                if (budget_exhausted(context))
                {
                    result.completed = false;
                    result.stats = context.stats;
                    return result;
                }

                auto moves = board.get_moves_for_current_state();
                if (moves.empty())
                {
                    result.score = search_subtree_impl(
                        board,
                        depth,
                        ALPHABETA_MIN,
                        ALPHABETA_MAX,
                        perspective_player,
                        context,
                        0);
                    result.completed = !context.aborted;
                    result.stats = context.stats;
                    return result;
                }

                TTEntry entry{};
                const bool has_tt_entry = context.transposition_table.lookup(board.get_hash(), entry);
                if (has_tt_entry)
                {
                    ++context.stats.tt_hits;
                }

                order_moves(
                    moves,
                    perspective_player,
                    0,
                    context,
                    has_tt_entry ? &entry : nullptr);
                const bool use_parallel_root = can_use_parallel_root(context.options, depth, moves.size());

                const int beta = ALPHABETA_MAX;
                std::size_t best_move_order = moves.size();

                if (use_parallel_root)
                {
                    int alpha = ALPHABETA_MIN;
                    std::size_t first_parallel_move = 0;

                    if (!moves.empty())
                    {
                        const auto &candidate = moves[0];
                        if (board.process_move(candidate, perspective_player) == OK)
                        {
                            board.set_current_player(opponent(perspective_player));

                            const int eval = search_subtree_impl(
                                board,
                                std::max(0, depth - 1),
                                alpha,
                                beta,
                                perspective_player,
                                context,
                                1);

                            board.set_current_player(perspective_player);
                            if (board.undo_move(candidate, perspective_player) != OK)
                            {
                                context.aborted = true;
                                update_elapsed(context);
                            }
                            else if (!context.aborted)
                            {
                                RootMoveResult move_result{};
                                move_result.move = candidate;
                                move_result.score = eval;
                                move_result.completed = true;
                                move_result.move_order = 0;

                                accept_root_result(result, best_move_order, move_result);

                                alpha = std::max(alpha, move_result.score);
                                first_parallel_move = 1;
                            }
                        }
                    }

                    if (!context.aborted)
                    {
                        std::size_t batch_first_move = first_parallel_move;

                        while (!context.aborted && batch_first_move < moves.size())
                        {
                            const std::size_t remaining_root_moves = moves.size() - batch_first_move;
                            const std::size_t batch_size =
                                detail::parallel_root_batch_size(context.options, remaining_root_moves);
                            if (batch_size == 0)
                            {
                                break;
                            }

                            std::vector<RootMoveResult> parallel_results(batch_size);

                            detail::evaluate_parallel_root_batch(
                                parallel_results,
                                board,
                                moves,
                                perspective_player,
                                depth,
                                alpha,
                                beta,
                                context.options,
                                batch_first_move,
                                batch_size);

                            for (const RootMoveResult &move_result : parallel_results)
                            {
                                merge_parallel_root_result(context, result, best_move_order, move_result);
                                if (context.aborted)
                                {
                                    break;
                                }

                                alpha = std::max(alpha, move_result.score);
                            }

                            batch_first_move += batch_size;
                        }
                    }
                }
                else
                {
                    int alpha = ALPHABETA_MIN;

                    for (std::size_t move_order = 0; move_order < moves.size(); ++move_order)
                    {
                        if (budget_exhausted(context))
                        {
                            break;
                        }

                        const auto &candidate = moves[move_order];
                        if (board.process_move(candidate, perspective_player) != OK)
                        {
                            continue;
                        }
                        board.set_current_player(opponent(perspective_player));

                        const int eval = search_subtree_impl(
                            board,
                            std::max(0, depth - 1),
                            alpha,
                            beta,
                            perspective_player,
                            context,
                            1);

                        board.set_current_player(perspective_player);
                        if (board.undo_move(candidate, perspective_player) != OK)
                        {
                            context.aborted = true;
                            update_elapsed(context);
                            break;
                        }

                        if (context.aborted)
                        {
                            break;
                        }

                        RootMoveResult move_result{};
                        move_result.move = candidate;
                        move_result.score = eval;
                        move_result.completed = true;
                        move_result.move_order = move_order;

                        accept_root_result(result, best_move_order, move_result);

                        alpha = std::max(alpha, move_result.score);
                    }
                }

                if (!context.aborted && result.best_move.has_value())
                {
                    context.transposition_table.store(
                        board.get_hash(),
                        result.score,
                        depth,
                        TTEntryType::EXACT,
                        &(*result.best_move));
                }

                result.completed = !context.aborted;
                result.stats = context.stats;
                return result;
            }

            SearchResult run_search(const othello::board::Board &board, const SearchOptions &options)
            {
                othello::board::Board working_board = board;
                SearchContext context(options);

                SearchResult best_result{};
                best_result.score = working_board.evaluate(working_board.get_current_player());
                best_result.completed = false;

                if (options.max_depth <= 0)
                {
                    update_elapsed(context);
                    best_result.completed = true;
                    best_result.stats = context.stats;
                    return best_result;
                }

                const int start_depth = options.iterative_deepening ? 1 : options.max_depth;
                for (int depth = start_depth; depth <= options.max_depth; ++depth)
                {
                    context.transposition_table.new_search();
                    SearchResult current_result = search_at_depth(working_board, depth, context);
                    update_elapsed(context);

                    if (current_result.completed)
                    {
                        context.stats.completed_depth = depth;
                        best_result = current_result;
                    }
                    else
                    {
                        break;
                    }

                    if (!options.iterative_deepening)
                    {
                        break;
                    }
                }

                update_elapsed(context);
                best_result.stats = context.stats;
                best_result.completed = context.stats.completed_depth >= options.max_depth;
                return best_result;
            }

        } // namespace

        int detail::search_subtree(
            othello::board::Board &board,
            int depth,
            int alpha,
            int beta,
            Disc perspective_player,
            detail::SearchContext &context,
            int ply)
        {
            return search_subtree_impl(
                board,
                depth,
                alpha,
                beta,
                perspective_player,
                context,
                ply);
        }

        int alphabeta(othello::board::Board board, int depth, int alpha, int beta, Disc perspective_player)
        {
            SearchOptions options{};
            options.max_depth = depth;
            options.iterative_deepening = false;

            detail::SearchContext context(options);
            context.transposition_table.new_search();
            return detail::search_subtree(
                board,
                depth,
                alpha,
                beta,
                perspective_player,
                context,
                0);
        }

        SearchResult find_best_move(const othello::board::Board &board, int depth)
        {
            SearchOptions options{};
            options.max_depth = depth;
            return find_best_move(board, options);
        }

        SearchResult find_best_move(const othello::board::Board &board, const SearchOptions &options)
        {
            return run_search(board, options);
        }

    } // namespace engine
} // namespace othello
