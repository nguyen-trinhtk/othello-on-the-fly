#include "board/board.hpp"

namespace othello
{
    namespace board
    {

        Board::Board()
            : white_moves(INIT_BOARD_WHITE),
              black_moves(INIT_BOARD_BLACK),
              current_turn(BLACK)
        {}

    } // namespace board
} // namespace othello
