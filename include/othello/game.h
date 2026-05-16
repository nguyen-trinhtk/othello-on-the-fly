#pragma once

#include "types.h"
#include "board.h"
#include "move.h"
#include "rules.h"
#include <vector>

class GameState
{
public:
    static GameState &getInstance() {
        // Singleton
        static GameState instance;
        return instance;
    }

    const Board &board() const { return m_board; }
    Player current_player() const { return m_current_player; }

    void reset();
    std::vector<Move> valid_moves() const;

    OptError play_move(const Move &move);
    void switch_turn();
    bool is_game_over() const;

private:
    // Constructors
    GameState() = default;
    GameState(const GameState &) = delete;            // no copy
    GameState &operator=(const GameState &) = delete; // no copy
    ~GameState() = default;

    Board m_board{Board::standard_start()};
    Player m_current_player = Player::BLACK;
};
