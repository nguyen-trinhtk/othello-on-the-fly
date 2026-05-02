#include "board.h"

Board Board::clone() const
{
    Board new_board;
    new_board.m_black_discs = m_black_discs;
    new_board.m_white_discs = m_white_discs;
    return new_board;
}

uint64_t Board::discs(Player player) const
{
    return player == Player::BLACK ? m_black_discs : m_white_discs;
}

uint64_t Board::occupied_discs() const
{
    return m_black_discs | m_white_discs;
}

void Board::apply_bit_move(Player player, uint64_t move_bit, uint64_t flip_mask)
{
    if (player == Player::BLACK)
    {
        m_black_discs |= move_bit | flip_mask;
        m_white_discs &= ~flip_mask;
        return;
    }

    m_white_discs |= move_bit | flip_mask;
    m_black_discs &= ~flip_mask;
}

OptError Board::occupy_position(const Position &pos, Player player)
{
    int r = pos.row;
    int c = pos.col;

    if (r < 0 || r >= SIZE || c < 0 || c >= SIZE)
        return GameError::OUT_OF_BOUNDS;

    uint64_t bit = 1ULL << pos.to_board_index();

    if ((m_black_discs & bit) || (m_white_discs & bit)) {
        // Cell already occupied
        return GameError::CELL_OCCUPIED; 
    }

    // Else set cell
    if (player == Player::BLACK) {
        m_black_discs |= bit;
    } else {
        m_white_discs |= bit;
    }
    return std::nullopt; // Success
}

std::optional<Player> Board::get_disc(const Position &pos) const
{
    int r = pos.row;
    int c = pos.col;

    if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) {
        return std::nullopt; // Out of bounds, TODO: maybe return ERROR
    }

    uint64_t mask = 1ULL << (r * 8 + c);
    if (m_black_discs & mask)
        return Player::BLACK;
    else if (m_white_discs & mask)
        return Player::WHITE;
    else
        return std::nullopt; // No disc at this position
}

OptError Board::flip_disc(const Position &pos, Player player) {
    int r = pos.row;
    int c = pos.col;

    if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) {
        return GameError::OUT_OF_BOUNDS;
    }

    uint64_t bit = 1ULL << pos.to_board_index();

    // Check if there's a disc to flip
    if (!((m_black_discs | m_white_discs) & bit)) {
        return GameError::INVALID_MOVE; // No disc to flip
    }

    // Check correct player
    if (player == Player::BLACK) {
        if (!(m_white_discs & bit)) {
            return GameError::INVALID_MOVE; // No white disc to flip
        }
    } else {
        if (!(m_black_discs & bit)) {
            return GameError::INVALID_MOVE; // No black disc to flip
        }
    }

    m_black_discs ^= bit;
    m_white_discs ^= bit;
    return std::nullopt; // Success
}
