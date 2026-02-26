#include <gtest/gtest.h>
#include "board.hpp"
#include "alphabeta.hpp"
#include "../utils.hpp"

TEST(AlphaBetaTest, StandardOpeningBoard)
{
    Board board;
    std::vector<std::vector<int>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, WHITE, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};
    set_board_state(board, state, BLACK);
    int eval = alphabeta(board, 1, -1000000, 1000000, true);
    EXPECT_EQ(eval, -3); // matches actual eval output
}

TEST(AlphaBetaTest, NoLegalMovesForMaximizingPlayer)
{
    Board board;
    std::vector<std::vector<int>> state = {
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};
    set_board_state(board, state, BLACK);
    int eval = alphabeta(board, 2, -1000000, 1000000, true);
    EXPECT_EQ(eval, -56);
}

TEST(AlphaBetaTest, TerminalState)
{
    Board board;
    std::vector<std::vector<int>> state(8, std::vector<int>(8, BLACK)); // All Black
    set_board_state(board, state, BLACK);
    int eval = alphabeta(board, 3, -1000000, 1000000, true);
    EXPECT_EQ(eval, 64);
}

TEST(AlphaBetaTest, AlphaCutoff)
{
    Board board;
    std::vector<std::vector<int>> state = {
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE}};
    set_board_state(board, state, BLACK);
    int eval = alphabeta(board, 3, -1000000, 1000000, true);
    EXPECT_EQ(eval, -2);
}

TEST(AlphaBetaTest, BetaCutoff)
{
    Board board;
    std::vector<std::vector<int>> state = {
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE},
        {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
        {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK}};
    set_board_state(board, state, WHITE);
    int eval = alphabeta(board, 3, -1000000, 1000000, false);
    EXPECT_EQ(eval, -2);
}

TEST(AlphaBetaTest, MaximizingVsMinimizing)
{
    Board board;
    std::vector<std::vector<int>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, WHITE, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};
    set_board_state(board, state, BLACK);
    int eval = alphabeta(board, 2, -1000000, 1000000, true);
    EXPECT_EQ(eval, 2);
}

TEST(AlphaBetaTest, DeepSearch)
{
    Board board;
    std::vector<std::vector<int>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, WHITE, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, WHITE, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}};
    set_board_state(board, state, BLACK);
    int eval = alphabeta(board, 5, -1000000, 1000000, true);
    EXPECT_EQ(eval, -7);
}
