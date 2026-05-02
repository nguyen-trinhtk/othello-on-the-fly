#include "othello/ai.h"
#include "othello/game.h"

constexpr int SEARCH_DEPTH = 5;

int main() {
    AIEngine ai(SEARCH_DEPTH);
    GameState& game = GameState::getInstance();

    // while (!game.is_game_over()) {
    //     Move move = ai.best_move(game.board(), game.current_player());
    //     game.play_move(move);
    // }

    return 0;
}