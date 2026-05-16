#pragma once

#include <string>
#include <optional>
#include "board.h"
#include "game_result.h"
#include "types.h"

class ConsoleRenderer
{
public:
    void render_board(const Board &board) const;
    void render_game_over(const GameOutcome &outcome) const;
    void render_turn(Player current_player) const;
    void clear() const;
    void display_message(const std::string &message) const;

private:
    static char player_to_char(std::optional<Player> player); // enum to char
};