#include "game.h"

OptError GameState::play_move(const Move &move) {
    if (auto err = Rules::apply_move(m_board, m_current_player, move); err.has_value())
        return err;
    switch_turn();
    return std::nullopt;
}

// TODO: fix redundancy here
void GameState::switch_turn() {
    m_current_player = opponent(m_current_player);
    if (Rules::get_all_valid_moves(m_board, m_current_player).empty()) {
        m_current_player = opponent(m_current_player);
    }
}

bool GameState::is_game_over() const {
    return Rules::get_all_valid_moves(m_board, Player::BLACK).empty() &&
           Rules::get_all_valid_moves(m_board, Player::WHITE).empty();
}