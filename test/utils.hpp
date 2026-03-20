#pragma once

// Helper: fill board with a specific disc
template <typename BoardType>
void fill_board(BoardType &board, Disc disc)
{
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            board.set_square(r, c, disc);
}

// Helper: set up board with arbitrary state and player
#include <vector>
inline void set_board_state(
    othello::board::Board &board,
    const std::vector<std::vector<Disc>> &state,
    Disc player)
{
    // Clear the board first to avoid leftover discs from default setup
    fill_board(board, EMPTY);
    // Now set the custom state
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            board.set_square(r, c, state[r][c]);
    board.set_current_player(player);
    board.compute_valid_moves();
}
