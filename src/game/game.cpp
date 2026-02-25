#include "game.hpp"
#include "board.hpp"

Game::Game() {}

void Game::play_pvp()
{
    Board board;
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
    Board board;
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
