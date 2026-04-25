#include <chrono>

#include <gtest/gtest.h>
#include "board/board.hpp"
#include "engine/alphabeta.hpp"
#include "engine/trans_table.hpp"
#include "../utils.hpp"

TEST(AlphaBetaTest, StandardOpeningBoard)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, WHITE, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};
    set_board_state(board, state, BLACK);
    const int black_eval = othello::engine::alphabeta(board, 1, -1000000, 1000000, BLACK);
    const int white_eval = othello::engine::alphabeta(board, 1, -1000000, 1000000, WHITE);
    EXPECT_EQ(black_eval, -white_eval);
}

TEST(AlphaBetaTest, NoLegalMovesForMaximizingPlayer)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state = {
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};
    set_board_state(board, state, BLACK);
    int eval = othello::engine::alphabeta(board, 2, -1000000, 1000000, BLACK);
    EXPECT_LT(eval, 0);
}

TEST(AlphaBetaTest, TerminalState)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, BLACK)); // All Black
    set_board_state(board, state, BLACK);
    const int black_eval = othello::engine::alphabeta(board, 3, -1000000, 1000000, BLACK);
    const int white_eval = othello::engine::alphabeta(board, 3, -1000000, 1000000, WHITE);
    EXPECT_GT(black_eval, 0);
    EXPECT_LT(white_eval, 0);
    EXPECT_EQ(black_eval, -white_eval);
}

TEST(AlphaBetaTest, AlphaCutoff)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state = {
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE}};
    set_board_state(board, state, BLACK);
    int eval = othello::engine::alphabeta(board, 3, -1000000, 1000000, BLACK);
    EXPECT_LT(eval, 0);
}

TEST(AlphaBetaTest, BetaCutoff)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state = {
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK}};
    set_board_state(board, state, WHITE);
    int eval = othello::engine::alphabeta(board, 3, -1000000, 1000000, WHITE);
    EXPECT_LT(eval, 0);
}

TEST(AlphaBetaTest, MaximizingVsMinimizing)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, WHITE, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};
    set_board_state(board, state, BLACK);
    const int black_eval = othello::engine::alphabeta(board, 2, -1000000, 1000000, BLACK);
    const int white_eval = othello::engine::alphabeta(board, 2, -1000000, 1000000, WHITE);
    EXPECT_EQ(black_eval, -white_eval);
}

TEST(AlphaBetaTest, DeepSearch)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, WHITE, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, WHITE, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};
    set_board_state(board, state, BLACK);
    const int black_eval = othello::engine::alphabeta(board, 5, -1000000, 1000000, BLACK);
    const int white_eval = othello::engine::alphabeta(board, 5, -1000000, 1000000, WHITE);
    EXPECT_EQ(black_eval, -white_eval);
}

TEST(AlphaBetaTest, PassTurnUsesOpponentMove)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, WHITE));
    state[0][0] = EMPTY;
    state[0][1] = BLACK;
    state[0][2] = WHITE;
    set_board_state(board, state, BLACK);

    EXPECT_EQ(board.compute_valid_moves(), 0);
    board.set_current_player(WHITE);
    EXPECT_EQ(board.compute_valid_moves(), 1);
    board.set_current_player(BLACK);

    othello::board::Board opponent_turn_board = board;
    opponent_turn_board.set_current_player(WHITE);

    EXPECT_EQ(
        othello::engine::alphabeta(board, 1, ALPHABETA_MIN, ALPHABETA_MAX, WHITE),
        othello::engine::alphabeta(opponent_turn_board, 1, ALPHABETA_MIN, ALPHABETA_MAX, WHITE));
}

TEST(AlphaBetaTest, SearchAdvancesToOpponentTurn)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, WHITE, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};
    set_board_state(board, state, BLACK);

    EXPECT_EQ(board.compute_valid_moves(), 4);
    const int depth_one_eval = othello::engine::alphabeta(board, 1, ALPHABETA_MIN, ALPHABETA_MAX, BLACK);
    const int depth_two_eval = othello::engine::alphabeta(board, 2, ALPHABETA_MIN, ALPHABETA_MAX, BLACK);

    EXPECT_NE(depth_one_eval, depth_two_eval);
}

