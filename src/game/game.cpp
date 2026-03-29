#include "game/game.hpp"

#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include "board/board.hpp"
#include "engine/alphabeta.hpp"
#include "game/console_ui.hpp"

namespace
{
    [[nodiscard]] std::string player_name(Disc player)
    {
        return player == BLACK ? "Black" : "White";
    }

    [[nodiscard]] std::string format_move(const othello::board::Move &move)
    {
        return std::string(1, static_cast<char>('A' + move.col)) +
               std::to_string(move.row + 1);
    }

    [[nodiscard]] bool advance_past_empty_turn(
        othello::board::Board &board,
        const othello::game_ui::ConsoleUi &ui)
    {
        if (board.has_valid_moves(board.get_current_player()))
        {
            return false;
        }

        ui.show_message("No valid moves for " + player_name(board.get_current_player()) + ". Skipping turn.");
        board.set_current_player(opponent(board.get_current_player()));
        ui.show_board(board);
        if (board.has_valid_moves(board.get_current_player()))
        {
            return false;
        }

        ui.show_message("No valid moves for both players. Game over.");
        return true;
    }

    template <typename MoveProvider>
    void run_game_loop(
        const othello::game_ui::ConsoleUi &ui,
        std::string_view start_message,
        bool show_help,
        MoveProvider &&move_provider)
    {
        othello::board::Board board;

        if (!start_message.empty())
        {
            ui.show_message(start_message);
        }
        if (show_help)
        {
            ui.show_help();
        }

        while (true)
        {
            ui.show_board(board);
            if (advance_past_empty_turn(board, ui))
            {
                break;
            }

            const auto move = std::invoke(move_provider, board, ui);
            if (!move.has_value())
            {
                return;
            }

            if (board.process_move(*move, board.get_current_player()) != OK)
            {
                ui.show_message("Failed to apply the selected move. Ending game.");
                return;
            }
            board.set_current_player(opponent(board.get_current_player()));
        }

        ui.show_board(board, false);
        ui.show_score_summary(board);
    }

    [[nodiscard]] std::optional<othello::board::Move> find_ai_move(
        const othello::board::Board &board,
        const othello::game_ui::ConsoleUi &ui,
        std::string_view prefix)
    {
        ui.show_ai_thinking();
        const auto search = othello::engine::find_best_move(board, ALPHABETA_DEPTH);
        if (!search.best_move.has_value())
        {
            ui.show_message(std::string(prefix) + " could not find a move.");
            return std::nullopt;
        }

        ui.show_message(
            std::string(prefix) + " " + format_move(*search.best_move) +
            " (score " + std::to_string(search.score) + ")");
        return search.best_move;
    }
} // namespace

Game::Game() {}

void Game::play_pvp()
{
    othello::game_ui::ConsoleUi ui(std::cin, std::cout, std::cerr, true);
    run_game_loop(
        ui,
        MSG_STARTING_GAME,
        true,
        [](const othello::board::Board &board, const othello::game_ui::ConsoleUi &game_ui)
        {
            return game_ui.prompt_for_move(board);
        });
}

void Game::autoplay()
{
    othello::game_ui::ConsoleUi ui(std::cin, std::cout, std::cerr, true);
    run_game_loop(
        ui,
        "Starting autoplay...",
        false,
        [](const othello::board::Board &board, const othello::game_ui::ConsoleUi &game_ui)
        {
            return find_ai_move(board, game_ui, player_name(board.get_current_player()) + " plays:");
        });
}

void Game::play_pve()
{
    othello::game_ui::ConsoleUi ui(std::cin, std::cout, std::cerr, true);
    const bool human_is_black = true;
    run_game_loop(
        ui,
        MSG_STARTING_GAME,
        true,
        [human_is_black](const othello::board::Board &board, const othello::game_ui::ConsoleUi &game_ui)
        {
            const bool is_human_turn =
                (board.get_current_player() == BLACK && human_is_black) ||
                (board.get_current_player() == WHITE && !human_is_black);

            if (is_human_turn)
            {
                return game_ui.prompt_for_move(board);
            }

            return find_ai_move(board, game_ui, "AI played:");
        });
}
