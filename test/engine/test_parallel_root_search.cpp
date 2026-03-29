#include <chrono>

#include <gtest/gtest.h>
#include "board/board.hpp"
#include "engine/alphabeta.hpp"
#include "../utils.hpp"

namespace
{
    void expect_parallel_matches_sequential(
        const othello::board::Board &board,
        const othello::engine::SearchOptions &sequential_options,
        bool expect_exact_stats = false)
    {
        othello::engine::SearchOptions parallel_options = sequential_options;
        parallel_options.parallel_root = true;

        const auto sequential = othello::engine::find_best_move(board, sequential_options);
        const auto parallel = othello::engine::find_best_move(board, parallel_options);

        EXPECT_EQ(parallel.completed, sequential.completed);
        EXPECT_EQ(parallel.stats.completed_depth, sequential.stats.completed_depth);
        EXPECT_EQ(parallel.score, sequential.score);
        EXPECT_EQ(parallel.best_move.has_value(), sequential.best_move.has_value());

        if (expect_exact_stats)
        {
            EXPECT_EQ(parallel.stats.nodes_searched, sequential.stats.nodes_searched);
            EXPECT_EQ(parallel.stats.tt_hits, sequential.stats.tt_hits);
            EXPECT_EQ(parallel.stats.beta_cutoffs, sequential.stats.beta_cutoffs);
        }

        if (sequential.best_move.has_value())
        {
            EXPECT_EQ(parallel.best_move->row, sequential.best_move->row);
            EXPECT_EQ(parallel.best_move->col, sequential.best_move->col);
        }
    }

    othello::board::Board make_parallel_eligible_midgame_board()
    {
        othello::board::Board board;
        const std::vector<std::vector<Disc>> state = {
            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
            {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
            {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
            {EMPTY, BLACK, BLACK, WHITE, WHITE, WHITE, EMPTY, EMPTY},
            {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
            {EMPTY, EMPTY, WHITE, BLACK, BLACK, EMPTY, EMPTY, EMPTY},
            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
            {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        };
        set_board_state(board, state, BLACK);
        return board;
    }

}

TEST(ParallelRootTest, MatchesSequentialOpeningSearch)
{
    othello::board::Board board;
    board.set_current_player(BLACK);
    board.compute_valid_moves();

    othello::engine::SearchOptions options{};
    options.max_depth = 3;

    expect_parallel_matches_sequential(board, options);
}

TEST(ParallelRootTest, MatchesSequentialMidgameSearchAtParallelDepth)
{
    othello::board::Board board;
    const std::vector<std::vector<Disc>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, BLACK, BLACK, WHITE, WHITE, WHITE, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, WHITE, BLACK, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    };
    set_board_state(board, state, BLACK);

    EXPECT_GE(board.get_moves_for_current_state().size(), 5U);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;

    expect_parallel_matches_sequential(board, options);
}

TEST(ParallelRootTest, MatchesSequentialEndgameSearch)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, WHITE));
    state[0][0] = EMPTY;
    state[0][1] = BLACK;
    state[0][2] = BLACK;
    state[1][0] = BLACK;
    state[1][1] = BLACK;
    set_board_state(board, state, BLACK);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;

    expect_parallel_matches_sequential(board, options);
}

TEST(ParallelRootTest, KeepsResultSelectionDeterministicAtParallelDepth)
{
    othello::board::Board board;
    const std::vector<std::vector<Disc>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, BLACK, BLACK, WHITE, WHITE, WHITE, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, WHITE, BLACK, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    };
    set_board_state(board, state, BLACK);

    EXPECT_GE(board.get_moves_for_current_state().size(), 5U);

    othello::engine::SearchOptions sequential_options{};
    sequential_options.max_depth = 5;

    othello::engine::SearchOptions parallel_options = sequential_options;
    parallel_options.parallel_root = true;

    const auto sequential = othello::engine::find_best_move(board, sequential_options);

    EXPECT_TRUE(sequential.completed);
    ASSERT_TRUE(sequential.best_move.has_value());

    for (int attempt = 0; attempt < 32; ++attempt)
    {
        const auto parallel = othello::engine::find_best_move(board, parallel_options);

        EXPECT_TRUE(parallel.completed);
        ASSERT_TRUE(parallel.best_move.has_value());
        EXPECT_EQ(parallel.best_move->row, sequential.best_move->row);
        EXPECT_EQ(parallel.best_move->col, sequential.best_move->col);
        EXPECT_EQ(parallel.score, sequential.score);
    }
}

TEST(ParallelRootTest, FallsBackToSequentialWhenZeroNodeBudgetIsSetOnParallelEligibleSearch)
{
    othello::board::Board board = make_parallel_eligible_midgame_board();

    EXPECT_GE(board.get_moves_for_current_state().size(), 5U);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;
    options.node_limit = 0;

    expect_parallel_matches_sequential(board, options, true);
}

TEST(ParallelRootTest, FallsBackToSequentialWhenTimeBudgetIsSetOnParallelEligibleSearch)
{
    othello::board::Board board = make_parallel_eligible_midgame_board();

    EXPECT_GE(board.get_moves_for_current_state().size(), 5U);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;
    options.time_limit = std::chrono::milliseconds(0);

    expect_parallel_matches_sequential(board, options, true);
}

TEST(ParallelRootTest, FallsBackToSequentialWhenPositiveNodeBudgetIsSetOnParallelEligibleSearch)
{
    othello::board::Board board = make_parallel_eligible_midgame_board();

    EXPECT_GE(board.get_moves_for_current_state().size(), 5U);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;
    options.node_limit = 64;

    expect_parallel_matches_sequential(board, options, true);
}

