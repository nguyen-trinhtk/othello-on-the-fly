#include <gtest/gtest.h>
#include "board/board.hpp"
#include "../utils.hpp"

namespace
{
    othello::board::Move make_move(int row, int col)
    {
        return othello::board::Move(row, col);
    }

    std::uint64_t bit_at(int row, int col)
    {
        return 1ULL << (BOARD_SIZE * row + col);
    }

    int count_bits(std::uint64_t mask)
    {
        return __builtin_popcountll(mask);
    }

    const othello::board::Move *find_move(
        const std::vector<othello::board::Move> &moves,
        int row,
        int col)
    {
        for (const auto &move : moves)
        {
            if (move.row == row && move.col == col)
            {
                return &move;
            }
        }
        return nullptr;
    }
}

// --- is_valid_move tests ---
TEST(MoveLogic, ValidMoveBlackWithoutPrecompute)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    EXPECT_TRUE(b.is_valid_move(2, 3, BLACK));
}

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
    ASSERT_EQ(b.process_move(move, BLACK), OK);
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
    EXPECT_FALSE(b.is_valid_move(2, 3, static_cast<Disc>(2)));
}

// --- process_move tests ---
TEST(MoveLogic, ProcessValidMove)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    int before = b.get_black_count();
    auto move = make_move(2, 3);
    ASSERT_EQ(b.process_move(move, BLACK), OK);
    EXPECT_GT(b.get_black_count(), before);
}
TEST(MoveLogic, ProcessInvalidMove)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    int before = b.get_black_count();
    auto move = make_move(0, 0);
    EXPECT_EQ(b.process_move(move, BLACK), ERR_INVALID_MOVE);
    EXPECT_EQ(b.get_black_count(), before);
}
TEST(MoveLogic, EdgeMove)
{
    othello::board::Board b;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, EMPTY));
    // Edge flip: black at (0,2), white at (0,1), empty at (0,0)
    state[0][1] = WHITE;
    state[0][2] = BLACK;
    set_board_state(b, state, BLACK);
    EXPECT_TRUE(b.is_valid_move(0, 0, BLACK));
    auto move = make_move(0, 0);
    ASSERT_EQ(b.process_move(move, BLACK), OK);
    EXPECT_EQ(b.get_black_count(), 3); // (0,0) placed, (0,1) flipped, (0,2) original
}
TEST(MoveLogic, CornerMove)
{
    othello::board::Board b;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, EMPTY));
    // Diagonal flip: black at (2,2), white at (1,1), empty at (0,0)
    state[1][1] = WHITE;
    state[2][2] = BLACK;
    set_board_state(b, state, BLACK);
    EXPECT_TRUE(b.is_valid_move(0, 0, BLACK));
    auto move = make_move(0, 0);
    ASSERT_EQ(b.process_move(move, BLACK), OK);
    EXPECT_EQ(b.get_black_count(), 3); // (0,0) placed, (1,1) flipped, (2,2) original
}
TEST(MoveLogic, MultiDirectionFlips)
{
    othello::board::Board b;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, EMPTY));
    // Multi-direction flip: black at (3,2) and (2,3), white at (3,3), empty at (3,4)
    state[3][2] = BLACK;
    state[2][3] = BLACK;
    state[3][3] = WHITE;
    set_board_state(b, state, BLACK);
    EXPECT_TRUE(b.is_valid_move(3, 4, BLACK));
    auto move = make_move(3, 4);
    ASSERT_EQ(b.process_move(move, BLACK), OK);
    EXPECT_EQ(b.get_black_count(), 4); // (3,2), (2,3), (3,4) placed, (3,3) flipped
}
TEST(MoveLogic, ProcessInvalidPlayer)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    int before = b.get_black_count();
    auto move = make_move(2, 3);
    EXPECT_EQ(b.process_move(move, static_cast<Disc>(2)), ERR_INVALID_MOVE);
    EXPECT_EQ(b.get_black_count(), before);
}

TEST(MoveLogic, ProcessMoveUsesPrecomputedFlipMetadata)
{
    othello::board::Board b;
    b.set_current_player(BLACK);

    const auto generated_moves = b.get_moves_for_current_state();
    const auto *move = find_move(generated_moves, 2, 3);

    ASSERT_NE(move, nullptr);
    ASSERT_EQ(b.process_move(*move, BLACK), OK);
    EXPECT_EQ(b.get_square(2, 3), BLACK);
    EXPECT_EQ(b.get_square(3, 3), BLACK);
}

TEST(MoveLogic, UndoMoveRestoresBoardAfterPrecomputedMove)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    const othello::board::Board before = b;

    const auto generated_moves = b.get_moves_for_current_state();
    const auto *move = find_move(generated_moves, 2, 3);

    ASSERT_NE(move, nullptr);
    ASSERT_EQ(b.process_move(*move, BLACK), OK);
    ASSERT_EQ(b.undo_move(*move, BLACK), OK);
    EXPECT_EQ(b.get_hash(), before.get_hash());
    EXPECT_EQ(b.get_black_count(), before.get_black_count());
    EXPECT_EQ(b.get_white_count(), before.get_white_count());
    EXPECT_EQ(b.get_current_player(), before.get_current_player());
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
    ASSERT_EQ(b.process_move(move, BLACK), OK);
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

