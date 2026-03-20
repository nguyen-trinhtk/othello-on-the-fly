#include "game/game.hpp"

#include <algorithm>
#include <iostream>

#include "board/board.hpp"
#include "engine/alphabeta.hpp"
#include "game/console_ui.hpp"

namespace
{
[[nodiscard]] std::string player_name(Disc player)
{
    return player == BLACK ? "Black" : "White";
}
} // namespace

Game::Game() {}

void Game::play_pvp()
{
    othello::board::Board board;
    othello::game_ui::ConsoleUi ui(std::cin, std::cout, std::cerr);

    ui.show_message(MSG_STARTING_GAME);
    ui.show_help();
    bool game_over = false;
    while (!game_over)
    {
        const int valid_moves_count = board.compute_valid_moves();
        ui.show_board(board);
        if (valid_moves_count == 0)
        {
            ui.show_message("No valid moves for " + player_name(board.get_current_player()) + ". Skipping turn.");
            board.set_current_player(opponent(board.get_current_player()));
            const int next_valid_moves = board.compute_valid_moves();
            ui.show_board(board);
            if (next_valid_moves == 0)
            {
                ui.show_message("No valid moves for both players. Game over.");
                game_over = true;
                continue;
            }
        }
        const auto move = ui.prompt_for_move(board);
        if (!move)
        {
            return;
        }
        auto selected_move = *move;
        board.process_move(selected_move, board.get_current_player());
        board.set_current_player(opponent(board.get_current_player()));
    }
    ui.show_board(board, false);
    ui.show_score_summary(board);
}

void Game::autoplay()
{
    othello::board::Board board;
    othello::game_ui::ConsoleUi ui(std::cin, std::cout, std::cerr);

    ui.show_message("[DEBUG] Starting autoplay...");
    bool game_over = false;
    while (!game_over)
    {
        const int valid_moves_count = board.compute_valid_moves();
        ui.show_board(board);
        if (valid_moves_count == 0)
        {
            ui.show_message("No valid moves for " + player_name(board.get_current_player()) + ". Skipping turn.");
            board.set_current_player(opponent(board.get_current_player()));
            const int next_valid_moves = board.compute_valid_moves();
            ui.show_board(board);
            if (next_valid_moves == 0)
            {
                ui.show_message("No valid moves for both players. Game over.");
                game_over = true;
                continue;
            }
        }
        othello::board::Move move = *board.get_valid_moves().begin();
        ui.show_message(player_name(board.get_current_player()) + " plays: " +
                        std::string(1, static_cast<char>('A' + move.col)) +
                        std::to_string(move.row + 1));
        board.process_move(move, board.get_current_player());
        board.set_current_player(opponent(board.get_current_player()));
    }
    ui.show_board(board, false);
    ui.show_score_summary(board);
}

void Game::play_pve()
{
    othello::board::Board board;
    othello::game_ui::ConsoleUi ui(std::cin, std::cout, std::cerr);

    ui.show_message(MSG_STARTING_GAME);
    ui.show_help();
    bool game_over = false;
    bool human_is_black = true;

    while (!game_over)
    {
        const int valid_moves_count = board.compute_valid_moves();
        ui.show_board(board);
        if (valid_moves_count == 0)
        {
            ui.show_message("No valid moves for " + player_name(board.get_current_player()) + ". Skipping turn.");
            board.set_current_player(opponent(board.get_current_player()));
            const int next_valid_moves = board.compute_valid_moves();
            ui.show_board(board);
            if (next_valid_moves == 0)
            {
                ui.show_message("No valid moves for both players. Game over.");
                game_over = true;
                continue;
            }
        }

        bool is_human_turn = (board.get_current_player() == BLACK && human_is_black) ||
                             (board.get_current_player() == WHITE && !human_is_black);

        if (is_human_turn)
        {
            const auto move = ui.prompt_for_move(board);
            if (!move)
            {
                return;
            }
            auto selected_move = *move;
            board.process_move(selected_move, board.get_current_player());
        }
        else
        {
            // AI turn
            int best_score = ALPHABETA_MIN;
            othello::board::Move best_move;
            const Disc ai_player = board.get_current_player();
            for (const auto &candidate : board.get_valid_moves())
            {
                auto move = candidate;
                othello::board::Board child = board;
                child.process_move(move, ai_player);
                child.set_current_player(opponent(ai_player));
                int score = othello::engine::alphabeta(
                    child,
                    ALPHABETA_DEPTH - 1,
                    ALPHABETA_MIN,
                    ALPHABETA_MAX,
                    ai_player);
                if (score > best_score || best_score == ALPHABETA_MIN)
                {
                    best_score = score;
                    best_move = move;
                }
            }
            board.process_move(best_move, board.get_current_player());
            ui.show_message("AI played: " +
                            std::string(1, static_cast<char>('A' + best_move.col)) +
                            std::to_string(best_move.row + 1));
        }
        board.set_current_player(opponent(board.get_current_player()));
    }
    ui.show_board(board, false);
    ui.show_score_summary(board);
}
