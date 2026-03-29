#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <functional>
#include <thread>
#include <utility>

#include "engine/alphabeta.hpp"
#include "engine/trans_table.hpp"

namespace othello
{
    namespace engine
    {
        namespace
        {
            using Clock = std::chrono::steady_clock;

            constexpr int MAX_SEARCH_PLY = BOARD_SIZE * BOARD_SIZE;
            constexpr int TT_MOVE_PRIORITY = 2'000'000;
            constexpr int CORNER_PRIORITY = 1'000'000;
            constexpr int KILLER_PRIORITY_PRIMARY = 500'000;
            constexpr int KILLER_PRIORITY_SECONDARY = 400'000;
            constexpr int EDGE_PRIORITY = 50'000;
            constexpr int X_SQUARE_PENALTY = 25'000;
            constexpr int MIN_PARALLEL_ROOT_DEPTH = 5;
            constexpr std::size_t MIN_PARALLEL_ROOT_MOVES = 5;

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
                std::size_t move_order = 0;
            };

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

            [[nodiscard]] bool can_use_parallel_root(
                const SearchOptions &options,
                int depth,
                std::size_t root_move_count) noexcept
            {
                return options.parallel_root &&
                       !options.time_limit.has_value() &&
                       !options.node_limit.has_value() &&
                       depth >= MIN_PARALLEL_ROOT_DEPTH &&
                       root_move_count >= MIN_PARALLEL_ROOT_MOVES;
            }

            [[nodiscard]] std::size_t parallel_root_worker_count(std::size_t remaining_root_moves) noexcept
            {
                if (remaining_root_moves == 0)
                {
                    return 0;
                }

                const unsigned int hardware_threads = std::thread::hardware_concurrency();
                const std::size_t hardware_limit =
                    hardware_threads == 0 ? 1U : static_cast<std::size_t>(hardware_threads);

                return std::min(remaining_root_moves, hardware_limit);
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

            int alphabeta_impl(
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
                    const int pass_eval = alphabeta_impl(
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

                    const int eval = alphabeta_impl(
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

            [[nodiscard]] RootMoveResult evaluate_root_move_isolated(
                const othello::board::Board &root_board,
                const othello::board::Move &candidate,
                Disc perspective_player,
                int depth,
                int alpha,
                int beta,
                const SearchOptions &options,
                std::size_t move_order)
            {
                othello::board::Board board = root_board;
                SearchContext worker_context(options);
                worker_context.transposition_table.new_search();

                RootMoveResult result{};
                result.move = candidate;
                result.move_order = move_order;

                if (board.process_move(candidate, perspective_player) != OK)
                {
                    result.score = board.evaluate(perspective_player);
                    result.completed = false;
                    return result;
                }

                board.set_current_player(opponent(perspective_player));
                result.score = alphabeta_impl(
                    board,
                    std::max(0, depth - 1),
                    alpha,
                    beta,
                    perspective_player,
                    worker_context,
                    1);

                result.stats = worker_context.stats;
                result.completed = !worker_context.aborted;
                return result;
            }

            void run_parallel_root_workers(
                std::vector<RootMoveResult> &results,
                const othello::board::Board &root_board,
                const std::vector<othello::board::Move> &moves,
                Disc perspective_player,
                int depth,
                int alpha,
                int beta,
                const SearchOptions &options,
                std::size_t first_parallel_move)
            {
                const std::size_t remaining_root_moves = moves.size() - first_parallel_move;
                const std::size_t worker_count = parallel_root_worker_count(remaining_root_moves);
                if (worker_count == 0)
                {
                    return;
                }

                std::atomic<std::size_t> next_work_item{0};
                std::vector<std::thread> workers;
                workers.reserve(worker_count);

                for (std::size_t worker = 0; worker < worker_count; ++worker)
                {
                    workers.emplace_back([&, alpha, beta]()
                                         {
            while (true)
            {
                const std::size_t work_item =
                    next_work_item.fetch_add(1, std::memory_order_relaxed);
                if (work_item >= remaining_root_moves)
                {
                    return;
                }

                const std::size_t move_order = first_parallel_move + work_item;
                results[work_item] = evaluate_root_move_isolated(
                    root_board,
                    moves[move_order],
                    perspective_player,
                    depth,
                    alpha,
                    beta,
                    options,
                    move_order);
            } });
                }

                for (auto &worker : workers)
                {
                    worker.join();
                }
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
                    result.score = alphabeta_impl(
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
                const othello::board::Move *best_move = nullptr;
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

                            const int eval = alphabeta_impl(
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

                                if (root_result_is_better(
                                        result.score,
                                        best_move_order,
                                        move_result,
                                        best_move != nullptr))
                                {
                                    result.best_move = move_result.move;
                                    result.score = move_result.score;
                                    best_move = &moves[0];
                                    best_move_order = 0;
                                }

                                alpha = std::max(alpha, move_result.score);
                                first_parallel_move = 1;
                            }
                        }
                    }
                    
                    if (!context.aborted)
                    {
                        const std::size_t remaining_root_moves = moves.size() - first_parallel_move;
                        std::vector<RootMoveResult> parallel_results(remaining_root_moves);

                        run_parallel_root_workers(
                            parallel_results,
                            board,
                            moves,
                            perspective_player,
                            depth,
                            alpha,
                            beta,
                            context.options,
                            first_parallel_move);

                        for (const RootMoveResult &move_result : parallel_results)
                        {
                            accumulate_stats(context.stats, move_result.stats);

                            if (!move_result.completed)
                            {
                                context.aborted = true;
                                continue;
                            }

                            if (root_result_is_better(
                                    result.score,
                                    best_move_order,
                                    move_result,
                                    best_move != nullptr))
                            {
                                result.best_move = move_result.move;
                                result.score = move_result.score;
                                best_move = &moves[move_result.move_order];
                                best_move_order = move_result.move_order;
                            }
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

                        const int eval = alphabeta_impl(
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

                        if (root_result_is_better(
                                result.score,
                                best_move_order,
                                move_result,
                                best_move != nullptr))
                        {
                            result.best_move = move_result.move;
                            result.score = move_result.score;
                            best_move = &moves[move_order];
                            best_move_order = move_order;
                        }

                        alpha = std::max(alpha, move_result.score);
                    }
                }

                if (!context.aborted && best_move != nullptr)
                {
                    context.transposition_table.store(
                        board.get_hash(),
                        result.score,
                        depth,
                        TTEntryType::EXACT,
                        best_move);
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

        int alphabeta(othello::board::Board board, int depth, int alpha, int beta, Disc perspective_player)
        {
            SearchOptions options{};
            options.max_depth = depth;
            options.iterative_deepening = false;

            SearchContext context(options);
            context.transposition_table.new_search();
            return alphabeta_impl(
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