TEST(ParallelRootTest, MatchesSequentialAcrossParallelRootWorkerCaps)
{
    othello::board::Board board = make_parallel_eligible_midgame_board();

    EXPECT_GE(board.get_moves_for_current_state().size(), 5U);

    const std::array<int, 5> worker_caps = {0, 1, 2, 3, 4};
    for (const int worker_cap : worker_caps)
    {
        SCOPED_TRACE(::testing::Message() << "worker_cap=" << worker_cap);

        othello::engine::SearchOptions options{};
        options.max_depth = 5;
        options.parallel_root_max_workers = worker_cap;

        expect_parallel_matches_sequential(board, options);
    }
}

TEST(ParallelRootTest, MatchesSequentialWhenNoLegalMove)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, WHITE));
    state[0][0] = EMPTY;
    state[0][1] = BLACK;
    state[0][2] = WHITE;
    set_board_state(board, state, BLACK);

    othello::engine::SearchOptions options{};
    options.max_depth = 3;

    expect_parallel_matches_sequential(board, options);
}

TEST(ParallelRootTest, MatchesSequentialOnTerminalState)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, BLACK));
    set_board_state(board, state, BLACK);

    othello::engine::SearchOptions options{};
    options.max_depth = 3;

    expect_parallel_matches_sequential(board, options);
}

TEST(ParallelRootTest, FallsBackToSequentialWhenDepthIsBelowParallelThreshold)
{
    othello::board::Board board;
    const std::vector<std::vector<Disc>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, BLACK, BLACK, WHITE, WHITE, WHITE, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, WHITE, BLACK, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    };
    set_board_state(board, state, BLACK);

    othello::engine::SearchOptions options{};
    options.max_depth = 4;

    expect_parallel_matches_sequential(board, options, true);
}

TEST(ParallelRootTest, FallsBackToSequentialWhenRootMoveCountIsBelowParallelThreshold)
{
    othello::board::Board board;
    board.set_current_player(BLACK);
    board.compute_valid_moves();

    EXPECT_EQ(board.get_moves_for_current_state().size(), 4U);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;

    expect_parallel_matches_sequential(board, options, true);
}

TEST(ParallelRootTest, FallsBackToSequentialWhenConfiguredParallelDepthThresholdIsHigher)
{
    othello::board::Board board = make_parallel_eligible_midgame_board();

    EXPECT_GE(board.get_moves_for_current_state().size(), 5U);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;
    options.parallel_root_min_depth = 6;

    expect_parallel_matches_sequential(board, options, true);
}

TEST(ParallelRootTest, FallsBackToSequentialWhenConfiguredParallelMoveThresholdIsHigher)
{
    othello::board::Board board = make_parallel_eligible_midgame_board();

    const int root_move_count = static_cast<int>(board.get_moves_for_current_state().size());
    EXPECT_GE(root_move_count, 5);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;
    options.parallel_root_min_moves = root_move_count + 1;

    expect_parallel_matches_sequential(board, options, true);
}

TEST(ParallelRootTest, MatchesSequentialWhenParallelRootWorkersAreCapped)
{
    othello::board::Board board = make_parallel_eligible_midgame_board();

    EXPECT_GE(board.get_moves_for_current_state().size(), 5U);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;
    options.parallel_root_max_workers = 1;

    expect_parallel_matches_sequential(board, options);
}

TEST(ParallelRootTest, MatchesSequentialRepeatedlyWhenOneWorkerReusesState)
{
    othello::board::Board board = make_parallel_eligible_midgame_board();

    EXPECT_GE(board.get_moves_for_current_state().size(), 5U);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;
    options.parallel_root_max_workers = 1;

    for (int attempt = 0; attempt < 16; ++attempt)
    {
        expect_parallel_matches_sequential(board, options);
    }
}

TEST(ParallelRootTest, MatchesSequentialRepeatedlyWhenThreadPoolWorkersPersist)
{
    othello::board::Board board = make_parallel_eligible_midgame_board();

    EXPECT_GE(board.get_moves_for_current_state().size(), 5U);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;
    options.parallel_root_max_workers = 2;

    for (int attempt = 0; attempt < 16; ++attempt)
    {
        expect_parallel_matches_sequential(board, options);
    }
}

TEST(ParallelRootTest, MatchesSequentialRepeatedlyAtParallelDepth)
{
    othello::board::Board board;
    const std::vector<std::vector<Disc>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, BLACK, BLACK, WHITE, WHITE, WHITE, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, WHITE, BLACK, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    };
    set_board_state(board, state, BLACK);

    EXPECT_GE(board.get_moves_for_current_state().size(), 5U);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;

    for (int attempt = 0; attempt < 16; ++attempt)
    {
        expect_parallel_matches_sequential(board, options);
    }
}

TEST(ParallelRootTest, MatchesSequentialWhenParallelRootRunsInMultipleAlphaRefreshBatches)
{
    othello::board::Board board = make_parallel_eligible_midgame_board();

    const std::size_t root_move_count = board.get_moves_for_current_state().size();
    EXPECT_GE(root_move_count, 5U);

    othello::engine::SearchOptions options{};
    options.max_depth = 5;
    options.parallel_root_max_workers = 2;

    for (int attempt = 0; attempt < 8; ++attempt)
    {
        expect_parallel_matches_sequential(board, options);
    }
}
