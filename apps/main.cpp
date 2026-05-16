#include "othello/ai.h"
#include "othello/board.h"
#include "othello/game.h"
#include "othello/game_result.h"
#include "othello/move.h"
#include "othello/render.h"
#include "othello/types.h"

#include <iostream>
#include <optional>
#include <sstream>
#include <string>

namespace
{
    // Game loop helpers
    constexpr int SEARCH_DEPTH = 5;

    const char *error_text(GameError e)
    {
        switch (e)
        {
        case GameError::OUT_OF_BOUNDS:
            return "Out of bounds.";
        case GameError::CELL_OCCUPIED:
            return "That square is already occupied.";
        case GameError::INVALID_MOVE:
            return "Illegal move (must bracket opponent discs in a line).";
        }
        return "Move rejected.";
    }

    // std::nullopt = quit (q) or EOF
    std::optional<Move> read_human_move()
    {
        for (;;)
        {
            std::cout << "Enter row col (0-7 each), or q to quit: " << std::flush;
            std::string line;
            if (!std::getline(std::cin, line))
                return std::nullopt;

            if (line == "q" || line == "Q")
                return std::nullopt;

            std::istringstream iss(line);
            int r = -1;
            int c = -1;
            if (iss >> r >> c && r >= 0 && r < Board::SIZE && c >= 0 && c < Board::SIZE)
                return Move{Position{r, c}};

            std::cout << "Invalid input; try again (e.g. \"3 4\").\n";
        }
    }

    void print_usage(const char *argv0)
    {
        std::cerr << "Usage: " << (argv0 ? argv0 : "othello") << "\n"
                  << "  Human (Black) vs AI (White). No options.\n";
    }

    void run_pve(GameState &game, AIEngine &ai)
    {
        ConsoleRenderer view;
        game.reset();

        view.display_message("Game started!");
        view.display_message("Black (Player) vs White (AI)");

        while (!game.is_game_over())
        {
            view.render_board(game.board());
            view.render_turn(game.current_player());

            const auto moves = game.valid_moves();
            std::ostringstream hint;
            hint << "Legal moves (" << moves.size() << "): ";
            // Hint
            for (std::size_t i = 0; i < moves.size(); ++i)
            {
                if (i != 0)
                    hint << ' ';
                hint << '(' << moves[i].pos.row << ',' << moves[i].pos.col << ')';
            }
            view.display_message(hint.str());

            std::optional<Move> chosen;
            if (game.current_player() == Player::BLACK)
            {
                chosen = read_human_move();
                if (!chosen.has_value())
                    return;
            }
            else
            {
                chosen = ai.best_move(game.board(), game.current_player());
                if (!chosen.has_value())
                {
                    view.display_message("AI reported no legal move (unexpected).");
                    continue;
                }
            }

            if (const OptError err = game.play_move(*chosen); err.has_value())
            {
                view.display_message(error_text(*err));
                continue;
            }
        }

        view.render_board(game.board());
        view.render_game_over(outcome_from_board(game.board()));
    }
} // namespace

int main(int argc, char **argv)
{
    if (argc != 1)
    {
        print_usage(argv[0]);
        return 1;
    }

    AIEngine ai(SEARCH_DEPTH);
    run_pve(GameState::getInstance(), ai);
    return 0;
}
