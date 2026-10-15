// The bridge between the web page and the C++ engine.
//
// The page sends the board as 64 characters, read row by row from a1 to h8:
//   '0' = empty, '1' = black, '2' = white
// and gets back the square the AI wants to play (0 to 63), or -1 if it has no move.

#include "othello/ai.h"
#include "eval/swh_eval.h"

#include <memory>

static std::unique_ptr<AIEngine> engine;
static int engine_depth = 0;

extern "C" int best_move(const char* squares, int color, int depth, int use_threads)
{
    // Make the engine once, and again only if the search depth changes.
    if (!engine || engine_depth != depth) {
        engine = std::make_unique<AIEngine>(depth, std::make_unique<SwhEvaluator>());
        engine_depth = depth;
    }
    engine->set_parallel(use_threads != 0);

    Board board;
    for (int square = 0; square < 64; ++square) {
        const Position position{square / 8, square % 8};
        if (squares[square] == '1')
            board.occupy_position(position, Player::BLACK);
        else if (squares[square] == '2')
            board.occupy_position(position, Player::WHITE);
    }

    const Player player = color == 1 ? Player::BLACK : Player::WHITE;
    const std::optional<Move> move = engine->best_move(board, player);
    if (!move)
        return -1;
    return move->pos.to_board_index();
}