TEST(AlphaBetaTest, FindBestMoveReturnsLegalOpeningMove)
{
    othello::board::Board board;
    board.set_current_player(BLACK);
    board.compute_valid_moves();

    const auto result = othello::engine::find_best_move(board, 2);
    const int expected_score = othello::engine::alphabeta(board, 2, ALPHABETA_MIN, ALPHABETA_MAX, BLACK);

    ASSERT_TRUE(result.best_move.has_value());
    EXPECT_TRUE(board.is_valid_move(result.best_move->row, result.best_move->col, BLACK));
    EXPECT_EQ(result.score, expected_score);
}

TEST(AlphaBetaTest, FindBestMoveReturnsNulloptWhenPlayerMustPass)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, WHITE));
    state[0][0] = EMPTY;
    state[0][1] = BLACK;
    state[0][2] = WHITE;
    set_board_state(board, state, BLACK);

    const int expected_score = othello::engine::alphabeta(board, 1, ALPHABETA_MIN, ALPHABETA_MAX, BLACK);
    const auto result = othello::engine::find_best_move(board, 1);

    EXPECT_FALSE(result.best_move.has_value());
    EXPECT_EQ(result.score, expected_score);
}

TEST(AlphaBetaTest, SearchOptionsCanDisableIterativeDeepening)
{
    othello::board::Board board;
    board.set_current_player(BLACK);

    othello::engine::SearchOptions options{};
    options.max_depth = 3;
    options.iterative_deepening = false;

    const auto direct = othello::engine::find_best_move(board, options);
    const auto iterative = othello::engine::find_best_move(board, 3);

    EXPECT_TRUE(direct.completed);
    EXPECT_EQ(direct.stats.completed_depth, 3);
    ASSERT_TRUE(direct.best_move.has_value());
    ASSERT_TRUE(iterative.best_move.has_value());
    EXPECT_TRUE(board.is_valid_move(direct.best_move->row, direct.best_move->col, BLACK));
    EXPECT_TRUE(board.is_valid_move(iterative.best_move->row, iterative.best_move->col, BLACK));
    EXPECT_EQ(direct.score, iterative.score);
}

TEST(AlphaBetaTest, SearchResultReportsStatsForCompletedSearch)
{
    othello::board::Board board;
    board.set_current_player(BLACK);

    othello::engine::SearchOptions options{};
    options.max_depth = 3;

    const auto result = othello::engine::find_best_move(board, options);

    EXPECT_TRUE(result.completed);
    EXPECT_EQ(result.stats.completed_depth, 3);
    EXPECT_GT(result.stats.nodes_searched, 0U);
    EXPECT_GE(result.stats.elapsed.count(), 0);
}

TEST(AlphaBetaTest, SearchResultStopsImmediatelyWhenNodeBudgetIsZero)
{
    othello::board::Board board;
    board.set_current_player(BLACK);

    othello::engine::SearchOptions options{};
    options.max_depth = 4;
    options.node_limit = 0;

    const auto result = othello::engine::find_best_move(board, options);

    EXPECT_FALSE(result.completed);
    EXPECT_FALSE(result.best_move.has_value());
    EXPECT_EQ(result.stats.completed_depth, 0);
    EXPECT_EQ(result.stats.nodes_searched, 0U);
}

TEST(AlphaBetaTest, SearchResultStopsImmediatelyWhenTimeBudgetIsZero)
{
    othello::board::Board board;
    board.set_current_player(BLACK);

    othello::engine::SearchOptions options{};
    options.max_depth = 4;
    options.time_limit = std::chrono::milliseconds(0);

    const auto result = othello::engine::find_best_move(board, options);

    EXPECT_FALSE(result.completed);
    EXPECT_EQ(result.stats.completed_depth, 0);
    EXPECT_EQ(result.stats.nodes_searched, 0U);
}

