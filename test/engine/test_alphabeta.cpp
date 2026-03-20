#include <gtest/gtest.h>
#include "board/board.hpp"
#include "engine/alphabeta.hpp"
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
    int eval = othello::engine::alphabeta(board, 1, -1000000, 1000000, BLACK);
    EXPECT_EQ(eval, 3);
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
    EXPECT_EQ(eval, -56);
}

TEST(AlphaBetaTest, TerminalState)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, BLACK)); // All Black
    set_board_state(board, state, BLACK);
    int eval = othello::engine::alphabeta(board, 3, -1000000, 1000000, BLACK);
    EXPECT_EQ(eval, 64);
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
    EXPECT_EQ(eval, -2);
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
    EXPECT_EQ(eval, -2);
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
    int eval = othello::engine::alphabeta(board, 2, -1000000, 1000000, BLACK);
    EXPECT_EQ(eval, 0);
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
    int eval = othello::engine::alphabeta(board, 5, -1000000, 1000000, BLACK);
    EXPECT_EQ(eval, 3);
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

    EXPECT_EQ(othello::engine::alphabeta(board, 1, ALPHABETA_MIN, ALPHABETA_MAX, WHITE), 64);
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
    EXPECT_EQ(othello::engine::alphabeta(board, 1, ALPHABETA_MIN, ALPHABETA_MAX, BLACK), 3);
    EXPECT_EQ(othello::engine::alphabeta(board, 2, ALPHABETA_MIN, ALPHABETA_MAX, BLACK), 0);
}
