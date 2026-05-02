#pragma once

#include "types.h"
#include "board.h"
#include "move.h"
#include "rules.h" // Use the rules namespaces later

class GameState
{
public:
    static GameState &getInstance() {
        // Singleton
        static GameState instance;
        return instance;
    }

    // Member functions
    OptError play_move(const Move &move);
    void switch_turn();
    bool is_game_over() const;

private:
    // Constructors
    GameState() = default;
    GameState(const GameState &) = delete;            // no copy
    GameState &operator=(const GameState &) = delete; // no copy
    ~GameState() = default;

    // Member vars
    Board m_board;
    Player m_current_player = Player::BLACK; // Black first
};
