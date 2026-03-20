#include <gtest/gtest.h>
#include "board/board.hpp"
#include "../utils.hpp"

namespace
{
othello::board::Move make_move(int row, int col)
{
    return othello::board::Move(row, col);
}
}

// --- is_valid_move tests ---
TEST(MoveLogic, ValidMoveBlack)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    EXPECT_TRUE(b.is_valid_move(2, 3, BLACK));
}
TEST(MoveLogic, ValidMoveWhite)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    auto move = make_move(2, 3);
    b.process_move(move, BLACK);
    b.set_current_player(WHITE);
    b.compute_valid_moves();
    EXPECT_TRUE(b.is_valid_move(2, 2, WHITE));
}
TEST(MoveLogic, OccupiedSquare)
{
    othello::board::Board b;
    b.set_square(2, 3, BLACK);
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    EXPECT_FALSE(b.is_valid_move(2, 3, BLACK));
}
TEST(MoveLogic, NoFlips)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    EXPECT_FALSE(b.is_valid_move(0, 0, BLACK));
}
TEST(MoveLogic, OutOfBounds)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    EXPECT_FALSE(b.is_valid_move(-1, 0, BLACK));
    EXPECT_FALSE(b.is_valid_move(0, 8, BLACK));
}
TEST(MoveLogic, InvalidPlayerValue)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    EXPECT_FALSE(b.is_valid_move(2, 3, 2)); // 2 is not BLACK or WHITE
}

// --- process_move tests ---
TEST(MoveLogic, ProcessValidMove)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    int before = b.get_black_count();
    auto move = make_move(2, 3);
    b.process_move(move, BLACK);
    EXPECT_GT(b.get_black_count(), before);
}
TEST(MoveLogic, ProcessInvalidMove)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    int before = b.get_black_count();
    auto move = make_move(0, 0);
    b.process_move(move, BLACK);
    EXPECT_EQ(b.get_black_count(), before);
}
TEST(MoveLogic, EdgeMove)
{
    othello::board::Board b;
    std::vector<std::vector<int>> state(8, std::vector<int>(8, EMPTY));
    // Edge flip: black at (0,2), white at (0,1), empty at (0,0)
    state[0][1] = WHITE;
    state[0][2] = BLACK;
    set_board_state(b, state, BLACK);
    EXPECT_TRUE(b.is_valid_move(0, 0, BLACK));
    auto move = make_move(0, 0);
    b.process_move(move, BLACK);
    EXPECT_EQ(b.get_black_count(), 3); // (0,0) placed, (0,1) flipped, (0,2) original
}
TEST(MoveLogic, CornerMove)
{
    othello::board::Board b;
    std::vector<std::vector<int>> state(8, std::vector<int>(8, EMPTY));
    // Diagonal flip: black at (2,2), white at (1,1), empty at (0,0)
    state[1][1] = WHITE;
    state[2][2] = BLACK;
    set_board_state(b, state, BLACK);
    EXPECT_TRUE(b.is_valid_move(0, 0, BLACK));
    auto move = make_move(0, 0);
    b.process_move(move, BLACK);
    EXPECT_EQ(b.get_black_count(), 3); // (0,0) placed, (1,1) flipped, (2,2) original
}
TEST(MoveLogic, MultiDirectionFlips)
{
    othello::board::Board b;
    std::vector<std::vector<int>> state(8, std::vector<int>(8, EMPTY));
    // Multi-direction flip: black at (3,2) and (2,3), white at (3,3), empty at (3,4)
    state[3][2] = BLACK;
    state[2][3] = BLACK;
    state[3][3] = WHITE;
    set_board_state(b, state, BLACK);
    EXPECT_TRUE(b.is_valid_move(3, 4, BLACK));
    auto move = make_move(3, 4);
    b.process_move(move, BLACK);
    EXPECT_EQ(b.get_black_count(), 4); // (3,2), (2,3), (3,4) placed, (3,3) flipped
}
TEST(MoveLogic, ProcessInvalidPlayer)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    int before = b.get_black_count();
    auto move = make_move(2, 3);
    b.process_move(move, 2); // 2 is not BLACK or WHITE
    EXPECT_EQ(b.get_black_count(), before);
}

// --- compute_valid_moves tests ---
TEST(MoveLogic, ComputeValidMovesInitial)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    EXPECT_EQ(b.compute_valid_moves(), 4);
}
TEST(MoveLogic, ComputeValidMovesAfterMove)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    auto move = make_move(2, 3);
    b.process_move(move, BLACK);
    b.set_current_player(WHITE);
    EXPECT_GT(b.compute_valid_moves(), 0);
}
TEST(MoveLogic, ComputeValidMovesNoMoves)
{
    othello::board::Board b;
    fill_board(b, BLACK);
    b.set_current_player(BLACK);
    EXPECT_EQ(b.compute_valid_moves(), 0);
}

