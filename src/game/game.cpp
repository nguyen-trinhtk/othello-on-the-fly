#include "game/game.hpp"
#include "board/board.hpp"
#include "engine/alphabeta.hpp"

Game::Game() {}

void Game::play_pvp()
{
    othello::board::Board board;
    std::cout << MSG_STARTING_GAME << std::endl;
    bool game_over = false;
    while (!game_over)
    {
        board.print_board();
        int valid_moves_count = board.compute_valid_moves();
        if (valid_moves_count == 0)
        {
            std::cout << "No valid moves for " << ((board.get_current_player() == BLACK) ? "Black" : "White") << ". Skipping turn." << std::endl;
            board.set_current_player(!board.get_current_player());
            valid_moves_count = board.compute_valid_moves();
            if (valid_moves_count == 0)
            {
                std::cout << "No valid moves for both players. Game over." << std::endl;
                game_over = true;
                continue;
            }
        }
        std::pair<int, int> move = board.parse_move();
        board.process_move(move.first, move.second, board.get_current_player());
        board.set_current_player(!board.get_current_player());
    }
    board.print_board();
    board.sum_game_stats();
}

void Game::autoplay()
{
    othello::board::Board board;
    std::cout << "[DEBUG] Starting autoplay..." << std::endl;
    bool game_over = false;
    while (!game_over)
    {
        board.print_board();
        int valid_moves_count = board.compute_valid_moves();
        if (valid_moves_count == 0)
        {
            std::cout << "No valid moves for " << ((board.get_current_player() == BLACK) ? "Black" : "White") << ". Skipping turn." << std::endl;
            board.set_current_player(!board.get_current_player());
            valid_moves_count = board.compute_valid_moves();
            if (valid_moves_count == 0)
            {
                std::cout << "No valid moves for both players. Game over." << std::endl;
                game_over = true;
                continue;
            }
        }
        std::pair<int, int> move = *board.get_valid_moves().begin();
        std::cout << ((board.get_current_player() == BLACK) ? "Black" : "White") << " plays: " << char('A' + move.second) << (move.first + 1) << std::endl;
        board.process_move(move.first, move.second, board.get_current_player());
        board.set_current_player(!board.get_current_player());
    }
    board.print_board();
    board.sum_game_stats();
}

void Game::play_pve()
{
    othello::board::Board board;
    std::cout << MSG_STARTING_GAME << std::endl;
    bool game_over = false;
    bool human_is_black = true;

    while (!game_over)
    {
        board.print_board();
        int valid_moves_count = board.compute_valid_moves();
        if (valid_moves_count == 0)
        {
            std::cout << "No valid moves for " << (board.get_current_player() == BLACK ? "Black" : "White") << ". Skipping turn." << std::endl;
            board.set_current_player(!board.get_current_player());
            valid_moves_count = board.compute_valid_moves();
            if (valid_moves_count == 0)
            {
                std::cout << "No valid moves for both players. Game over." << std::endl;
                game_over = true;
                continue;
            }
        }

        bool is_human_turn = (board.get_current_player() == BLACK && human_is_black) ||
                             (board.get_current_player() == WHITE && !human_is_black);

        if (is_human_turn)
        {
            std::pair<int, int> move = board.parse_move();
            board.process_move(move.first, move.second, board.get_current_player());
        }
        else
        {
            // AI turn
            int best_score = ALPHABETA_MIN;
            std::pair<int, int> best_move;
            for (auto move : board.get_valid_moves())
            {
                othello::board::Board child = board;
                child.process_move(move.first, move.second, board.get_current_player());
                int score = othello::engine::alphabeta(child, ALPHABETA_DEPTH, ALPHABETA_MIN, ALPHABETA_MAX, !board.get_current_player());
                if (score > best_score || best_score == ALPHABETA_MIN)
                {
                    best_score = score;
                    best_move = move;
                }
            }
            board.process_move(best_move.first, best_move.second, board.get_current_player());
            std::cout << "AI played: " << char('A' + best_move.second) << (best_move.first + 1) << std::endl;
        }
        board.set_current_player(!board.get_current_player());
    }
    board.print_board();
    board.sum_game_stats();
}
