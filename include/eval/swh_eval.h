#pragma once
#include "eval/eval.h"
#include "board.h"
#include "types.h"

#include <cmath>

// Standard WPC Heuristic (SWH) — Yoshioka et al., Table I in Jaśkowski 2014.
// Board encoding: +weight per black disc, -weight per white disc.
// Output negation for search: black maximizes raw WPC, white minimizes it
// (implemented as negated score from white's POV in evaluate()).
class SwhEvaluator : public IEvaluator {
public:
    int evaluate(const Board& board, Player player) override;

private:
    static constexpr float k_weights[8][8] = {
        {1.00f, -0.25f, 0.10f, 0.05f, 0.05f, 0.10f, -0.25f, 1.00f},
        {-0.25f, -0.25f, 0.01f, 0.01f, 0.01f, 0.01f, -0.25f, -0.25f},
        {0.10f, 0.01f, 0.05f, 0.02f, 0.02f, 0.05f, 0.01f, 0.10f},
        {0.05f, 0.01f, 0.02f, 0.01f, 0.01f, 0.02f, 0.01f, 0.05f},
        {0.05f, 0.01f, 0.02f, 0.01f, 0.01f, 0.02f, 0.01f, 0.05f},
        {0.10f, 0.01f, 0.05f, 0.02f, 0.02f, 0.05f, 0.01f, 0.10f},
        {-0.25f, -0.25f, 0.01f, 0.01f, 0.01f, 0.01f, -0.25f, -0.25f},
        {1.00f, -0.25f, 0.10f, 0.05f, 0.05f, 0.10f, -0.25f, 1.00f},
    };

    static float raw_wpc(const Board& board);
};

inline float SwhEvaluator::raw_wpc(const Board& board)
{
    float sum = 0.f;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            const auto disc = board.get_disc(Position{r, c});
            if (!disc)
                continue;
            const float w = k_weights[r][c];
            sum += *disc == Player::BLACK ? w : -w;
        }
    }
    return sum;
}

inline int SwhEvaluator::evaluate(const Board& board, Player player)
{
    const float raw = raw_wpc(board);
    const float score = player == Player::BLACK ? raw : -raw;
    return static_cast<int>(std::round(score * 100.f));
}