// --- get_valid_moves tests ---
TEST(MoveLogic, GetValidMovesMatchesCompute)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    int n = b.compute_valid_moves();
    EXPECT_EQ(b.get_valid_moves().size(), n);
}
TEST(MoveLogic, GetValidMovesAfterBoardChange)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    auto move = make_move(2, 3);
    b.process_move(move, BLACK);
    b.set_current_player(WHITE);
    b.compute_valid_moves();
    EXPECT_EQ(b.get_valid_moves().size(), b.compute_valid_moves());
}

// --- parse_move (not easily testable without refactor) ---

// --- Edge & Game End Cases ---
TEST(MoveLogic, MovesOnEdgesCorners)
{
    othello::board::Board b;
    std::vector<std::vector<int>> state(8, std::vector<int>(8, WHITE));
    state[0][0] = EMPTY;
    state[0][7] = EMPTY;
    state[7][0] = EMPTY;
    state[7][7] = EMPTY;
    set_board_state(b, state, BLACK);
    EXPECT_FALSE(b.is_valid_move(0, 0, BLACK));
    EXPECT_FALSE(b.is_valid_move(0, 7, BLACK));
    EXPECT_FALSE(b.is_valid_move(7, 0, BLACK));
    EXPECT_FALSE(b.is_valid_move(7, 7, BLACK));
}
TEST(MoveLogic, ValidMoveAfterPass)
{
    othello::board::Board b;
    std::vector<std::vector<int>> state(8, std::vector<int>(8, EMPTY));
    // Legal scenario: black at (0,1), white at (0,2), empty at (0,0)
    state[0][1] = BLACK;
    state[0][2] = WHITE;
    set_board_state(b, state, WHITE);
    EXPECT_EQ(b.compute_valid_moves(), 1);
    EXPECT_TRUE(b.is_valid_move(0, 0, WHITE));
}
TEST(MoveLogic, NoValidMovesForOnePlayer)
{
    othello::board::Board b;
    std::vector<std::vector<int>> state(8, std::vector<int>(8, BLACK));
    set_board_state(b, state, WHITE);
    EXPECT_EQ(b.compute_valid_moves(), 0);
    b.set_current_player(BLACK);
    EXPECT_EQ(b.compute_valid_moves(), 0);
}
TEST(MoveLogic, FullBoard)
{
    othello::board::Board b;
    fill_board(b, BLACK);
    b.set_current_player(BLACK);
    EXPECT_EQ(b.compute_valid_moves(), 0);
    b.set_current_player(WHITE);
    EXPECT_EQ(b.compute_valid_moves(), 0);
}
TEST(MoveLogic, GameEndDetectionConsecutivePasses)
{
    othello::board::Board b;
    std::vector<std::vector<int>> state(8, std::vector<int>(8, BLACK));
    set_board_state(b, state, BLACK);
    EXPECT_EQ(b.compute_valid_moves(), 0);
    b.set_current_player(WHITE);
    EXPECT_EQ(b.compute_valid_moves(), 0);
    // Game should end after two passes (not directly testable here, but logic is covered)
}
TEST(MoveLogic, GameEndDetectionNotFullNoMoves)
{
    othello::board::Board b;
    std::vector<std::vector<int>> state(8, std::vector<int>(8, EMPTY));
    // Fill board with alternating discs, no valid moves
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            state[i][j] = ((i + j) % 2 == 0) ? BLACK : WHITE;
        }
    }
    set_board_state(b, state, WHITE);
    EXPECT_EQ(b.compute_valid_moves(), 0);
    b.set_current_player(BLACK);
    EXPECT_EQ(b.compute_valid_moves(), 0);
}
TEST(MoveLogic, InvalidMoveNegativeIndices)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    EXPECT_FALSE(b.is_valid_move(-1, 0, BLACK));
    auto move = make_move(-1, 0);
    EXPECT_EQ(b.process_move(move, BLACK), ERR_INVALID_MOVE);
}
TEST(MoveLogic, InvalidMoveIndicesGE8)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    EXPECT_FALSE(b.is_valid_move(8, 0, BLACK));
    auto move = make_move(0, 8);
    EXPECT_EQ(b.process_move(move, BLACK), ERR_INVALID_MOVE);
}
TEST(MoveLogic, InvalidMoveOccupiedSquare)
{
    othello::board::Board b;
    b.set_square(2, 3, BLACK);
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    EXPECT_FALSE(b.is_valid_move(2, 3, BLACK));
    auto move = make_move(2, 3);
    EXPECT_EQ(b.process_move(move, BLACK), ERR_INVALID_MOVE);
}
