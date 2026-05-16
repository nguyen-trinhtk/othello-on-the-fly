#include "eval/component_eval.h"

#include "bitboard.h"
#include "rules.h"

#include <array>
#include <bit>
#include <cstdlib>

namespace
{
    constexpr int BOARD_SIZE = 8;
    constexpr uint64_t NOT_FILE_A = 0xfefefefefefefefeULL;
    constexpr uint64_t NOT_FILE_H = 0x7f7f7f7f7f7f7f7fULL;
    constexpr std::array<Position, 4> CORNERS = {
        Position{0, 0},
        Position{0, 7},
        Position{7, 0},
        Position{7, 7},
    };

    uint64_t shift_north(uint64_t bits)
    {
        return bits >> 8;
    }

    uint64_t shift_south(uint64_t bits)
    {
        return bits << 8;
    }

    uint64_t shift_east(uint64_t bits)
    {
        return (bits << 1) & NOT_FILE_A;
    }

    uint64_t shift_west(uint64_t bits)
    {
        return (bits >> 1) & NOT_FILE_H;
    }

    uint64_t shift_north_east(uint64_t bits)
    {
        return (bits >> 7) & NOT_FILE_A;
    }

    uint64_t shift_north_west(uint64_t bits)
    {
        return (bits >> 9) & NOT_FILE_H;
    }

    uint64_t shift_south_east(uint64_t bits)
    {
        return (bits << 9) & NOT_FILE_A;
    }

    uint64_t shift_south_west(uint64_t bits)
    {
        return (bits << 7) & NOT_FILE_H;
    }

    uint64_t adjacent_squares(uint64_t bits)
    {
        return shift_north(bits) |
               shift_south(bits) |
               shift_east(bits) |
               shift_west(bits) |
               shift_north_east(bits) |
               shift_north_west(bits) |
               shift_south_east(bits) |
               shift_south_west(bits);
    }

    void add_stable_edge_run(uint64_t player_discs, int start_row, int start_col, int row_step, int col_step, uint64_t& stable_discs)
    {
        int row = start_row;
        int col = start_col;

        while (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE)
        {
            uint64_t square = Bitboard::bit_at(row, col);
            if ((player_discs & square) == 0)
                return;

            stable_discs |= square;
            row += row_step;
            col += col_step;
        }
    }

    uint64_t stable_edge_discs(const Board& board, Player player)
    {
        uint64_t stable_discs = 0;
        uint64_t player_discs = board.discs(player);

        if ((player_discs & Bitboard::bit_at(0, 0)) != 0)
        {
            add_stable_edge_run(player_discs, 0, 0, 0, 1, stable_discs);
            add_stable_edge_run(player_discs, 0, 0, 1, 0, stable_discs);
        }
        if ((player_discs & Bitboard::bit_at(0, 7)) != 0)
        {
            add_stable_edge_run(player_discs, 0, 7, 0, -1, stable_discs);
            add_stable_edge_run(player_discs, 0, 7, 1, 0, stable_discs);
        }
        if ((player_discs & Bitboard::bit_at(7, 0)) != 0)
        {
            add_stable_edge_run(player_discs, 7, 0, 0, 1, stable_discs);
            add_stable_edge_run(player_discs, 7, 0, -1, 0, stable_discs);
        }
        if ((player_discs & Bitboard::bit_at(7, 7)) != 0)
        {
            add_stable_edge_run(player_discs, 7, 7, 0, -1, stable_discs);
            add_stable_edge_run(player_discs, 7, 7, -1, 0, stable_discs);
        }

        return stable_discs;
    }

    uint64_t immediately_flippable_discs(const Board& board, Player player)
    {
        uint64_t flippable_discs = 0;
        Player other_player = opponent(player);
        Bitboards bitboards{board.discs(other_player), board.discs(player)};

        for (const Move& move : Rules::get_all_valid_moves(board, other_player))
        {
            uint64_t move_bit = Bitboard::bit_at(move.pos.row, move.pos.col);
            flippable_discs |= Bitboard::flip_mask(bitboards, move_bit);
        }

        return flippable_discs & board.discs(player);
    }
}

int ComponentEvaluator::evaluate(const Board& board, Player player) {
    int coin_parity_val = coin_parity(board, player);
    int mobility_val = mobility(board, player);
    int corner_val = corner(board, player);
    int stability_val = stability(board, player);

    return m_coin_parity_weight * coin_parity_val 
            + m_mobility_weight * mobility_val 
            + m_corner_weight * corner_val 
            + m_stability_weight * stability_val;
}


int ComponentEvaluator::derive_heuristic_value(int current_player_value, int opponent_value) {
    if (current_player_value + opponent_value != 0) {
        return 100 * (current_player_value - opponent_value) / (current_player_value + opponent_value);
    }
    return 0;
}

int ComponentEvaluator::derive_signed_heuristic_value(int current_player_value, int opponent_value) {
    int denominator = std::abs(current_player_value) + std::abs(opponent_value);
    if (denominator != 0) {
        return 100 * (current_player_value - opponent_value) / denominator;
    }
    return 0;
}

int ComponentEvaluator::coin_parity(const Board& board, Player player) {
    int current_player_value = std::popcount(board.discs(player));
    int opponent_value = std::popcount(board.discs(opponent(player)));
    return derive_heuristic_value(current_player_value, opponent_value);
}

int ComponentEvaluator::mobility(const Board& board, Player player) {
    int current_actual_mobility = static_cast<int>(Rules::get_all_valid_moves(board, player).size());
    int opponent_actual_mobility = static_cast<int>(Rules::get_all_valid_moves(board, opponent(player)).size());

    uint64_t empty_squares = ~board.occupied_discs();
    int current_potential_mobility = std::popcount(adjacent_squares(board.discs(opponent(player))) & empty_squares);
    int opponent_potential_mobility = std::popcount(adjacent_squares(board.discs(player)) & empty_squares);

    int current_player_value = current_actual_mobility + current_potential_mobility;
    int opponent_value = opponent_actual_mobility + opponent_potential_mobility;
    return derive_heuristic_value(current_player_value, opponent_value);
}

int ComponentEvaluator::corner(const Board& board, Player player) {
    int current_player_value = count_corners(board, player);
    int opponent_value = count_corners(board, opponent(player));
    return derive_heuristic_value(current_player_value, opponent_value);
}

int ComponentEvaluator::stability(const Board& board, Player player) {
    int current_player_value = stability_score(board, player);
    int opponent_value = stability_score(board, opponent(player));
    return derive_signed_heuristic_value(current_player_value, opponent_value);
}

int ComponentEvaluator::count_corners(const Board& board, Player player) {
    int corners = 0;
    for (const Position& corner_pos : CORNERS) {
        if (board.get_disc(corner_pos) == player) {
            ++corners;
        }
    }
    return corners;
}

int ComponentEvaluator::stability_score(const Board& board, Player player) {
    uint64_t stable_discs = stable_edge_discs(board, player);
    uint64_t unstable_discs = immediately_flippable_discs(board, player) & ~stable_discs;

    int stable_count = std::popcount(stable_discs);
    int unstable_count = std::popcount(unstable_discs);
    return stable_count - unstable_count;
}