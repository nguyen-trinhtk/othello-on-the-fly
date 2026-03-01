#ifndef ALPHABETA_HPP
#define ALPHABETA_HPP

#include "board/board.hpp"

namespace othello
{
    namespace engine
    {

        int alphabeta(othello::board::Board board, int depth, int alpha, int beta, bool maximizing_player);

    } // namespace engine
} // namespace othello

#endif