TEST(TranspositionTableTest, StoreLookupAndClear)
{
    othello::engine::TranspositionTable transposition_table(8);
    othello::engine::TTEntry entry{};

    EXPECT_FALSE(transposition_table.lookup(42, entry));

    transposition_table.store(42, 7, 3, othello::engine::TTEntryType::EXACT);

    ASSERT_TRUE(transposition_table.lookup(42, entry));
    EXPECT_EQ(entry.hash, 42U);
    EXPECT_EQ(entry.value, 7);
    EXPECT_EQ(entry.depth, 3);
    EXPECT_EQ(entry.type, othello::engine::TTEntryType::EXACT);
    EXPECT_FALSE(entry.has_best_move());

    transposition_table.clear();
    EXPECT_EQ(transposition_table.size(), 0U);
}

TEST(TranspositionTableTest, StorePrefersDeeperEntryForSameHash)
{
    othello::engine::TranspositionTable transposition_table(4);
    othello::engine::TTEntry entry{};

    transposition_table.store(42, 7, 3, othello::engine::TTEntryType::EXACT);
    transposition_table.store(42, 2, 1, othello::engine::TTEntryType::LOWERBOUND);

    ASSERT_TRUE(transposition_table.lookup(42, entry));
    EXPECT_EQ(entry.value, 7);
    EXPECT_EQ(entry.depth, 3);
    EXPECT_EQ(entry.type, othello::engine::TTEntryType::EXACT);

    transposition_table.store(42, 9, 5, othello::engine::TTEntryType::LOWERBOUND);

    ASSERT_TRUE(transposition_table.lookup(42, entry));
    EXPECT_EQ(entry.value, 9);
    EXPECT_EQ(entry.depth, 5);
    EXPECT_EQ(entry.type, othello::engine::TTEntryType::LOWERBOUND);
}

TEST(TranspositionTableTest, StoreRoundTripsBestMove)
{
    othello::engine::TranspositionTable transposition_table(2);
    othello::engine::TTEntry entry{};
    const othello::board::Move best_move(2, 3);

    transposition_table.store(42, 7, 4, othello::engine::TTEntryType::EXACT, &best_move);

    ASSERT_TRUE(transposition_table.lookup(42, entry));
    ASSERT_TRUE(entry.has_best_move());
    EXPECT_EQ(entry.best_move.row, 2);
    EXPECT_EQ(entry.best_move.col, 3);
}

TEST(TranspositionTableTest, StoreKeepsDeeperEntryOnSameGenerationCollision)
{
    othello::engine::TranspositionTable transposition_table(2);
    othello::engine::TTEntry entry{};

    transposition_table.store(1, 10, 5, othello::engine::TTEntryType::EXACT);
    transposition_table.store(3, 20, 2, othello::engine::TTEntryType::LOWERBOUND);

    EXPECT_EQ(transposition_table.size(), 1U);
    ASSERT_TRUE(transposition_table.lookup(1, entry));
    EXPECT_EQ(entry.value, 10);
    EXPECT_EQ(entry.depth, 5);
    EXPECT_FALSE(transposition_table.lookup(3, entry));

    transposition_table.store(3, 30, 6, othello::engine::TTEntryType::LOWERBOUND);

    EXPECT_EQ(transposition_table.size(), 1U);
    EXPECT_FALSE(transposition_table.lookup(1, entry));
    ASSERT_TRUE(transposition_table.lookup(3, entry));
    EXPECT_EQ(entry.value, 30);
    EXPECT_EQ(entry.depth, 6);
}

TEST(TranspositionTableTest, StoreReplacesOlderGenerationOnCollision)
{
    othello::engine::TranspositionTable transposition_table(2);
    othello::engine::TTEntry entry{};

    transposition_table.store(1, 10, 5, othello::engine::TTEntryType::EXACT);
    transposition_table.new_search();
    transposition_table.store(3, 20, 1, othello::engine::TTEntryType::UPPERBOUND);

    EXPECT_EQ(transposition_table.size(), 1U);
    EXPECT_FALSE(transposition_table.lookup(1, entry));
    ASSERT_TRUE(transposition_table.lookup(3, entry));
    EXPECT_EQ(entry.value, 20);
    EXPECT_EQ(entry.depth, 1);
    EXPECT_EQ(entry.type, othello::engine::TTEntryType::UPPERBOUND);
}
