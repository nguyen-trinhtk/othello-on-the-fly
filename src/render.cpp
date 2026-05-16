#include "othello/render.h"
#include <iostream>

void ConsoleRenderer::render_board(const Board &board) const
{
    // TODO: better printing
    std::cout << "\n  0 1 2 3 4 5 6 7\n";
    for (int r = 0; r < Board::SIZE; ++r)
    {
        std::cout << r << " ";
        for (int c = 0; c < Board::SIZE; ++c)
        {
            auto disc = board.get_disc({r, c});
            std::cout << player_to_char(disc) << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void ConsoleRenderer::render_game_over(const GameOutcome &outcome) const
{
    std::cout << "Game Over! Black: " << outcome.black_count << "  White: " << outcome.white_count << "\n";
    if (!outcome.winner.has_value())
    {
        std::cout << "Tie.\n";
        return;
    }
    std::cout << (outcome.winner.value() == Player::BLACK ? "Black wins!\n" : "White wins!\n");
}

void ConsoleRenderer::render_turn(Player current_player) const
{
    std::cout << "Current player: " << (current_player == Player::BLACK ? "BLACK" : "WHITE") << "\n";
}

void ConsoleRenderer::clear() const
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void ConsoleRenderer::display_message(const std::string &message) const
{
    std::cout << message << "\n";
}

char ConsoleRenderer::player_to_char(std::optional<Player> player)
{
    if (!player.has_value())
        return '.';
    return player.value() == Player::BLACK ? 'B' : 'W';
}
