#include <gtest/gtest.h>
#include "board/board.hpp"
#include "../utils.hpp"
// sum_game_stats: Not easily testable without capturing stdout, so omitted here.
// Destructor: Memory leaks should be checked with valgrind or similar tool, not in unit test.

// Constructor tests
TEST(BoardState, DefaultConstruction)
{
    othello::board::Board b;
    // Board is 8x8, center 4 discs correct
    EXPECT_EQ(b.get_square(3, 3), WHITE);
    EXPECT_EQ(b.get_square(3, 4), BLACK);
    EXPECT_EQ(b.get_square(4, 3), BLACK);
    EXPECT_EQ(b.get_square(4, 4), WHITE);
    // All other squares empty
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (!((r == 3 || r == 4) && (c == 3 || c == 4)))
                EXPECT_EQ(b.get_square(r, c), EMPTY);
    // Current player is BLACK (or as per rules)
    EXPECT_TRUE(b.get_current_player() == BLACK || b.get_current_player() == WHITE);
}

TEST(BoardState, MultipleBoardsIndependent)
{
    othello::board::Board b1, b2;
    b1.set_square(2, 2, BLACK);
    b2.set_square(5, 5, WHITE);
    EXPECT_EQ(b1.get_square(2, 2), BLACK);
    EXPECT_EQ(b2.get_square(5, 5), WHITE);
}

// get_square tests
TEST(BoardState, GetSquareValid)
{

    othello::board::Board b;
    EXPECT_EQ(b.get_square(3, 3), WHITE);
    EXPECT_EQ(b.get_square(0, 0), EMPTY);
    EXPECT_EQ(b.get_square(7, 7), EMPTY);
}

// set_square tests
TEST(BoardState, SetSquareBlackWhite)
{

    othello::board::Board b;
    b.set_square(2, 2, BLACK);
    EXPECT_EQ(b.get_square(2, 2), BLACK);
    b.set_square(5, 5, WHITE);
    EXPECT_EQ(b.get_square(5, 5), WHITE);
}

TEST(BoardState, SetSquareOverwrite)
{

    othello::board::Board b;
    b.set_square(2, 2, BLACK);
    b.set_square(2, 2, WHITE);
    EXPECT_EQ(b.get_square(2, 2), WHITE);
}

// get_black_count, get_white_count tests
TEST(BoardState, DiscCountsInitial)
{

    othello::board::Board b;
    EXPECT_EQ(b.get_black_count(), 2);
    EXPECT_EQ(b.get_white_count(), 2);
}

TEST(BoardState, DiscCountsAfterMoves)
{

    othello::board::Board b;
    b.set_square(0, 0, BLACK);
    b.set_square(1, 1, WHITE);
    EXPECT_EQ(b.get_black_count(), 3);
    EXPECT_EQ(b.get_white_count(), 3);
}

TEST(BoardState, DiscCountsFullBoard)
{

    othello::board::Board b;
    fill_board(b, BLACK);
    EXPECT_EQ(b.get_black_count(), 64);
    EXPECT_EQ(b.get_white_count(), 0);
}

// evaluate tests
TEST(BoardState, EvaluateInitial)
{

    othello::board::Board b;
    EXPECT_EQ(b.evaluate(BLACK), 0);
    EXPECT_EQ(b.evaluate(WHITE), 0);
}

TEST(BoardState, EvaluateMoreBlack)
{

    othello::board::Board b;
    fill_board(b, BLACK);
    EXPECT_GT(b.evaluate(BLACK), 0);
    EXPECT_LT(b.evaluate(WHITE), 0);
}

// is_game_over tests
TEST(BoardState, GameNotOverInitial)
{
    othello::board::Board b;
    EXPECT_FALSE(b.is_game_over());
}

TEST(BoardState, GameOverFullBoard)
{
    othello::board::Board b;
    fill_board(b, BLACK);
    EXPECT_TRUE(b.is_game_over());
}

// get_current_player/set_current_player tests
TEST(BoardState, CurrentPlayerSetGet)
{

    othello::board::Board b;
    b.set_current_player(BLACK);
    EXPECT_EQ(b.get_current_player(), BLACK);
    b.set_current_player(WHITE);
    EXPECT_EQ(b.get_current_player(), WHITE);
}
