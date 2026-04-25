#include <gtest/gtest.h>

#include "board/board.hpp"
#include "engine/evaluation.hpp"
#include "../utils.hpp"

TEST(EvaluationTest, OpeningBoardScoresZeroForBothPlayers)
{
    othello::board::Board board;
    board.set_current_player(BLACK);

    EXPECT_EQ(othello::engine::evaluate_position(board, BLACK), 0);
    EXPECT_EQ(othello::engine::evaluate_position(board, WHITE), 0);
}

TEST(EvaluationTest, CornerOwnershipImprovesScore)
{
    othello::board::Board board;
    const std::vector<std::vector<Disc>> state = {
        {BLACK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, WHITE, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    };
    set_board_state(board, state, BLACK);

    EXPECT_GT(othello::engine::evaluate_position(board, BLACK), 0);
    EXPECT_LT(othello::engine::evaluate_position(board, WHITE), 0);
}

TEST(EvaluationTest, TerminalBoardsRemainPerspectiveSymmetric)
{
    othello::board::Board board;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, BLACK));
    set_board_state(board, state, BLACK);

    const int black_eval = othello::engine::evaluate_position(board, BLACK);
    const int white_eval = othello::engine::evaluate_position(board, WHITE);

    EXPECT_GT(black_eval, 0);
    EXPECT_LT(white_eval, 0);
    EXPECT_EQ(black_eval, -white_eval);
}

TEST(EvaluationTest, NonParityFeaturesBreakEqualDiscTie)
{
    othello::board::Board board;
    const std::vector<std::vector<Disc>> state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, WHITE, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    };
    set_board_state(board, state, BLACK);

    EXPECT_EQ(board.disc_diff(BLACK), 0);
    EXPECT_EQ(board.disc_diff(WHITE), 0);
    EXPECT_GT(othello::engine::evaluate_position(board, BLACK), 0);
    EXPECT_LT(othello::engine::evaluate_position(board, WHITE), 0);
}

TEST(EvaluationTest, CompactStructureOutscoresExposedStructure)
{
    othello::board::Board compact_black_board;
    const std::vector<std::vector<Disc>> compact_black_state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, WHITE, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, BLACK, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, WHITE, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    };
    set_board_state(compact_black_board, compact_black_state, BLACK);

    othello::board::Board compact_white_board;
    const std::vector<std::vector<Disc>> compact_white_state = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, BLACK, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, WHITE, WHITE, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, WHITE, WHITE, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, BLACK, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    };
    set_board_state(compact_white_board, compact_white_state, BLACK);

    EXPECT_EQ(compact_black_board.disc_diff(BLACK), 0);
    EXPECT_EQ(compact_white_board.disc_diff(BLACK), 0);
    EXPECT_GT(
        othello::engine::evaluate_position(compact_black_board, BLACK),
        othello::engine::evaluate_position(compact_white_board, BLACK));
}
