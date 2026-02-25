// #include "board.hpp"

// int alphabeta(Board board, int depth, int alpha, int beta, bool maximizingPlayer)
// {
//     if (depth == 0 || board.is_game_over())
//     {
//         return evaluate(board);
//     }
//     board.eval_valid_moves();
//     if (maximizingPlayer)
//     {
//         int maxEval = -INF;
//         for (auto move : board.get_valid_moves())
//         {
//             Board child = board; // copy
//             child.process_move(move.first, move.second, true);
//             int eval = alphabeta(child, depth - 1, alpha, beta, false);
//             maxEval = std::max(maxEval, eval);
//             alpha = std::max(alpha, eval);
//             if (beta <= alpha)
//                 break;
//         }
//         return maxEval;
//     }
//     else
//     {
//         int minEval = INF;
//         for (auto move : board.get_valid_moves())
//         {
//             Board child = board; // copy
//             child.process_move(move.first, move.second, false);
//             int eval = alphabeta(child, depth - 1, alpha, beta, true);
//             minEval = std::min(minEval, eval);
//             beta = std::min(beta, eval);
//             if (beta <= alpha)
//                 break;
//         }
//         return minEval;
//     }
// }