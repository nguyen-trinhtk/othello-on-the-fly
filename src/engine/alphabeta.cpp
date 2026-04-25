#include <algorithm>
#include <array>
#include <chrono>
#include <utility>

#include "engine/alphabeta.hpp"
#include "engine/evaluation.hpp"
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
                int score = static_cast<int>(move.flip_count());

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
                    return evaluate_position(board, perspective_player);
                }

                ++context.stats.nodes_searched;
                if (depth == 0)
                {
                    return evaluate_position(board, perspective_player);
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
                        return evaluate_position(board, perspective_player);
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
                        return best_move != nullptr ? best_eval : evaluate_position(board, perspective_player);
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
                    return best_move != nullptr ? best_eval : evaluate_position(board, perspective_player);
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

                return evaluate_position(board, perspective_player);
            }

            SearchResult search_at_depth(
                othello::board::Board &board,
                int depth,
                SearchContext &context)
            {
                SearchResult result{};
                const Disc perspective_player = board.get_current_player();
                result.score = evaluate_position(board, perspective_player);

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

                int alpha = ALPHABETA_MIN;
                const int beta = ALPHABETA_MAX;
                const othello::board::Move *best_move = nullptr;

                for (const auto &candidate : moves)
                {
                    if (budget_exhausted(context))
                    {
                        break;
                    }

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

                    if (best_move == nullptr || eval > result.score)
                    {
                        result.best_move = candidate;
                        result.score = eval;
                        best_move = &candidate;
                    }
                    alpha = std::max(alpha, eval);
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
                best_result.score = evaluate_position(
                    working_board,
                    working_board.get_current_player());
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
