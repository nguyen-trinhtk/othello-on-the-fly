#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include "board.h"
#include "types.h"

// Systematic all-2 n-tuple network (288 weights) from Jaśkowski 2014.
// Architecture: 32 canonical 2-tuples covering all adjacent pairs on the 8×8
// board, each symmetry-expanded under D4 (rotations + reflections).
// Evaluation uses board inversion: always scores from the current mover's
// perspective by treating its pieces as "black" and the opponent's as "white".

namespace NTuple {

// A pair of linear board positions [0,63], always stored with a < b.
struct Tuple2 { int a, b; };

// One symmetry group: a canonical representative + all unique D4 expansions.
struct TupleGroup {
    Tuple2 canonical;
    std::vector<Tuple2> syms;  // 2, 4, or 8 entries
};

// Returns the 32 all-2 canonical groups (computed once, then cached).
const std::vector<TupleGroup>& all2_groups();

class Net {
public:
    static constexpr int N_GROUPS = 32;
    static constexpr int LUT_SIZE = 9;   // 3^2 patterns per 2-tuple

    using LUT     = std::array<float, LUT_SIZE>;
    using Weights = std::array<LUT, N_GROUPS>;
    using Grad    = std::array<LUT, N_GROUPS>;

    Weights w{};  // zero-initialized; load() or mutate() to populate

    // Score from player's POV (board inversion: treat player's pieces as black)
    float evaluate(const Board& board, Player player) const;

    // Per-weight activation count: grad[g][j] = times w[g][j] was read
    Grad  gradient(const Board& board, Player player) const;

    // w[g][j] += alpha * delta * grad[g][j]
    void  update(const Grad& grad, float delta, float alpha);

    // Binary weight I/O (32 × 9 × sizeof(float) = 1152 bytes)
    bool  save(const std::string& path) const;
    bool  load(const std::string& path);

private:
    float eval_bbs(uint64_t my_bb, uint64_t opp_bb) const;
    Grad  grad_bbs(uint64_t my_bb, uint64_t opp_bb) const;

    // 0 = white (opponent), 1 = empty, 2 = black (current player)
    static int cell_val(uint64_t my_bb, uint64_t opp_bb, int pos);
    // Base-3 index for a 2-tuple: va*3^0 + vb*3^1
    static int lut_idx(int va, int vb) { return va + 3 * vb; }
};

} // namespace NTuple
