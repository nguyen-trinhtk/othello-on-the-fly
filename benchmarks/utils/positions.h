#pragma once

#include "board.h"
#include "types.h"

#include <array>

namespace bench {

inline Board board_from_string(const char* s)
{
    Board b;
    for (int i = 0; i < 64 && s[i]; ++i) {
        const char c = s[i];
        const int row = i / 8;
        const int col = i % 8;
        if (c == 'B' || c == '*' || c == 'X' || c == 'x')
            b.occupy_position({row, col}, Player::BLACK);
        else if (c == 'W' || c == 'O' || c == 'o' || c == '0')
            b.occupy_position({row, col}, Player::WHITE);
    }
    return b;
}

inline constexpr const char* MIDGAME =
    "--------"
    "--B-----"
    "WWWWW---"
    "BWWBWBB-"
    "BBWWBBB-"
    "--WWWW--"
    "--------"
    "--------";

inline constexpr const char* NEAR_END =
    "W--WWWWB"
    "-WWWWWWB"
    "WWBBWWWB"
    "WWBWWWBB"
    "WWWWWWBB"
    "---WWWWB"
    "----W--B"
    "--------";

struct PositionCase {
    const char* name;
    Board board;
    Player side;
};

inline std::array<PositionCase, 3> all_positions()
{
    return {{
        {"opening", Board::standard_start(), Player::BLACK},
        {"midgame", board_from_string(MIDGAME), Player::BLACK},
        {"near-endgame", board_from_string(NEAR_END), Player::BLACK},
    }};
}

} // namespace bench
