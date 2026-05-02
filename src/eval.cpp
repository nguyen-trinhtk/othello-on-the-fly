#include "eval.h"
#include <bit>

int Eval::score(const Board& board, Player player)
{
    // Pure count for now
    // TODO: add n-tuple NN
    const int my_discs = std::popcount(board.discs(player));
    const int opponent_discs = std::popcount(board.discs(opponent(player)));
    return my_discs - opponent_discs;
}