#include <gtest/gtest.h>

#include <sstream>

#include "game/console_ui.hpp"
#include "../utils.hpp"

TEST(ConsoleUiTest, ParseCommandRecognizesMoveAndCommands)
{
    const auto move_command = othello::game_ui::parse_command(" d3 ");
    ASSERT_EQ(move_command.type, othello::game_ui::CommandType::move);
    ASSERT_TRUE(move_command.move.has_value());
    EXPECT_EQ(move_command.move->row, 2);
    EXPECT_EQ(move_command.move->col, 3);

    EXPECT_EQ(
        othello::game_ui::parse_command("help").type,
        othello::game_ui::CommandType::help);
    EXPECT_EQ(
        othello::game_ui::parse_command("moves").type,
        othello::game_ui::CommandType::moves);
    EXPECT_EQ(
        othello::game_ui::parse_command("quit").type,
        othello::game_ui::CommandType::quit);
}

TEST(ConsoleUiTest, ParseCommandRejectsBadInput)
{
    const auto invalid = othello::game_ui::parse_command("z9");
    EXPECT_EQ(invalid.type, othello::game_ui::CommandType::invalid);
    EXPECT_FALSE(invalid.error_message.empty());
}

TEST(ConsoleUiTest, RenderBoardShowsHintsAndScore)
{
    othello::board::Board board;
    board.set_current_player(BLACK);
    board.compute_valid_moves();

    const std::string rendered = othello::game_ui::render_board(board);

    EXPECT_NE(rendered.find("Turn: Black"), std::string::npos);
    EXPECT_NE(rendered.find("Score  Black 2  White 2"), std::string::npos);
    EXPECT_NE(rendered.find("·"), std::string::npos);
}

TEST(ConsoleUiTest, RenderValidMovesListsMoves)
{
    othello::board::Board board;
    board.set_current_player(BLACK);
    board.compute_valid_moves();

    const std::string rendered = othello::game_ui::render_valid_moves(board);

    EXPECT_NE(rendered.find("Black valid moves:"), std::string::npos);
    EXPECT_NE(rendered.find("D3"), std::string::npos);
}

TEST(ConsoleUiTest, PromptForMoveSupportsCommandsBeforeMove)
{
    othello::board::Board board;
    board.set_current_player(BLACK);
    board.compute_valid_moves();

    std::istringstream input("moves\nscore\nD3\n");
    std::ostringstream output;
    std::ostringstream error;
    othello::game_ui::ConsoleUi ui(input, output, error);

    const auto move = ui.prompt_for_move(board);

    ASSERT_TRUE(move.has_value());
    EXPECT_EQ(move->row, 2);
    EXPECT_EQ(move->col, 3);
    EXPECT_NE(output.str().find("Black valid moves:"), std::string::npos);
    EXPECT_NE(output.str().find("Score  Black 2  White 2"), std::string::npos);
    EXPECT_TRUE(error.str().empty());
}

TEST(ConsoleUiTest, PromptForMoveReturnsNulloptOnQuit)
{
    othello::board::Board board;
    board.set_current_player(BLACK);
    board.compute_valid_moves();

    std::istringstream input("quit\n");
    std::ostringstream output;
    std::ostringstream error;
    othello::game_ui::ConsoleUi ui(input, output, error);

    const auto move = ui.prompt_for_move(board);

    EXPECT_FALSE(move.has_value());
    EXPECT_NE(output.str().find("Ending the current game."), std::string::npos);
}

TEST(ConsoleUiTest, ShowBoardDoesNotEmitAnsiByDefault)
{
    othello::board::Board board;
    std::istringstream input;
    std::ostringstream output;
    std::ostringstream error;
    othello::game_ui::ConsoleUi ui(input, output, error);

    ui.show_board(board);

    EXPECT_EQ(output.str().find("\033[2J\033[H"), std::string::npos);
}

TEST(ConsoleUiTest, ShowBoardCanEmitAnsiWhenEnabled)
{
    othello::board::Board board;
    std::istringstream input;
    std::ostringstream output;
    std::ostringstream error;
    othello::game_ui::ConsoleUi ui(input, output, error, true);

    ui.show_board(board);

    EXPECT_NE(output.str().find("\033[2J\033[H"), std::string::npos);
}

TEST(ConsoleUiTest, ShowAiThinkingPrintsMessage)
{
    std::istringstream input;
    std::ostringstream output;
    std::ostringstream error;
    othello::game_ui::ConsoleUi ui(input, output, error);

    ui.show_ai_thinking();

    EXPECT_NE(output.str().find("AI is thinking..."), std::string::npos);
}
