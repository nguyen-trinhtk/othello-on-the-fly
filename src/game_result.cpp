#include "othello/game_result.h"

#include <bit>

GameOutcome outcome_from_board(const Board &board)
{
    const int black_count = static_cast<int>(std::popcount(board.discs(Player::BLACK)));
    const int white_count = static_cast<int>(std::popcount(board.discs(Player::WHITE)));

    GameOutcome out{};
    out.black_count = black_count;
    out.white_count = white_count;
    out.winner = std::nullopt;

    if (black_count > white_count)
        out.winner = Player::BLACK;
    else if (white_count > black_count)
        out.winner = Player::WHITE;

    return out;
}
