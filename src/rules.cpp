#include "rules.h"

#include "bitboard.h"

namespace
{
    bool is_in_bounds(const Position &pos)
    {
        return pos.row >= 0 && pos.row < Board::SIZE &&
               pos.col >= 0 && pos.col < Board::SIZE;
    }

    Bitboards make_bitboards(const Board &board, Player player)
    {
        return Bitboards{board.discs(player), board.discs(opponent(player))};
    }
}

bool Rules::is_valid_move(const Board &board, Player player, const Position &pos)
{
    if (!is_in_bounds(pos))
        return false;

    const Bitboards bitboards = make_bitboards(board, player);
    return (Bitboard::valid_moves_mask(bitboards) & Bitboard::bit_at(pos.row, pos.col)) != 0;
}

std::vector<Move> Rules::get_all_valid_moves(const Board &board, Player player)
{
    std::vector<Move> valid_moves;
    uint64_t move_mask = Bitboard::valid_moves_mask(make_bitboards(board, player));

    while (move_mask != 0)
    {
        const int index = Bitboard::pop_lsb_index(move_mask);
        valid_moves.push_back(Move{Position{index >> 3, index & 7}});
    }

    return valid_moves;
}

OptError Rules::apply_move(Board &board, Player player, const Move &move)
{
    if (!is_in_bounds(move.pos))
        return GameError::OUT_OF_BOUNDS;

    const uint64_t move_bit = Bitboard::bit_at(move.pos.row, move.pos.col);
    if ((board.occupied_discs() & move_bit) != 0)
        return GameError::CELL_OCCUPIED;

    const Bitboards bitboards = make_bitboards(board, player);
    const uint64_t flips = Bitboard::flip_mask(bitboards, move_bit);
    if (flips == 0)
        return GameError::INVALID_MOVE;

    board.apply_bit_move(player, move_bit, flips);
    return std::nullopt;
}
