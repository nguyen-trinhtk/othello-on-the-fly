#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include "board/board.hpp"

namespace othello
{
    namespace engine
    {
        enum class GamePhase
        {
            Early,
            Mid,
            End,
        };

        struct PhaseWeights
        {
            int actual_mobility = 0;
            int potential_mobility = 0;
            int corners = 0;
            int frontier = 0;
            int parity = 0;
            int stability = 0;
        };

        [[nodiscard]] int evaluate_position(
            const othello::board::Board &board,
            Disc perspective_player);

    } // namespace engine
} // namespace othello

#endif
