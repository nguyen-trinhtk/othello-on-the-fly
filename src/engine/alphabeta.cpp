#include "board.hpp"

int alphabeta(Board board, int depth, int alpha, int beta, bool maximizing_player) {
    if (depth == 0 || board.is_game_over()) {
        return board.evaluate(maximizing_player ? BLACK : WHITE);
    }
    board.compute_valid_moves();
    if (maximizing_player) {
        int max_eval = -1000000;
        for (auto move : board.get_valid_moves()) {
            Board child_board = board; // copy
            child_board.process_move(move.first, move.second, BLACK);
            int eval = alphabeta(child_board, depth - 1, alpha, beta, false);
            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return max_eval;
    } else {
        int min_eval = 1000000;
        for (auto move : board.get_valid_moves()) {
            Board child_board = board; // copy
            child_board.process_move(move.first, move.second, WHITE);
            int eval = alphabeta(child_board, depth - 1, alpha, beta, true);
            min_eval = std::min(min_eval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return min_eval;
    }
}