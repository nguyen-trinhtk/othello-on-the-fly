
#include <algorithm>

#include "board/board.hpp"

namespace othello
{
    namespace engine
    {

        int alphabeta(othello::board::Board board, int depth, int alpha, int beta, Disc perspective_player)
        {
            if (depth == 0 || board.is_game_over())
            {
                return board.evaluate(perspective_player);
            }

            const Disc current_player = board.get_current_player();
            const int valid_moves = board.compute_valid_moves();
            if (valid_moves == 0)
            {
                board.set_current_player(opponent(current_player));
                return alphabeta(board, depth, alpha, beta, perspective_player);
            }

            const bool maximizing = current_player == perspective_player;
            if (maximizing)
            {
                int max_eval = ALPHABETA_MIN;
                for (const auto &candidate : board.get_valid_moves())
                {
                    auto move = candidate;
                    othello::board::Board child_board = board; // copy
                    child_board.process_move(move, current_player);
                    child_board.set_current_player(opponent(current_player));
                    const int eval = alphabeta(child_board, depth - 1, alpha, beta, perspective_player);
                    max_eval = std::max(max_eval, eval);
                    alpha = std::max(alpha, eval);
                    if (beta <= alpha)
                    {
                        break;
                    }
                }
                return max_eval;
            }
            else
            {
                int min_eval = ALPHABETA_MAX;
                for (const auto &candidate : board.get_valid_moves())
                {
                    auto move = candidate;
                    othello::board::Board child_board = board; // copy
                    child_board.process_move(move, current_player);
                    child_board.set_current_player(opponent(current_player));
                    const int eval = alphabeta(child_board, depth - 1, alpha, beta, perspective_player);
                    min_eval = std::min(min_eval, eval);
                    beta = std::min(beta, eval);
                    if (beta <= alpha)
                    {
                        break;
                    }
                }
                return min_eval;
            }
        }

    } // namespace engine
} // namespace othello