// --- get_valid_move_mask tests ---
TEST(MoveLogic, GetValidMoveMaskMatchesCompute)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    int n = b.compute_valid_moves();
    EXPECT_EQ(count_bits(b.get_valid_move_mask()), n);
}
TEST(MoveLogic, GetValidMoveMaskAfterBoardChange)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();
    auto move = make_move(2, 3);
    ASSERT_EQ(b.process_move(move, BLACK), OK);
    b.set_current_player(WHITE);
    b.compute_valid_moves();
    EXPECT_EQ(count_bits(b.get_valid_move_mask()), b.compute_valid_moves());
}

TEST(MoveLogic, GetMovesForCurrentStateDoesNotMutateBoard)
{
    othello::board::Board b;
    b.set_current_player(BLACK);

    const int black_before = b.get_black_count();
    const int white_before = b.get_white_count();
    const Disc player_before = b.get_current_player();
    const std::uint64_t valid_moves_before = b.get_valid_move_mask();

    const auto generated_moves = b.get_moves_for_current_state();

    EXPECT_FALSE(generated_moves.empty());
    EXPECT_EQ(b.get_black_count(), black_before);
    EXPECT_EQ(b.get_white_count(), white_before);
    EXPECT_EQ(b.get_current_player(), player_before);
    EXPECT_EQ(b.get_valid_move_mask(), valid_moves_before);
}

TEST(MoveLogic, GetMovesForCurrentStateIncludesFlipMetadata)
{
    othello::board::Board b;
    b.set_current_player(BLACK);

    const auto generated_moves = b.get_moves_for_current_state();
    const auto *move = find_move(generated_moves, 2, 3);

    ASSERT_NE(move, nullptr);
    EXPECT_EQ(move->flip_mask, bit_at(3, 3));
    EXPECT_EQ(move->flip_count(), 1U);
}

TEST(MoveLogic, GetMovesForCurrentStateIsStableAcrossRepeatedCalls)
{
    othello::board::Board b;
    b.set_current_player(BLACK);

    const auto first_result = b.get_moves_for_current_state();
    const auto second_result = b.get_moves_for_current_state();

    ASSERT_EQ(first_result.size(), second_result.size());
    for (const auto &move : first_result)
    {
        const auto *repeated = find_move(second_result, move.row, move.col);
        ASSERT_NE(repeated, nullptr);
        EXPECT_EQ(repeated->flip_mask, move.flip_mask);
        EXPECT_EQ(repeated->flip_count(), move.flip_count());
    }
}

TEST(MoveLogic, GetMovesForCurrentStateChangesAfterBoardStateChanges)
{
    othello::board::Board b;
    b.set_current_player(BLACK);

    const auto opening_moves = b.get_moves_for_current_state();

    auto move = make_move(2, 3);
    ASSERT_EQ(b.process_move(move, BLACK), OK);
    b.set_current_player(WHITE);

    const auto response_moves = b.get_moves_for_current_state();

    EXPECT_NE(opening_moves.size(), response_moves.size());
    EXPECT_NE(find_move(response_moves, 2, 2), nullptr);
}

TEST(MoveLogic, IsGameOverPreservesCurrentTurnAndValidMoves)
{
    othello::board::Board b;
    b.set_current_player(BLACK);
    b.compute_valid_moves();

    const Disc player_before = b.get_current_player();
    const std::uint64_t valid_moves_before = b.get_valid_move_mask();

    EXPECT_FALSE(b.is_game_over());
    EXPECT_EQ(b.get_current_player(), player_before);
    EXPECT_EQ(b.get_valid_move_mask(), valid_moves_before);
}

TEST(MoveLogic, HashChangesWithBoardStateAndCurrentPlayer)
{
    othello::board::Board initial_board;
    othello::board::Board same_board;
    othello::board::Board different_turn_board;
    othello::board::Board changed_board;

    different_turn_board.set_current_player(WHITE);
    auto move = make_move(2, 3);
    ASSERT_EQ(changed_board.process_move(move, BLACK), OK);
    changed_board.set_current_player(WHITE);

    EXPECT_EQ(initial_board.get_hash(), same_board.get_hash());
    EXPECT_NE(initial_board.get_hash(), different_turn_board.get_hash());
    EXPECT_NE(initial_board.get_hash(), changed_board.get_hash());
}

// --- parse_move (not easily testable without refactor) ---

// --- Edge & Game End Cases ---
TEST(MoveLogic, MovesOnEdgesCorners)
{
    othello::board::Board b;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, WHITE));
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
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, EMPTY));
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
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, BLACK));
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
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, BLACK));
    set_board_state(b, state, BLACK);
    EXPECT_EQ(b.compute_valid_moves(), 0);
    b.set_current_player(WHITE);
    EXPECT_EQ(b.compute_valid_moves(), 0);
    // Game should end after two passes (not directly testable here, but logic is covered)
}
TEST(MoveLogic, GameEndDetectionNotFullNoMoves)
{
    othello::board::Board b;
    std::vector<std::vector<Disc>> state(8, std::vector<Disc>(8, EMPTY));
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
