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

TEST(ParallelRootTest, KeepsTieBreakDeterministic)
{
    othello::board::Board board;
    board.set_current_player(BLACK);
    board.compute_valid_moves();

    othello::engine::SearchOptions sequential_options{};
    sequential_options.max_depth = 1;
    sequential_options.iterative_deepening = false;

    othello::engine::SearchOptions parallel_options = sequential_options;
    parallel_options.parallel_root = true;

    const auto sequential = othello::engine::find_best_move(board, sequential_options);

    EXPECT_TRUE(sequential.completed);
    ASSERT_TRUE(sequential.best_move.has_value());

    for (int attempt = 0; attempt < 64; ++attempt)
    {
        const auto parallel = othello::engine::find_best_move(board, parallel_options);

        EXPECT_TRUE(parallel.completed);
        ASSERT_TRUE(parallel.best_move.has_value());
        EXPECT_EQ(parallel.best_move->row, sequential.best_move->row);
        EXPECT_EQ(parallel.best_move->col, sequential.best_move->col);
        EXPECT_EQ(parallel.score, sequential.score);
    }
}

TEST(ParallelRootTest, FallsBackToSequentialWhenNodeBudgetIsSet)
{
    othello::board::Board board;
    board.set_current_player(BLACK);

    othello::engine::SearchOptions options{};
    options.max_depth = 4;
    options.node_limit = 0;

    expect_parallel_matches_sequential(board, options, true);
}

TEST(ParallelRootTest, FallsBackToSequentialWhenTimeBudgetIsSet)
{
    othello::board::Board board;
    board.set_current_player(BLACK);

    othello::engine::SearchOptions options{};
    options.max_depth = 4;
    options.time_limit = std::chrono::milliseconds(0);

    expect_parallel_matches_sequential(board, options, true);
}

TEST(ParallelRootTest, FallsBackToSequentialWhenPositiveNodeBudgetIsSet)
{
    othello::board::Board board;
    board.set_current_player(BLACK);

    othello::engine::SearchOptions options{};
    options.max_depth = 4;
    options.node_limit = 5;

    expect_parallel_matches_sequential(board, options, true);
}

TEST(ParallelRootTest, MatchesSequentialAcrossDepthsOnMidgameBoard)
{
    othello::board::Board midgame;
    const std::vector<std::vector<Disc>> midgame_state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, BLACK, BLACK, WHITE, WHITE, WHITE, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, WHITE, BLACK, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    };
    set_board_state(midgame, midgame_state, BLACK);

    const std::array<int, 3> depths = {1, 3, 5};
    for (const int depth : depths)
    {
        othello::engine::SearchOptions options{};
        options.max_depth = depth;

        expect_parallel_matches_sequential(midgame, options);
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


